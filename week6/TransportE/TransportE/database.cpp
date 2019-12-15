#include "database.h"
#include "profile.h"

using namespace std;

void Database::AddStop(const StopParams& params) {
	if (!stops.count(params.name)) {
		auto stop = make_shared<Stop>(params.name);
		stop->SetCoords(params.lat, params.lon);
		SetDistancesForStop(stop, params.distances);
		stop->SetIndex(stop_last_index);
		stop_last_index++;
		stops[params.name] = move(stop);
		stops_by_index.push_back(params.name);
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
		auto bus = make_shared<Bus>(params.id, bus_stops, false);
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
		auto bus = make_shared<Bus>(params.id, bus_stops, true);
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

std::string Database::GetStopNameByIndex(size_t index) const {
	return stops_by_index.at(index);
}

RouterActivityPtr Database::GetWaitActivityByEdge(size_t edge_id) const {
	return wait_activities_by_edge.at(edge_id);
}

RouterActivityPtr Database::GetBusActivityByEdge(size_t edge_id) const {
	return bus_activities_by_edge.at(edge_id);
}


void Database::SetRouterSettings(const RouterSettings& params) {
	router_settings = params;
}

const Database::RouterSettings& Database::GetRouterSettings() const {
	return router_settings;
}

TransportGraphPtr Database::GetGraph() const {
	return graph;
}

TransportRouterPtr Database::GetRouter() const {
	return router;
}

void Database::UpdateAllBusesStats() {
	for (auto& [id, bus] : buses) {
		bus->UpdateStats();
	}
}

void Database::UpdateGraphAndRouter() {
	const size_t vertex_count = stops.size();
	graph = make_shared<TransportGraph>(vertex_count);
	const double wait_time = router_settings.bus_wait_time;
	for (auto& [bus_id, bus] : buses) {
		const auto& stops = bus->GetStops();
		const int n = stops.size();
		for (int i = 0; i < n - 1; ++i) {
			double distance = 0;
			for (int j = i + 1; j < n; ++j) {
				distance += ComputeRealDistanceBetweenStops(stops[j - 1], stops[j]);
				const double bus_time = distance / router_settings.bus_velocity;
				const size_t span_count = j - i;

				const auto edge_id = graph->AddEdge({ stops[i]->GetIndex(), stops[j]->GetIndex(), wait_time + bus_time });
				wait_activities_by_edge[edge_id] = make_shared<WaitActivity>(stops[i]->GetName(), wait_time);
				bus_activities_by_edge[edge_id] = make_shared<BusActivity>(bus_id, bus_time, span_count);
			}
		}
	}
	router = make_shared<TransportRouter>(*graph);
}
