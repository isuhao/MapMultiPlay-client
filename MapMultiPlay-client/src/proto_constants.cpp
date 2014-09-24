#include "proto_constants.h"

namespace mmp
{
    const char* const proto_constants::ENDPOINT_SERVER = "$server";
    
    const char* const proto_constants::EVENT_PUBLISH_LOCATION = "loc";
    
    const char* const proto_constants::EVENT_SYNC_LOCATION = "sync";
    
    const char* const proto_constants::EVENT_USER_SIGNIN = "user_signin";
    
    const char* const proto_constants::EVENT_USER_SIGNUP = "user_signup";
    
    const char* const proto_constants::EVENT_USER_TRIAL = "user_trial";
    
    const char* const proto_constants::EVENT_ROOM_CREATE = "room_create";
    
    const char* const proto_constants::EVENT_ROOM_JOIN = "room_join";
    
    const char* const proto_constants::EVENT_ROOM_LEAVE = "room_leave";
    
    const char* const proto_constants::EVENT_ROOM_FIND_BY_NAME = "room_fbn";
    
    const char* const proto_constants::EVENT_ROOM_PARTICIPANTS_CHANGE = "room_parts_change";
}