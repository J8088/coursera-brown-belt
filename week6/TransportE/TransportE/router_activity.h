#pragma once

#include <string>
#include <memory>
#include "json.h"

struct RouterActivity {
	std::string type;
	double time;

	RouterActivity(std::string rtype, double rtime) : type(rtype), time(rtime) {}
	virtual Json::Node ToJson() const = 0;
};

using RouterActivityPtr = std::shared_ptr<RouterActivity>;

struct WaitActivity : RouterActivity {
	std::string stop_name;

	WaitActivity(std::string stop, double time)
		: RouterActivity("Wait", time)
		, stop_name(stop)
	{}

	Json::Node ToJson() const override;
};

struct BusActivity : RouterActivity {
	std::string bus_id;
	size_t span_count;

	BusActivity(std::string bus, double time, size_t stops_count)
		: RouterActivity("Bus", time)
		, bus_id(bus)
		, span_count(stops_count)
	{}

	Json::Node ToJson() const override;
};
