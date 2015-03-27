#include "sync_engine.h"
#include <thread>
#include <chrono>
#include "message_convertor.hpp"
using namespace sioclient;

typedef std::chrono::duration<int> seconds_type;

namespace mmp
{
    
    using namespace message_convertor;
    
    sync_engine::user_manager::user_manager(sync_engine* engine):m_engine(engine),m_me()
    {
    }
    sync_engine::user_manager::~user_manager()
    {
    }
    
    void sync_engine::user_manager::signup(const user_signup_def& signup_def,callback_func callback)
    {


        message::ptr  msg;
        convert_user_signup_def(msg,signup_def);
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_SIGNUP,msg);
        m_engine->m_callback_mapping[proto_constants::EVENT_USER_SIGNUP] = callback;
    }
    void sync_engine::user_manager::signin(const user_signin_def& signin_def,callback_func callback)
    {
        //TODO:call socket.io interfaces.
        message::ptr  msg;
        convert_user_signin_def(msg, signin_def);
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_SIGNIN,msg);
        
        m_engine->m_callback_mapping[proto_constants::EVENT_USER_SIGNIN] = callback;
    }
    void sync_engine::user_manager::trial(const user_trial_def& trial_def,callback_func callback)
    {
        //TODO:call socket.io interfaces.
        message::ptr  msg;
        convert_user_trial_def(msg, trial_def);
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_TRIAL,msg);
        m_engine->m_callback_mapping[proto_constants::EVENT_USER_TRIAL] = callback;
    }
    const user* sync_engine::user_manager::me()
    {
        return m_me.get();
    }
    
    sync_engine::room_manager::room_manager(sync_engine* engine):m_engine(engine),m_room()
    {
    }
    sync_engine::room_manager::~room_manager()
    {
    }
    
    void sync_engine::room_manager::create(const room_def& room_def,callback_func callback)
    {
        if(!m_engine->m_usermgr.m_me)
        {
            return;
        }
        message::ptr  msg;
        convert_room_def(msg, room_def);
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_CREATE,msg);
        m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_CREATE] = callback;
    }
    
    void sync_engine::room_manager::join(id_type room_id,callback_func callback)
    {
        if(!m_engine->m_usermgr.m_me)
        {
            return;
        }
        message::ptr msg;
        convert_id(msg, room_id);
       
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_JOIN,msg);
        m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_JOIN] = callback;
    }
    
    void sync_engine::room_manager::leave(id_type room_id,callback_func callback)
    {
        if(!m_engine->m_roommgr.m_room)
        {
            return;
        }
        message::ptr msg;
        convert_id(msg, room_id);
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_LEAVE,msg);
        m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_LEAVE] = callback;
    }
    
    void sync_engine::room_manager::find_room_by_name(const std::string& name, callback_func callback)
    {
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_FIND_BY_NAME,name);
        m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_FIND_BY_NAME] = callback;
    }
    
    const room* sync_engine::room_manager::current_room()
    {
        return m_room.get();
    }
    
    sync_engine::sync_engine():m_roommgr(this),m_usermgr(this),m_interval(2),m_listener(NULL),m_client_handler_ptr(new sioclient::handler()),m_last_publish_time(0),m_connected(false)
    {
        m_client_handler_ptr->set_connect_listener(std::bind(&sync_engine::on_connected,this));
        m_client_handler_ptr->set_open_listener(std::bind(&sync_engine::on_open,this));
        m_client_handler_ptr->set_fail_listener(std::bind(&sync_engine::on_fail,this));
        m_client_handler_ptr->set_close_listener(std::bind(&sync_engine::on_close,this,std::placeholders::_1));
        
        m_client_handler_ptr->set_default_event_listener(std::bind(&sync_engine::on_socketio_event,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4));
    }
    
    sync_engine::~sync_engine(void)
    {
        m_client_handler_ptr->clear_con_listeners();
        m_client_handler_ptr->clear_socketio_listeners();
    }
    
    void sync_engine::on_fail()
    {
        m_usermgr.m_me.reset();
        m_roommgr.m_room.reset();
        sync_error unknown_error = (sync_error){.type = proto_constants::ERROR_UNKNOWN,.description = "Unknown"};
        for(auto it = m_callback_mapping.begin();it!=m_callback_mapping.end();++it)
        {
            (it->second)(false,&unknown_error);
        }
        m_callback_mapping.clear();
        con_event_type type = m_connected?con_event_connect_lost:con_event_handshake_failed;
        con_event event = (con_event){.type = type,.payload = NULL};
        if(m_listener)m_listener->on_con_event(event);
        m_connected = false;
    }
    
    void sync_engine::on_open()
    {
        con_event event = (con_event){.type = con_event_connected,.payload = NULL};
        if(m_listener) m_listener->on_con_event(event);
    }
    
    void sync_engine::on_connected()
    {
        m_last_publish_time = 0;
        m_connected = true;
        
        _session_id = m_client_handler_ptr->get_sessionid();
        con_event event = (con_event){.type = con_event_connected,.payload = NULL};
        if(m_listener) m_listener->on_con_event(event);
    }
    
    void sync_engine::on_close(handler::close_reason const& reason)
    {
        m_connected = false;
        con_event event = (con_event){.type = con_event_disconnected,.payload = NULL};
        if(m_listener) m_listener->on_con_event(event);
    }

    //io listener callbacks
    void sync_engine::on_socketio_event(const std::string& name,message::ptr const& message,bool needAck, message::ptr& ack_response)
    {
        
        void* callbackData = NULL;
        std::function<void (void*)> *deleter = NULL;
        if(name == proto_constants::EVENT_SYNC_LOCATION)
        {
            if(m_listener)
            {
                std::map<id_type,location> locs;
                to_locations(locs,message);
                sync_event ev = {sync_event_loc_update,&locs};
                if( m_roommgr.m_room)
                {
                    for(auto it =  m_roommgr.m_room->participants().begin();it!= m_roommgr.m_room->participants().end();++it)
                    {
                        auto locit = locs.find(it->id());
                        if(locit!= locs.end())
                        {
                            it->set_location(locit->second);
                        }
                    }
                }
                m_listener->on_sync_event(ev);
            }
            return;
        }
        else if(name == proto_constants::EVENT_ROOM_PARTICIPANTS_CHANGE)
        {
            room r = to_room(message);
            m_roommgr.m_room.reset(new room(r));
            
            if(m_listener)
            {
                sync_event ev = {sync_event_participants_change,NULL};
                m_listener->on_sync_event(ev);
            }
            
        }
        else if(name  == proto_constants::EVENT_ROOM_CREATE ||name  == proto_constants::EVENT_ROOM_JOIN)
        {
            room r = to_room(message);
            m_roommgr.m_room.reset(new room(r));
        }
        else if(name  == proto_constants::EVENT_ROOM_LEAVE)
        {
            m_roommgr.m_room.reset();
        }
        else if(name == proto_constants::EVENT_USER_TRIAL ||name == proto_constants::EVENT_USER_SIGNIN || name == proto_constants::EVENT_USER_SIGNUP)
        {
            m_usermgr.m_me.reset(new user(to_user(message)));
        }
        else if(name == proto_constants::EVENT_ROOM_FIND_BY_NAME)
        {
            room r = to_room(message);
            callbackData = new room(r);
            deleter = new std::function<void (void*)> ([](void* ptr)
                                                       {
                                                           delete (room*)ptr;
                                                       });
        }
        else if(name  == proto_constants::EVENT_RECOVER)
        {
            auto it = message->get_map().find("user");
            auto npos = message->get_map().end();
            if(it!=npos)
            {
                m_usermgr.m_me.reset(new user(to_user(it->second)));
                it = message->get_map().find("room");
                if(it!=npos)
                {
                    m_roommgr.m_room.reset(new room(to_room(it->second)));

                }
            }
            if(m_listener)
            {
                sync_event ev = {sync_event_recovered,NULL};
                m_listener->on_sync_event(ev);
            }
        }
        else if(name == proto_constants::EVENT_ERROR)
        {
            std::string event = message->get_map()["event"]->get_string();
            sync_error errorObj;
            errorObj.description = message->get_map()["desc"]->get_string();
            errorObj.type =  (proto_constants::sync_error_type)message->get_map()["code"]->get_int();
            auto it = m_callback_mapping.find(event);
            if (it!=m_callback_mapping.end()) {
                (it->second)(false,&errorObj);
                m_callback_mapping.erase(it);
            }
            return;
        }
        
        auto it = m_callback_mapping.find(name);
        if(it!=m_callback_mapping.end())
        {
            (it->second)(true, callbackData);
            m_callback_mapping.erase(it);
        }
        
        if(callbackData)
        {
            (*deleter)(callbackData);
            delete deleter;
        }
    }
    
    void sync_engine::connect(std::string const& uri)
    {
        m_client_handler_ptr->connect(uri);
    }
    
    void sync_engine::reconnect(std::string const& uri)
    {
        m_client_handler_ptr->reconnect(uri);
    }
    
    void sync_engine::disconnect()
    {
        m_client_handler_ptr->close();
    }
    
    class sync_engine::room_manager* sync_engine::room_manager()
    {
        return &m_roommgr;
    }
    
    class sync_engine::user_manager* sync_engine::user_manager()
    {
        return &m_usermgr;
    }
    
    void sync_engine::publish_location(const location& loc)
    {
        time_t t = time(NULL);
        if(!m_client_handler_ptr->connected() || m_last_publish_time + m_interval > t)
        {
            return;
        }
        //TODO:call socket.io interfaces.
        message::ptr msg;
        convert_location(msg, loc);
        m_client_handler_ptr->emit(proto_constants::EVENT_PUBLISH_LOCATION,msg);
        m_last_publish_time = t;
    }
    
    void sync_engine::set_min_publish_interval(time_t interval)
    {
        m_interval = interval;
    }
    
    void sync_engine::set_listener(listener* listener)
    {
        m_listener = listener;
    }
}
