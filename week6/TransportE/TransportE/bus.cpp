#include "bus.h"
#include <set>

using namespace std;

Bus::Bus(const std::string& bus_id, const vector<StopPtr>& bus_stops, bool is_roundtrip_bus)
	: id(bus_id)
	, stops(bus_stops)
	, is_roundtrip(is_roundtrip_bus)
{
}

void Bus::UpdateStats() {
	stats.unique_stops_count = ComputeUniqueStopsCount();
	stats.route_length = ComputeRouteLength();
	stats.geographical_route_length = ComputeGeographicalRouteLength();
	stats.curvature = ComputeCurvature(stats.route_length, stats.geographical_route_length);
}

void Bus::AddBusToStopsBuses(BusPtr bus) {
	for (auto& stop : bus->stops) {
		stop->AddBus(bus);
	}
}

const string& Bus::GetId() const {
	return id;
}

const vector<StopPtr>& Bus::GetStops() const {
	return stops;
}

bool Bus::IsRoundtrip() const {
	return is_roundtrip;
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
		new_route_length += ComputeRealDistanceBetweenStops(stops[i - 1], stops[i]);
	}
	return new_route_length;
}

double Bus::ComputeGeographicalRouteLength() const {
	double new_route_length = 0;
	const int n = stops.size();
	for (int i = 1; i < n; ++i) {
		new_route_length += ComputeGeographicalDistanceBetweenStops(stops[i - 1], stops[i]);
	}
	return new_route_length;
}

double Bus::ComputeCurvature(double real_length, double geographical_length) const {
	return real_length / geographical_length;
}
