#include "user.h"

namespace mmp
{
	user_signup_def::user_signup_def():m_gender(gender_unknown)
	{
	}

	user_signup_def::~user_signup_def()
	{
	}

	user_signin_def::user_signin_def()
	{
	}

	user_signin_def::~user_signin_def()
	{
	}

	user_trial_def::user_trial_def()
	{
	}
	
	user_trial_def::~user_trial_def()
	{
	}

	user::user(id_type id,string const& name,enum gender gender):m_id(id),m_name(name),m_gender(gender)
	{
	}

	user::~user(void)
	{}

	const location & user::location() const
	{
		return m_loc;
	}

	void user::set_location(mmp::location const& loc)
	{
		m_loc = loc;
	}

	const string& user::name() const
	{
		return m_name;
	}

	id_type user::id() const
	{
		return m_id;
	}

	gender user::gender() const
	{
		return m_gender;
	}
}