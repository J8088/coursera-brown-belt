#include "bus.h"
#include <set>

using namespace std;

Bus::Bus(const std::string& bus_id, const vector<StopPtr>& bus_stops)
	: id(bus_id)
	, stops(bus_stops)
{
}

void Bus::UpdateStats() {
	stats.unique_stops_count = ComputeUniqueStopsCount();
	stats.route_length = ComputeRouteLength();
}

Bus::Stats Bus::GetStats() const {
	return stats;
}

size_t Bus::GetStopsCount() const {
	return stops.size();
}

size_t Bus::ComputeUniqueStopsCount() const {
	auto unique_stops = set<StopPtr>(stops.begin(), stops.end());
	return unique_stops.size();
}

double Bus::ComputeRouteLength() const {
	double new_route_length = 0;
	const int n = stops.size();
	for (int i = 1; i < n; ++i) {
		new_route_length += ComputeDistanceBetweenStops(*stops[i - 1], *stops[i]);
	}
	return new_route_length;
}
