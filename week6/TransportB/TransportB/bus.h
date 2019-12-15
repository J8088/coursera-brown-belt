#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>

struct Bus;
struct Stop;
using BusPtr = std::shared_ptr<Bus>;
using StopPtr = std::shared_ptr<Stop>;

struct Bus {
public:
	struct Stats {
		size_t unique_stops_count = 0;
		double route_length = 0.0;
	};

	Bus(const std::string& bus_id, const std::vector<StopPtr>& bus_stops);

	void UpdateStats();
	static void AddBusToStopsBuses(BusPtr bus);

	const std::string& GetId() const;
	Stats GetStats() const;
	size_t GetStopsCount() const;
private:
	std::string id;
	std::vector<StopPtr> stops;
	Stats stats;

	size_t ComputeUniqueStopsCount() const;
	double ComputeRouteLength() const;
};

struct Stop {
	struct Coords {
		double lat = 0.0;
		double lon = 0.0;
	};

	Stop(const std::string& stop_name, double lat_in_degrees = 0.0, double lon_in_degrees = 0.0);

	void SetCoords(double lat_in_degrees, double lon_in_degrees);
	Coords GetCoordsInRadians() const;

	void AddBus(BusPtr bus);
	const std::set<std::string>& GetBuses() const;

	friend double ComputeDistanceBetweenStops(const Stop& lhs, const Stop& rhs);
private:
	std::string name;
	Coords coords;
	std::set<std::string> buses;
};

double DegreesToRadians(double degree);
double ComputeDistanceBetweenStops(const Stop& lhs, const Stop& rhs);

