#define _USE_MATH_DEFINES

#include "bus.h"
#include <stdexcept>
#include <math.h>

using namespace std;

double DegreesToRadians(double degree) {
	return degree * M_PI / 180.0;
}

Stop::Stop(const string& stop_name)
	: name(stop_name)
{
}

string Stop::GetName() const {
	return name;
}

Stop& Stop::SetIndex(size_t ind) {
	index = ind;
	return *this;
}

size_t Stop::GetIndex() const {
	return index;
}

Stop& Stop::SetCoords(double lat_in_degrees, double lon_in_degrees) {
	coords.lat = lat_in_degrees;
	coords.lon = lon_in_degrees;
	return *this;
}

Stop::Coords Stop::GetCoordsInRadians() const {
	return { DegreesToRadians(coords.lat), DegreesToRadians(coords.lon) };
}

Stop& Stop::AddBus(BusPtr bus) {
	buses.insert(bus->GetId());
	return *this;
}

const set<string>& Stop::GetBuses() const {
	return buses;
}

Stop& Stop::SetDistances(const StopsDistances& new_distances) {
	distances = new_distances;
	return *this;
}

Stop& Stop::AddDistance(const std::string& other_stop, double distance) {
	distances[other_stop] = distance;
	return *this;
}

StopsDistances Stop::GetDistances() const {
	return distances;
}

optional<double> Stop::GetDistanceTo(StopPtr other_stop) const {
	const string stop = other_stop->GetName();
	if (distances.count(stop)) {
		return distances.at(stop);
	} else {
		return nullopt;
	}
}

double ComputeRealDistanceBetweenStops(StopPtr lhs, StopPtr rhs) {
	if (auto distance_from_lhs_to_rhs = lhs->GetDistanceTo(rhs)) {
		return *distance_from_lhs_to_rhs;
	} else if (auto distance_from_rhs_to_lhs = rhs->GetDistanceTo(lhs)) {
		return *distance_from_rhs_to_lhs;
	} else {
		throw runtime_error("no distance between " + lhs->GetName() + " and " + rhs->GetName());
	}
}

double ComputeGeographicalDistanceBetweenStops(StopPtr lhs, StopPtr rhs) {
	const double EARTH_RADIUS = 6371000.0;
	const auto lc = lhs->GetCoordsInRadians();
	const auto rc = rhs->GetCoordsInRadians();
	return acos(sin(lc.lat) * sin(rc.lat) + cos(lc.lat) * cos(rc.lat) * cos(abs(lc.lon - rc.lon))) * EARTH_RADIUS;
}
