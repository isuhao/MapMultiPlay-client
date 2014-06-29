#include <rapidjson/document.h>
#include "geo.hpp"

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
	}
}