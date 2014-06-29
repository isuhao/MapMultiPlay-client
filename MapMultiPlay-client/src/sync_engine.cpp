#include "sync_engine.h"
#include <thread>
#include <chrono>
#include "json_convertor.hpp"
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

	void sync_engine::user_manager::signup(const user_signup_def& signup_def)
	{
		//TODO:call socket.io interfaces.
	}
	void sync_engine::user_manager::signin(const user_signin_def& signin_def)
	{
		//TODO:call socket.io interfaces.
	}
	void sync_engine::user_manager::trial(const user_trial_def& trial_def)
	{
		//TODO:call socket.io interfaces.
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

	void sync_engine::room_manager::create_room(const room_def& room_def)
	{
		//TODO:call socket.io interfaces.
	}

	void sync_engine::room_manager::join(const room& room)
	{
		//TODO:call socket.io interfaces.
	}

	void sync_engine::room_manager::leave(const room& room)
	{
		//TODO:call socket.io interfaces.
	}

	void sync_engine::room_manager::find_room_by_name(const std::string& name, std::vector<room>& rooms)
	{
		//TODO:call socket.io interfaces.
	}

	const room* sync_engine::room_manager::current_room()
	{
		return m_room;
	}

	sync_engine::sync_engine():m_roommgr(this),m_usermgr(this),m_interval(2),m_listener(NULL),m_global_msg_id(1)
	{

	}

	sync_engine::~sync_engine(void)
	{
	}

	void sync_engine::connect(std::string uri)
	{
		socketio_client_handler_ptr handler(new socketio_client_handler());
		client::connection_ptr con;
		boost::shared_ptr<client> endpoint_ptr(new client(handler));

		// Set log level. Leave these unset for no logging, or only set a few for selective logging.
		endpoint_ptr->elog().set_level(websocketpp::log::elevel::RERROR);
		endpoint_ptr->elog().set_level(websocketpp::log::elevel::FATAL);
		endpoint_ptr->elog().set_level(websocketpp::log::elevel::WARN);
		endpoint_ptr->alog().set_level(websocketpp::log::alevel::DEVEL);

		std::string socket_io_uri = handler->perform_handshake(uri);
		con = endpoint_ptr->get_connection(socket_io_uri);

		// The previous two lines can be combined:
		// con = endpoint.get_connection(handler->perform_handshake(uri));

		endpoint_ptr->connect(con);
		
		std::thread t(boost::bind(&client::run, endpoint_ptr, false));
		while(!handler->connected())
		{
			//FIXME: dead loop if connect failed.
			std::this_thread::sleep_for(seconds_type(1));
		}
		m_client_handler_ptr = handler;
		m_client_ptr = endpoint_ptr;
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
		m_client_handler_ptr->emit("loc",doc,"$room",m_global_msg_id++);
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
