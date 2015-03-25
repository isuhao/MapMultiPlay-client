//
//  message_convertor.hpp
//  MMPClient
//
//  Created by Melo Yao on 3/25/15.
//  Copyright (c) 2015 AutoNavi. All rights reserved.
//

#ifndef MMPClient_message_convertor_hpp
#define MMPClient_message_convertor_hpp

#define FIX_POINT5(__f__) ((int) (__f__ * 100000.0))

#define FLOAT_POINT5(__d__) ((double) (__d__/100000.0))

#include "socket_io_message.h"

namespace mmp
{
    namespace message_convertor
    {
        using namespace socketio;
        
        static inline void convert_session(message::ptr &msg, std::string const& session_id)
        {
            msg = object_message::create();
            msg->get_map()["sid"] = string_message::create(session_id);
        }
        
        static inline void convert_location(message::ptr &msg, const location& loc)
        {
            
            msg = object_message::create();
            msg->get_map()["x"] = int_message::create(FIX_POINT5(loc.latitude));
            msg->get_map()["y"] = int_message::create(FIX_POINT5(loc.longitude));
            msg->get_map()["h"] = double_message::create(loc.heading);
            msg->get_map()["s"] = double_message::create(loc.speed);
        }
        
        static inline void convert_room_def(message::ptr &msg, const room_def& def)
        {
            msg = object_message::create();
            msg->get_map()["max"] = int_message::create(def.m_max_count);
            msg->get_map()["name"] = string_message::create(def.m_name);
        }
        
        static inline void convert_id(message::ptr &msg, id_type any_id)
        {
            msg = object_message::create();
            msg->get_map()["id"] = int_message::create(any_id);
        }
        
        static inline void convert_room_info(message::ptr &msg, const room_info& info)
        {
            msg = object_message::create();
            msg->get_map()["id"] = int_message::create(info.m_id);
            msg->get_map()["max"] = int_message::create(info.m_max_count);
            msg->get_map()["name"] = string_message::create(info.m_name);
            
            msg->get_map()["owner"] = string_message::create(info.m_owner_name);
            msg->get_map()["joined"] = int_message::create(info.m_joined_count);
        }
        
        static inline void convert_user_signup_def(message::ptr &msg, const user_signup_def& def)
        {
            msg = object_message::create();
            msg->get_map()["name"] = string_message::create(def.m_name);
            msg->get_map()["psw"] = string_message::create(def.m_password);
            msg->get_map()["gender"] = int_message::create((int)def.m_gender);
        }
        
        static inline void convert_user_signin_def(message::ptr &msg, const user_signin_def& def)
        {
            
            msg = object_message::create();
            msg->get_map()["name"] = string_message::create(def.m_name);
            msg->get_map()["psw"] = string_message::create(def.m_password);
        }
        
        
        static inline void convert_user_trial_def(message::ptr &msg, const user_trial_def& def)
        {
            msg = object_message::create();
            msg->get_map()["name"] = string_message::create(def.m_name);
        }
        
        static inline location to_location(message::ptr const& msg)
        {
            location l;
            l.latitude = FLOAT_POINT5(msg->get_map()["y"]->get_int());
            l.longitude = FLOAT_POINT5(msg->get_map()["x"]->get_int());
            l.heading = static_cast<float>(msg->get_map()["h"]->get_double());
            l.speed = static_cast<float>(msg->get_map()["s"]->get_double());
            return l;
        }
        
        static inline user to_user(message::ptr const& msg)
        {
            auto npos = msg->get_map().end();
            auto it = msg->get_map().find("gender");
            gender gen = gender_unknown;
            if(it!=npos)
            {
                gen = (gender)it->second->get_int();
            }
            
            user u(msg->get_map()["id"]->get_int(),msg->get_map()["name"]->get_string(),gen);
            if((it = msg->get_map().find("loc"))!=npos)
            {
                location loc = to_location(it->second);
                u.set_location(loc);
            }
            return u;
        }
        
        static inline room_info to_room_info(message::ptr const& msg)
        {
            room_info ri;
            ri.m_id = msg->get_map()["id"]->get_int();
            ri.m_max_count = msg->get_map()["max"]->get_int();
            ri.m_name = msg->get_map()["name"]->get_string();
            ri.m_owner_name = msg->get_map()["owner"]->get_string();
            ri.m_joined_count = msg->get_map()["joined"]->get_int();
            return ri;
        }
        
        static inline room to_room(message::ptr const& msg)
        {
            room r(msg->get_map()["id"]->get_int(),msg->get_map()["max"]->get_int(),msg->get_map()["name"]->get_string());
            const message::ptr varParts = msg->get_map()["parts"];
            std::vector<user> parts;
            for(auto it = varParts->get_vector().begin();it!=varParts->get_vector().end() ;++it)
            {
                parts.push_back(to_user(*it));
            }
            r.set_participants_and_owner(parts,msg->get_map()["owner"]->get_int());
            return r;
        }
        
        
        
        static inline void to_locations(std::map<id_type, location>& locs, message::ptr const& msg)
        {
            for(auto it = msg->get_map().begin();it!=msg->get_map().end() ;++it)
            {
                id_type userid = stoi(it->first);
                locs[userid] = to_location(it->second);
            }
        }
    }
}
#endif
