#include "database.h"

using namespace std;

void Database::AddStop(const std::string& name, double lat, double lon) {
	if (!stops.count(name)) {
		stops[name] = make_shared<Stop>(name, lat, lon);
	}
}

void Database::AddOrUpdateStop(const std::string& name, double lat, double lon) {
	if (!stops.count(name)) {
		AddStop(name, lat, lon);
	} else {
		stops[name]->SetCoords(lat, lon);
	}
}

void Database::AddBusWithRoute(const std::string& id, const vector<string>& stops_names) {
	if (!buses.count(id)) {
		vector<StopPtr> bus_stops;
		for (const string& name : stops_names) {
			AddStop(name);
			bus_stops.push_back(stops[name]);
		}
		for (int i = bus_stops.size() - 2; i >= 0; --i) {
			StopPtr stop = bus_stops[i];
			bus_stops.push_back(stop);
		}
		auto bus = make_shared<Bus>(id, bus_stops);
		Bus::AddBusToStopsBuses(bus);
		buses[id] = bus;
	}
}

void Database::AddBusWithRingRoute(const std::string& id, const vector<string>& stops_names) {
	if (!buses.count(id)) {
		vector<StopPtr> bus_stops;
		for (const string& name : stops_names) {
			AddStop(name);
			bus_stops.push_back(stops[name]);
		}
		auto bus = make_shared<Bus>(id, bus_stops);
		Bus::AddBusToStopsBuses(bus);
		buses[id] = bus;
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
