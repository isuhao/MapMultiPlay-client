#include "sync_engine.h"
#include <thread>
#include <chrono>
#include "json_convertor.hpp"
#include "proto_constants.h"
using namespace socketio;

typedef std::chrono::duration<int> seconds_type;

namespace mmp
{
	sync_engine::user_manager::user_manager(sync_engine* engine):m_engine(engine),m_me(NULL)
	{
	}
	sync_engine::user_manager::~user_manager()
	{
	}

	void sync_engine::user_manager::signup(const user_signup_def& signup_def,callback_func& callback)
	{
		//TODO:call socket.io interfaces.
		Document doc;
		json_convertor::convert_user_signup_def(doc, signup_def);
		m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_SIGNUP,doc,proto_constants::ENDPOINT_SERVER);
		m_engine->m_callback_mapping[proto_constants::EVENT_USER_SIGNUP] = callback;
	}
	void sync_engine::user_manager::signin(const user_signin_def& signin_def,callback_func& callback)
	{
		//TODO:call socket.io interfaces.
		Document doc;
		json_convertor::convert_user_signin_def(doc, signin_def);
		m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_SIGNIN,doc,proto_constants::ENDPOINT_SERVER);
		m_engine->m_callback_mapping[proto_constants::EVENT_USER_SIGNIN] = callback;
	}
	void sync_engine::user_manager::trial(const user_trial_def& trial_def,callback_func& callback)
	{
		//TODO:call socket.io interfaces.
		Document doc;
		json_convertor::convert_user_trial_def(doc, trial_def);
		m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_USER_TRIAL,doc,proto_constants::ENDPOINT_SERVER);
		m_engine->m_callback_mapping[proto_constants::EVENT_USER_TRIAL] = callback;
	}
	const user* sync_engine::user_manager::me()
	{
		return m_me;
	}

	sync_engine::room_manager::room_manager(sync_engine* engine):m_engine(engine),m_room(NULL)
	{
	}
	sync_engine::room_manager::~room_manager()
	{
	}

	void sync_engine::room_manager::create_room(const room_def& room_def,callback_func& callback)
	{
		//TODO:call socket.io interfaces.
		Document doc;
		json_convertor::convert_room_def(doc, room_def);
		m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_CREATE,doc,proto_constants::ENDPOINT_SERVER);
		m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_CREATE] = callback;
	}

	void sync_engine::room_manager::join(const room& room,callback_func& callback)
	{
		//TODO:call socket.io interfaces.
		Document doc;
		json_convertor::convert_room(doc,room);
		m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_JOIN,doc,proto_constants::ENDPOINT_SERVER);
		m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_JOIN] = callback;
	}

	void sync_engine::room_manager::leave(const room& room,callback_func& callback)
	{
		//TODO:call socket.io interfaces.
		Document doc;
		json_convertor::convert_room(doc,room);
		m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_LEAVE,doc,proto_constants::ENDPOINT_SERVER);
		m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_LEAVE] = callback;
	}

	void sync_engine::room_manager::find_room_by_name(const std::string& name, callback_func& callback)
	{
		//TODO:call socket.io interfaces.
		m_engine->m_client_handler_ptr->emit(proto_constants::EVENT_ROOM_FIND_BY_NAME,name,proto_constants::ENDPOINT_SERVER);
		m_engine->m_callback_mapping[proto_constants::EVENT_ROOM_FIND_BY_NAME] = callback;
	}

	const room* sync_engine::room_manager::current_room()
	{
		return m_room;
	}

	sync_engine::sync_engine():m_roommgr(this),m_usermgr(this),m_interval(2),m_listener(NULL),m_client_handler_ptr(new socketio::socketio_client_handler())
	{

	}

	sync_engine::~sync_engine(void)
	{
	}

	void sync_engine::on_fail(connection_hdl con)
	{
		for(auto it = m_callback_mapping.begin();it!=m_callback_mapping.end();++it)
		{
			(it->second)(false,NULL);
		}
	}

	void sync_engine::on_open(connection_hdl con) 
	{
	}

	void sync_engine::on_close(connection_hdl con)
	{
	}

	//io listener callbacks
	void sync_engine::on_socketio_event(const std::string& msgEndpoint,const std::string& name, const Value& args,std::string* ackResponse)
	{
		auto it = m_callback_mapping.find(name);
		if(it!=m_callback_mapping.end())
		{
			(it->second)(true, &args);
		}
		else if(m_listener)
		{
			if(name == proto_constants::EVENT_PUBLISH_LOCATION)
			{
				std::map<id_type,location> locs;
				json_convertor::to_locations(locs,args);
				sync_event ev = {sync_event_loc_update,&locs};
				m_listener->on_sync_event(ev);
			}
		}
	}
	void sync_engine::on_socketio_error(const std::string& endppoint,const std::string& reason,const std::string& advice)
	{
	
	}

	void sync_engine::connect(std::string uri)
	{
		m_client_handler_ptr->connect(uri);
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
		//TODO:call socket.io interfaces.
		rapidjson::Document doc;
		json_convertor::convert_location(doc,loc);
		m_client_handler_ptr->emit(proto_constants::EVENT_PUBLISH_LOCATION,doc,proto_constants::ENDPOINT_SERVER);
	}

	void sync_engine::set_min_publish_interval(time_t interval)
	{
		m_interval = interval;
	}

	void sync_engine::set_listener(isync_listener* listener)
	{
		m_listener = listener;
	}
}
