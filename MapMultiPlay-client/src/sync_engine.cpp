#include "sync_engine.h"
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

	sync_engine::sync_engine(std::string host):m_roommgr(this),m_usermgr(this),m_host(host),m_interval(2),m_listener(NULL)
	{

	}

	sync_engine::~sync_engine(void)
	{
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
