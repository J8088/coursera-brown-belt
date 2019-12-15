#pragma once

#include "bus.h"
#include "stop.h"
#include <unordered_map>
#include <vector>

class Database {
public:
	void AddStop(const std::string& name, double lat = 0.0, double lon = 0.0);
	void AddOrUpdateStop(const std::string& name, double lat = 0.0, double lon = 0.0);
	void AddBusWithRoute(const std::string& id, const std::vector<std::string>& stops_names);
	void AddBusWithRingRoute(const std::string& id, const std::vector<std::string>& stops_names);
	BusPtr GetBus(const std::string& id) const;
	void UpdateAllBusesStats();
private:
	std::unordered_map<std::string, StopPtr> stops;
	std::unordered_map<std::string, BusPtr> buses;
};
