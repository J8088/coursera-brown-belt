#pragma once

#include "stop.h"
#include <vector>

struct Bus {
public:
	struct Stats {
		size_t unique_stops_count = 0;
		double route_length = 0.0;
	};

	Bus(const std::string& bus_id, const std::vector<StopPtr>& bus_stops);
	void UpdateStats();
	Stats GetStats() const;
	size_t GetStopsCount() const;
private:
	std::string id;
	std::vector<StopPtr> stops;
	Stats stats;

	size_t ComputeUniqueStopsCount() const;
	double ComputeRouteLength() const;
};

using BusPtr = std::shared_ptr<Bus>;
