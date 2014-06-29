#include "user.h"

namespace mmp
{
	user_signup_def::user_signup_def()
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

	user::user(id_type id,string const& name)
	{}
	user::~user(void)
	{}

	const location & user::location() const
	{}
	void user::set_location(mmp::location const& loc)
	{}
	const string& user::name() const
	{}
	id_type user::id() const
	{}
	gender user::gender() const
	{}
}