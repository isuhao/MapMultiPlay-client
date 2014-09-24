#include "room.h"

namespace mmp{

	room_def::room_def():m_max_count(2)//default for 2.
	{
	}
	room_def::~room_def()
	{
	}
    
	room::room(id_type id, unsigned int max_count,string const& name)
		:m_id(id),m_max_count(max_count),m_name(name),m_owner_index(0)
	{}

	room::~room(void)
	{}
    
    vector<user> & room::participants()
    {
        return m_participants;
    }
    
	const vector<user> & room::participants() const
	{
		return m_participants;
	}

	id_type room::id() const
	{
		return m_id;
	}

	const string& room::name() const
	{
		return m_name;
	}

	unsigned int room::max_count() const
	{
		return m_max_count;
	}

	const user* room::owner() const
	{
		if(m_owner_index<m_participants.size())
		{
			return &(m_participants[m_owner_index]);
		}
		return NULL;
	}

	void room::set_participants_and_owner(vector<user> const& participants,unsigned int owner_idx)
	{
		m_participants = participants;
		m_owner_index = owner_idx;
	}
}