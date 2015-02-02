#pragma once

#include "room.h"

#include "user.h"
#include "geo.hpp"
#include <vector>
#include "socket_io_client.h"
#include "proto_constants.h"
#include <boost/shared_ptr.hpp>
#include <functional>
#include <map>


namespace mmp
{
	enum sync_event_type
	{
		sync_event_loc_update = 1,
        sync_event_participants_change = 2,
        sync_event_recovered = 3
	};

    enum con_event_type
    {
        con_event_connected = 1,
        con_event_handshake_failed = 2,
        con_event_connect_lost = 3,
        con_event_disconnected = 4
    };
    
    struct sync_error
    {
        proto_constants::sync_error_type type;
        std::string description;
    };
    
	struct sync_event
	{
		sync_event_type type;
		const void* payload;
	};

    struct con_event
    {
        con_event_type type;
        const void* payload;
    };

    using namespace socketio;
	class sync_engine:public socketio_client_handler::connection_listener,public socketio_client_handler::socketio_listener
	{
		typedef const void* result_ptr;
		typedef std::function<void (bool,result_ptr)> callback_func;

	public:
        class listener
        {
        public:
            virtual void on_sync_event(const sync_event& event) = 0;
            virtual void on_con_event(const con_event& evnet) = 0;
            
            virtual ~listener(){};
        };
        
		class user_manager
		{
			sync_engine* m_engine;
			user_manager(user_manager const&) {};
			void operator=(user_manager const&) {};
		protected:
			user_manager(sync_engine* engine);
			~user_manager();
            unique_ptr<user> m_me;
		public:
			void signup(const user_signup_def& signup_def,callback_func callback);

			void signin(const user_signin_def& signin_def,callback_func callback);

			void trial(const user_trial_def& trial_def,callback_func callback);

			const user* me();

			friend class sync_engine;
		};

		class room_manager
		{

			sync_engine* m_engine;
			room_manager(room_manager const&) {};
			void operator=(room_manager const&) {};
		protected:
            unique_ptr<room> m_room;
			room_manager(sync_engine* engine);
			~room_manager();
		public:

			void create(const room_def& room_def,callback_func callback);

            void join(id_type room_id,callback_func callback);

            void leave(id_type room_id,callback_func callback);

			void find_room_by_name(const std::string& name,callback_func callback);

			const room* current_room();
			friend class sync_engine;
		};

		sync_engine(void);
		~sync_engine(void);

		room_manager* room_manager();

		user_manager* user_manager();

		void publish_location(const location& loc);

		void set_min_publish_interval(time_t interval);

		void set_listener(listener* listener);

		void connect(std::string const& uri);
        
        void reconnect(std::string const& uri);

		void disconnect();
	protected:
		socketio::socketio_client_handler_ptr m_client_handler_ptr;
		std::map<std::string, callback_func> m_callback_mapping; 

		//con event callbacks
		void on_fail(connection_hdl con);
		void on_open(connection_hdl con) ;
		void on_close(connection_hdl con);

		//io listener callbacks
		void on_socketio_event(const std::string& msgEndpoint,const std::string& name, const Value& args,std::string* ackResponse);
		void on_socketio_error(const std::string& endppoint,const std::string& reason,const std::string& advice);

        void recover();
	private:
		listener *m_listener;
		class room_manager m_roommgr;
		class user_manager m_usermgr;
		time_t m_interval;
        time_t m_last_publish_time;
        std::string _session_id;
        bool m_connected;
        
		void __fire_event(sync_event const& event);
	};

}