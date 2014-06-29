#pragma once
#include "room.h"
#include "user.h"
#include "geo.hpp"
#include <vector>
#include "socket_io_client.hpp"
#include <boost/shared_ptr.hpp>
#include <functional>
namespace mmp
{
	enum sync_event_type
	{
		sync_event_loc_update = 1
	};

	struct sync_event
	{
		sync_event_type type;
		const void* payload;
	};

	class isync_listener
	{
	public:
		virtual void on_sync_event(const sync_event& event) = 0;
		virtual ~isync_listener(){};
	};

	class sync_engine
	{
		typedef void* result_ptr;
		typedef std::function<void (bool,result_ptr)> callback_func;

	public:
		class user_manager
		{
			user* m_me;
			sync_engine* m_engine;
			user_manager(user_manager const&) = delete;
			void operator=(user_manager const&) = delete;
		protected:
			user_manager(sync_engine* engine);
			~user_manager();
		public:
			void signup(const user_signup_def& signup_def,callback_func& callback);

			void signin(const user_signin_def& signin_def,callback_func& callback);

			void trial(const user_trial_def& trial_def,callback_func& callback);

			const user* me();

			friend class sync_engine;
		};

		class room_manager
		{
			room *m_room;
			sync_engine* m_engine;
			room_manager(room_manager const&) = delete;
			void operator=(room_manager const&) = delete;
		protected:
			room_manager(sync_engine* engine);
			~room_manager();
		public:

			void create_room(const room_def& room_def,callback_func& callback);

			void join(const room& room,callback_func& callback);

			void leave(const room& room,callback_func& callback);

			void find_room_by_name(const std::string& name,callback_func& callback);

			const room* current_room();
			friend class sync_engine;
		};

		sync_engine(void);
		~sync_engine(void);

		room_manager* room_manager();

		user_manager* user_manager();

		void publish_location(const location& loc);

		void set_min_publish_interval(time_t interval);

		void set_listener(isync_listener* listener);

		void connect(std::string uri);

		void disconnect();

	private:
		isync_listener *m_listener;
		class room_manager m_roommgr;
		class user_manager m_usermgr;
		time_t m_interval;
		unsigned int m_global_msg_id;
		socketio::socketio_client_handler_ptr m_client_handler_ptr;
		boost::shared_ptr<client> m_client_ptr;
		void __fire_event(sync_event const& event);
	};

}