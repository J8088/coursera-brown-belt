#pragma once

#include <string>
#include <memory>

struct Stop {
	struct Coords {
		double lat = 0.0;
		double lon = 0.0;
	};

	Stop(const std::string& stop_name, double lat_in_degrees = 0.0, double lon_in_degrees = 0.0);
	void SetCoords(double lat_in_degrees, double lon_in_degrees);
	Coords GetCoordsInRadians() const;

	friend double ComputeDistanceBetweenStops(const Stop& lhs, const Stop& rhs);
private:
	std::string name;
	Coords coords;
};

using StopPtr = std::shared_ptr<Stop>;

double DegreesToRadians(double degree);
double ComputeDistanceBetweenStops(const Stop& lhs, const Stop& rhs);
