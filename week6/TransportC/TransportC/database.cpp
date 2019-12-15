#include "database.h"

using namespace std;

void Database::AddStop(const StopParams& params) {
	if (!stops.count(params.name)) {
		auto stop = make_shared<Stop>(params.name);
		stop->SetCoords(params.lat, params.lon);
		SetDistancesForStop(stop, params.distances);
		stops[params.name] = move(stop);
	}
}

void Database::AddOrUpdateStop(const StopParams& params) {
	if (!stops.count(params.name)) {
		AddStop(params);
	} else {
		auto& stop = stops[params.name];
		stop->SetCoords(params.lat, params.lon);
		SetDistancesForStop(stop, params.distances);
	}
}

void Database::SetDistancesForStop(StopPtr stop, const StopsDistances& distances) {
	for (const auto& [other_stop, distance] : distances) {
		AddStop({ other_stop });
	}
	stop->SetDistances(move(distances));
}

void Database::AddBusWithRoute(const BusParams& params) {
	if (!buses.count(params.id)) {
		vector<StopPtr> bus_stops;
		for (const string& name : params.stops_names) {
			AddStop({ name });
			bus_stops.push_back(stops[name]);
		}
		for (int i = bus_stops.size() - 2; i >= 0; --i) {
			StopPtr stop = bus_stops[i];
			bus_stops.push_back(stop);
		}
		auto bus = make_shared<Bus>(params.id, bus_stops);
		Bus::AddBusToStopsBuses(bus);
		buses[params.id] = bus;
	}
}

void Database::AddBusWithRingRoute(const BusParams& params) {
	if (!buses.count(params.id)) {
		vector<StopPtr> bus_stops;
		for (const string& name : params.stops_names) {
			AddStop({ name });
			bus_stops.push_back(stops[name]);
		}
		auto bus = make_shared<Bus>(params.id, bus_stops);
		Bus::AddBusToStopsBuses(bus);
		buses[params.id] = bus;
	}
}

BusPtr Database::GetBus(const string& id) const {
	if (buses.count(id)) {
		return buses.at(id);
	} else {
		return nullptr;
	}
}

StopPtr Database::GetStop(const string& id) const {
	if (stops.count(id)) {
		return stops.at(id);
	} else {
		return nullptr;
	}
}

void Database::UpdateAllBusesStats() {
	for (auto& [id, bus] : buses) {
		bus->UpdateStats();
	}
}
