#pragma once
#include <string>
#include "common.h"
#include "geo.hpp"

namespace mmp
{
	using namespace std;
	enum gender
	{
		gender_unknown = 0,
		gender_male = 1,
		gender_female = 2
	};

	class user_signup_def
	{

	public:
		string m_name;
		string m_password;
		gender m_gender;
		//TODO: more info.
		user_signup_def();
		~user_signup_def();
	};

	class user_trial_def
	{

	public:
		string m_name;
		user_trial_def();
		~user_trial_def();
	};

	class user_signin_def
	{
	public:
		string m_name;
		string m_password;
		user_signin_def();
		~user_signin_def();
	};

	class user
	{
		id_type m_id;
		gender m_gender;
		string m_name;
		location m_loc;

	public:
		user(id_type id,string const& name,gender gender);
		~user(void);

		const location & location() const;
		void set_location(mmp::location const& loc);
		const string& name() const;
		id_type id() const;
		gender gender() const;
	};
}