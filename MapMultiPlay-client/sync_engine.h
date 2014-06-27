#pragma once
#include "room.h"
#include "user.h"
#include "geo.hpp"
#include <vector>
#include <chrono>

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
	public:
		sync_engine(void);
		~sync_engine(void);

		class room_manager
		{
		protected:
			room_manager(sync_engine* engine);
		public:

			~room_manager();
			void create_room(const room_def& room_def);

			void join(const room& room);

			const room* current_room();

			const std::vector<user>& room_users();
		};

		void publish_location(const location& loc);
		void set_min_publish_interval(time_t interval);

		void set_listener(isync_listener* listener);

	private:
		isync_listener *m_listener;
		room_manager m_roommgr;
		time_t m_interval;

		void __fire_event()
	};

}