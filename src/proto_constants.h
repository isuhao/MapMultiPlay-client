#pragma once
namespace mmp
{
	class proto_constants
	{
	public:
        enum sync_error_type
        {
            ERROR_USER_SIGNIN_FAILED=101,
            ERROR_USER_ILLEGAL=102,
            ERROR_ROOM_EXISTS=110,
            ERROR_ROOM_NOT_EXISTS=111,
            ERROR_ROOM_CREATE_FAILED=112,
            ERROR_ROOM_JOIN_FULL=113,
            ERROR_UNKNOWN=255
        };
        
        static const char* const EVENT_RECOVER;
        
		static const char* const EVENT_PUBLISH_LOCATION;

		static const char* const EVENT_SYNC_LOCATION;

		static const char* const ENDPOINT_SERVER;

		static const char* const EVENT_USER_SIGNIN;

		static const char* const EVENT_USER_SIGNUP;

		static const char* const EVENT_USER_TRIAL;

		static const char* const EVENT_ROOM_CREATE;

		static const char* const EVENT_ROOM_JOIN;

		static const char* const EVENT_ROOM_LEAVE;

		static const char* const EVENT_ROOM_FIND_BY_NAME;
        
        static const char* const EVENT_ROOM_PARTICIPANTS_CHANGE;
        
        static const char* const EVENT_ERROR;
	};
}