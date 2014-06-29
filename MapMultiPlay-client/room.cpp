#include "room.h"

namespace mmp{

	room_def::room_def()
	{
	}
	room_def::~room_def()
	{
	}

	room::room(id_type id, unsigned int max_count,string const& name)
	{}
	room::~room(void)
	{}

	const vector<user> & room::participants() const
	{}

	id_type room::id() const
	{}

	const string& room::name() const
	{}

	unsigned int room::max_count() const
	{}

	const user& room::owner() const
	{}

	void room::set_participants_and_owner(vector<user> const& users,unsigned int owner_idx)
	{}
}