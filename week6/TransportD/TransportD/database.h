#pragma once

#include "bus.h"
#include <unordered_map>
#include <vector>

class Database {
public:
	struct StopParams {
		std::string name;
		double lat = 0.0;
		double lon = 0.0;
		StopsDistances distances;
	};

	struct BusParams {
		std::string id;
		std::vector<std::string> stops_names;
	};

	void AddStop(const StopParams& params);
	void AddOrUpdateStop(const StopParams& params);
	void SetDistancesForStop(StopPtr stop, const StopsDistances& params);
	void AddBusWithRoute(const BusParams& params);
	void AddBusWithRingRoute(const BusParams& params);
	BusPtr GetBus(const std::string& id) const;
	StopPtr GetStop(const std::string& id) const;
	void UpdateAllBusesStats();
private:
	std::unordered_map<std::string, StopPtr> stops;
	std::unordered_map<std::string, BusPtr> buses;
};
