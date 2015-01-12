#include "sync_engine.h"
#include <thread>
#include <chrono>
#include "json_convertor.hpp"
using namespace socketio;

typedef std::chrono::duration<int> seconds_type;

namespace mmp
{
    void wrap_json_document(Document& doc,std::function<void (Value& json,Document& doc)> callback)
    {
        doc.SetObject();
        Value json(kObjectType);
        callback(json,doc);
        doc.AddMember("args",json,doc.GetAllocator());
    }
    
    sync_engine::user_manager::user_manager(sync_engine* engine):m_engine(engine),m_me()
    {
    }
    sync_engine::user_manager::~user_manager()
    {
    }
    
    void sync_engine::user_manager::signup(const user_signup_def& signup_def,callback_func callback)
    {
        //TODO:call socket.io interfaces.
        using namespace json_convertor;
        Document doc;
        wrap_json_document(doc, std::function<void (Value& json,Document& doc)>([&](Value& json,Document& d)
                                                                                {
                                                                                    json_convertor::convert_user_signup_def(json, signup_def,d.GetAllocator());
                                                                                }));
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_SIGNUP,doc,proto_constants::ENDPOINT_SERVER);
        m_engine->m_callback_mapping[proto_constants::EVENT_USER_SIGNUP] = callback;
    }
    void sync_engine::user_manager::signin(const user_signin_def& signin_def,callback_func callback)
    {
        //TODO:call socket.io interfaces.
        Document doc;
        wrap_json_document(doc, std::function<void (Value& json,Document& doc)>([&](Value& json,Document& d)
                                                                                {
                                                                                    json_convertor::convert_user_signin_def(json, signin_def,d.GetAllocator());
                                                                                }));
        
        
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_SIGNIN,doc,proto_constants::ENDPOINT_SERVER);
        m_engine->m_callback_mapping[proto_constants::EVENT_USER_SIGNIN] = callback;
    }
    void sync_engine::user_manager::trial(const user_trial_def& trial_def,callback_func callback)
    {
        //TODO:call socket.io interfaces.
        Document doc;
        
        wrap_json_document(doc, std::function<void (Value& json,Document& doc)>([&](Value& json,Document& d)
                                                                                {
                                                                                    json_convertor::convert_user_trial_def(json, trial_def,d.GetAllocator());
                                                                                }));
        
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_TRIAL,doc,proto_constants::ENDPOINT_SERVER);
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
        Document doc;
        wrap_json_document(doc, std::function<void (Value& json,Document& doc)>([&](Value& json,Document& d)
                                                                                {
                                                                                    json_convertor::convert_room_def(json, room_def,d.GetAllocator());
                                                                                }));
        
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_CREATE,doc,proto_constants::ENDPOINT_SERVER);
        m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_CREATE] = callback;
    }
    
    void sync_engine::room_manager::join(id_type room_id,callback_func callback)
    {
        if(!m_engine->m_usermgr.m_me)
        {
            return;
        }
        Document doc;
        wrap_json_document(doc, std::function<void (Value& json,Document& doc)>([&](Value& json,Document& d)
                                                                                {
                                                                                    json_convertor::convert_id(json, room_id,d.GetAllocator());
                                                                                }));
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_JOIN,doc,proto_constants::ENDPOINT_SERVER);
        m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_JOIN] = callback;
    }
    
    void sync_engine::room_manager::leave(id_type room_id,callback_func callback)
    {
        if(!m_engine->m_roommgr.m_room)
        {
            return;
        }
        Document doc;
        wrap_json_document(doc, std::function<void (Value& json,Document& doc)>([&](Value& json,Document& d)
                                                                                {
                                                                                    json_convertor::convert_id(json, room_id,d.GetAllocator());
                                                                                }));
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_LEAVE,doc,proto_constants::ENDPOINT_SERVER);
        m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_LEAVE] = callback;
    }
    
    void sync_engine::room_manager::find_room_by_name(const std::string& name, callback_func callback)
    {
        //TODO:call socket.io interfaces.
        m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_FIND_BY_NAME,name,proto_constants::ENDPOINT_SERVER);
        m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_FIND_BY_NAME] = callback;
    }
    
    const room* sync_engine::room_manager::current_room()
    {
        return m_room.get();
    }
    
    sync_engine::sync_engine():m_roommgr(this),m_usermgr(this),m_interval(2),m_listener(NULL),m_client_handler_ptr(new socketio::socketio_client_handler()),m_last_publish_time(0),m_connected(false)
    {
        m_client_handler_ptr->set_socketio_listener(this);
        m_client_handler_ptr->set_connection_listener(this);
    }
    
    sync_engine::~sync_engine(void)
    {
        m_client_handler_ptr->set_socketio_listener(NULL);
        m_client_handler_ptr->set_connection_listener(NULL);
    }
    
    void sync_engine::on_fail(connection_hdl con)
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
    
    void sync_engine::on_open(connection_hdl con)
    {
        m_last_publish_time = 0;
        m_connected = true;
        con_event event = (con_event){.type = con_event_connected,.payload = NULL};
        if(m_listener) m_listener->on_con_event(event);
    }
    
    void sync_engine::on_close(connection_hdl con)
    {
        m_connected = false;
        con_event event = (con_event){.type = con_event_disconnected,.payload = NULL};
        if(m_listener) m_listener->on_con_event(event);
    }
    
    //io listener callbacks
    void sync_engine::on_socketio_event(const std::string& msgEndpoint,const std::string& name, const Value& args,std::string* ackResponse)
    {
        
        void* callbackData = NULL;
        std::function<void (void*)> *deleter = NULL;
        if(name == proto_constants::EVENT_SYNC_LOCATION)
        {
            if(m_listener)
            {
                std::map<id_type,location> locs;
                json_convertor::to_locations(locs,args[0U]);
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
            room r = json_convertor::to_room(args[0U]);
            m_roommgr.m_room.reset(new room(r));
            
            if(m_listener)
            {
                sync_event ev = {sync_event_participants_change,NULL};
                m_listener->on_sync_event(ev);
            }
            
        }
        else if(name  == proto_constants::EVENT_ROOM_CREATE ||name  == proto_constants::EVENT_ROOM_JOIN)
        {
            room r = json_convertor::to_room(args[0U]);
            m_roommgr.m_room.reset(new room(r));
        }
        else if(name  == proto_constants::EVENT_ROOM_LEAVE)
        {
            m_roommgr.m_room.reset();
        }
        else if(name == proto_constants::EVENT_USER_TRIAL ||name == proto_constants::EVENT_USER_SIGNIN || name == proto_constants::EVENT_USER_SIGNUP)
        {
            m_usermgr.m_me.reset(new user(json_convertor::to_user(args[0U])));
        }
        else if(name == proto_constants::EVENT_ROOM_FIND_BY_NAME)
        {
            room r = json_convertor::to_room(args[0U]);
            callbackData = new room(r);
            deleter = new std::function<void (void*)> ([](void* ptr)
                                                       {
                                                           delete (room*)ptr;
                                                       });
        }
        else if(name == proto_constants::EVENT_ERROR)
        {
            std::string event = args[0U]["event"].GetString();
            sync_error errorObj;
            errorObj.description = args[0U]["desc"].GetString();
            errorObj.type =  (proto_constants::sync_error_type)args[0U]["code"].GetInt();
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
    
    void sync_engine::on_socketio_error(const std::string& endppoint,const std::string& reason,const std::string& advice)
    {
        
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
        rapidjson::Document doc;
        wrap_json_document(doc, std::function<void (Value& json,Document& doc)>([&](Value& json,Document& d)
                                                                                {
                                                                                    json_convertor::convert_location(json, loc,d.GetAllocator());
                                                                                }));
        
        m_client_handler_ptr->emit(proto_constants::EVENT_PUBLISH_LOCATION,doc,proto_constants::ENDPOINT_SERVER);
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
