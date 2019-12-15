#pragma once

#include "bus.h"
#include "router.h"
#include "router_activity.h"
#include <unordered_map>
#include <vector>
#include <memory>

using TransportGraph = Graph::DirectedWeightedGraph<double>;
using TransportGraphPtr = std::shared_ptr<Graph::DirectedWeightedGraph<double>>;
using TransportRouter = Graph::Router<double>;
using TransportRouterPtr = std::shared_ptr<Graph::Router<double>>;

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

	struct RouterSettings {
		int bus_wait_time;
		double bus_velocity;
	};

	void AddStop(const StopParams& params);
	void AddOrUpdateStop(const StopParams& params);
	void SetDistancesForStop(StopPtr stop, const StopsDistances& params);
	void AddBusWithRoute(const BusParams& params);
	void AddBusWithRingRoute(const BusParams& params);
	BusPtr GetBus(const std::string& id) const;
	StopPtr GetStop(const std::string& id) const;
	std::string GetStopNameByIndex(size_t index) const;

	void SetRouterSettings(const RouterSettings& params);
	const RouterSettings& GetRouterSettings() const;

	TransportGraphPtr GetGraph() const;
	TransportRouterPtr GetRouter() const;
	RouterActivityPtr GetWaitActivityByEdge(size_t edge_id) const;
	RouterActivityPtr GetBusActivityByEdge(size_t edge_id) const;

	void UpdateAllBusesStats();
	void UpdateGraphAndRouter();
private:
	std::unordered_map<std::string, StopPtr> stops;
	std::unordered_map<std::string, BusPtr> buses;
	RouterSettings router_settings;

	size_t stop_last_index = 0;
	std::vector<std::string> stops_by_index;
	std::unordered_map<size_t, RouterActivityPtr> wait_activities_by_edge;
	std::unordered_map<size_t, RouterActivityPtr> bus_activities_by_edge;

	TransportGraphPtr graph;
	TransportRouterPtr router;
};
