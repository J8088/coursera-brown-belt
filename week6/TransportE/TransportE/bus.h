#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>
#include <unordered_map>
#include <optional>

struct Bus;
struct Stop;
using BusPtr = std::shared_ptr<Bus>;
using StopPtr = std::shared_ptr<Stop>;

using StopsDistances = std::unordered_map<std::string, double>;

struct Bus {
public:
	struct Stats {
		size_t unique_stops_count = 0;
		double route_length = 0.0;
		double geographical_route_length = 0.0;
		double curvature = 0.0;
	};

	Bus(const std::string& bus_id, const std::vector<StopPtr>& bus_stops, bool is_roundtrip_bus);

	void UpdateStats();
	static void AddBusToStopsBuses(BusPtr bus);

	const std::string& GetId() const;
	const std::vector<StopPtr>& GetStops() const;
	bool IsRoundtrip() const;
	Stats GetStats() const;
	size_t GetStopsCount() const;
private:
	std::string id;
	std::vector<StopPtr> stops;
	bool is_roundtrip;
	Stats stats;

	size_t ComputeUniqueStopsCount() const;
	double ComputeRouteLength() const;
	double ComputeGeographicalRouteLength() const;
	double ComputeCurvature(double real_length, double geographical_length) const;
};

struct Stop {
	struct Coords {
		double lat = 0.0;
		double lon = 0.0;
	};

	Stop(const std::string& stop_name);

	std::string GetName() const;

	Stop& SetIndex(size_t ind);
	size_t GetIndex() const;

	Stop& SetCoords(double lat_in_degrees, double lon_in_degrees);
	Coords GetCoordsInRadians() const;

	Stop& AddBus(BusPtr bus);
	const std::set<std::string>& GetBuses() const;

	Stop& SetDistances(const StopsDistances& new_distances);
	Stop& AddDistance(const std::string& other_stop, double distance);
	StopsDistances GetDistances() const;
	std::optional<double> GetDistanceTo(StopPtr other_stop) const;

	friend double ComputeRealDistanceBetweenStops(StopPtr lhs, StopPtr rhs);
	friend double ComputeGeographicalDistanceBetweenStops(StopPtr lhs, StopPtr rhs);
private:
	size_t index;
	std::string name;
	Coords coords;
	std::set<std::string> buses;
	StopsDistances distances;
};

double DegreesToRadians(double degree);
double ComputeRealDistanceBetweenStops(StopPtr lhs, StopPtr rhs);
double ComputeGeographicalDistanceBetweenStops(StopPtr lhs, StopPtr rhs);
