#define _USE_MATH_DEFINES

#include "stop.h"
#include <math.h>

using namespace std;

double DegreesToRadians(double degree) {
	return degree * M_PI / 180.0;
}

Stop::Stop(const string& stop_name, double lat_in_degrees, double lon_in_degrees)
	: name(stop_name)
	, coords({ lat_in_degrees, lon_in_degrees })
{
}

void Stop::SetCoords(double lat_in_degrees, double lon_in_degrees) {
	coords.lat = lat_in_degrees;
	coords.lon = lon_in_degrees;
}

Stop::Coords Stop::GetCoordsInRadians() const {
	return { DegreesToRadians(coords.lat), DegreesToRadians(coords.lon) };
}

double ComputeDistanceBetweenStops(const Stop& lhs, const Stop& rhs) {
	const double EARTH_RADIUS = 6371000.0;
	const auto lc = lhs.GetCoordsInRadians();
	const auto rc = rhs.GetCoordsInRadians();
	return acos(sin(lc.lat) * sin(rc.lat) + cos(lc.lat) * cos(rc.lat) * cos(abs(lc.lon - rc.lon))) * EARTH_RADIUS;
}
