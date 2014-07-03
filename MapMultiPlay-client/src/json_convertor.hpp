#include <rapidjson/document.h>
#include "geo.hpp"
#include "room.h"

namespace mmp
{
	namespace json_convertor
	{
		using namespace rapidjson;
		static inline void convert_location(Document &json, const location& loc)
		{
			Value jsonLoc(kArrayType);
			jsonLoc.PushBack(loc.latitude,json.GetAllocator());
			jsonLoc.PushBack(loc.longitude,json.GetAllocator());
			jsonLoc.PushBack(loc.heading,json.GetAllocator());
			jsonLoc.PushBack(loc.speed,json.GetAllocator());
			json.AddMember("args",jsonLoc,json.GetAllocator());
		}

		static inline void convert_room_def(Document &json, const room_def& def)
		{
		}

		static inline void convert_user_signup_def(Document &json, const user_signup_def& def)
		{
		}
		static inline void convert_user_signin_def(Document &json, const user_signin_def& def)
		{
		}

		static inline void convert_user_trial_def(Document &json, const user_trial_def& def)
		{
		}
	}
}