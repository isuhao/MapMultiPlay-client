#pragma once
#include "room.h"
#include "user.h"
#include <vector>

namespace mmp
{
	class isync_listener
	{
	public:
		virtual void on_sync_event() = 0;
		virtual ~isync_listener(){};
	};

	class sync_engine
	{
	public:
		sync_engine(void);
		~sync_engine(void);

		class room_manager
		{
		public:
		void create_room(const room_def& room_def);

		void join(const room& room);

		const room* current_room();

		const std::vector<user>& room_users();
		};
	private:
		isync_listener *m_listener;
		room_manager m_roommgr;
	};

}