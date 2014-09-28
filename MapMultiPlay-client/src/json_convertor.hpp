#include <rapidjson/document.h>
#include "geo.hpp"
#include "room.h"

namespace mmp
{
    namespace json_convertor
    {
        using namespace rapidjson;
        
        template <typename Allocator>
        static inline void AddStringMember(Value& json,const char* name,const std::string& str,Allocator& allocator)
        {
            Value strVal(str.data(),str.length(),allocator);
            json.AddMember(name,strVal,allocator);
        }
        
        template <typename Allocator>
        static inline void convert_location(Value &json, const location& loc,Allocator& allocator)
        {
            json.AddMember("y",loc.latitude,allocator);
            json.AddMember("x",loc.longitude,allocator);
            json.AddMember("h",loc.heading,allocator);
            json.AddMember("s",loc.speed,allocator);
        }
        
        template <typename Allocator>
        static inline void convert_room_def(Value &json, const room_def& def,Allocator& allocator)
        {
            json.AddMember("max",def.m_max_count,allocator);
            AddStringMember(json,"name",def.m_name,allocator);
        }
       
        template <typename Allocator>
        static inline void convert_id(Value &json, id_type any_id,Allocator& allocator)
        {
            json.AddMember("id",any_id,allocator);
        }
        
        template <typename Allocator>
        static inline void convert_room_info(Value &json, const room_info& info,Allocator& allocator)
        {
            json.AddMember("id",info.m_id,allocator);
            json.AddMember("max",info.m_max_count,allocator);
            AddStringMember(json,"name",info.m_name,allocator);
            AddStringMember(json,"owner",info.m_owner_name,allocator);
            json.AddMember("joined",info.m_joined_count,allocator);
        }
        
        template <typename Allocator>
        static inline void convert_user_signup_def(Value &json, const user_signup_def& def,Allocator& allocator)
        {
            AddStringMember(json,"name",def.m_name,allocator);
            AddStringMember(json,"psw",def.m_password,allocator);
            json.AddMember("gender",(unsigned int)def.m_gender,allocator);
        }
        
        template <typename Allocator>
        static inline void convert_user_signin_def(Value &json, const user_signin_def& def,Allocator& allocator)
        {
            AddStringMember(json,"name",def.m_name,allocator);
            AddStringMember(json,"psw",def.m_password,allocator);
        }
        
        template <typename Allocator>
        static inline void convert_user_trial_def(Value &json, const user_trial_def& def,Allocator& allocator)
        {
            AddStringMember(json,"name",def.m_name,allocator);
        }
        
        static inline user to_user(Value const&json)
        {
            gender gen = json.HasMember("gender")? (gender)json["gender"].GetInt() : gender_unknown;
            user u(json["id"].GetInt(),json["name"].GetString(),gen);
            return u;
        }
        
        static inline room_info to_room_info(Value const&json)
        {
            room_info ri;
            ri.m_id = json["id"].GetInt();
            ri.m_max_count = json["max"].GetInt();
            ri.m_name = json["name"].GetString();
            ri.m_owner_name = json["owner"].GetString();
            ri.m_joined_count = json["joined"].GetInt();
            return ri;
        }
        
        static inline room to_room(Value const&json)
        {
            room r(json["id"].GetInt(),json["max"].GetInt(),json["name"].GetString());
            const Value *varParts = &(json["parts"]);
            std::vector<user> parts;
            for(Value::ConstValueIterator it = varParts->Begin();it!=varParts->End() ;++it)
            {
                parts.push_back(to_user(*it));
            }
            r.set_participants_and_owner(parts,json["owner"].GetInt());
            return r;
        }
        
        static inline location to_location(Value const& json)
        {
            location l;
            l.latitude = json["y"].GetDouble();
            l.longitude = json["x"].GetDouble();
            l.heading = static_cast<float>(json["h"].GetDouble());
            l.speed = static_cast<float>(json["s"].GetDouble());
            return l;
        }
        
        static inline void to_locations(std::map<id_type, location>& locs, Value const& json)
        {
            for(Value::ConstMemberIterator it = json.MemberBegin();it!=json.MemberEnd() ;++it)
            {
                id_type userid = it->name.GetInt();
                locs[userid] = to_location(it->value);
            }
        }
    }
}