#pragma once
namespace mmp
{
	struct location
	{
		double latitude;
		double longitude;
		float heading;
		float speed;

		location():latitude(0),longitude(0),heading(0),speed(0)
		{}
	};
}