#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "bus.h"
#include "json.h"

struct Response {
	Response(size_t rid) : request_id(rid) {}
	virtual std::string ToString() const = 0;
	virtual Json::Node ToJson() const = 0;

	size_t request_id = 0;
};

using ResponsePtr = std::shared_ptr<Response>;

struct BusInfoResponse : Response {
	std::string bus_id;
	BusPtr bus;

	BusInfoResponse(size_t rid, const std::string& id, BusPtr b) : Response(rid), bus_id(id), bus(b) {}
	std::string ToString() const override;
	Json::Node ToJson() const override;
};

struct StopInfoResponse : Response {
	std::string stop_id;
	StopPtr stop;

	StopInfoResponse(size_t rid, const std::string& id, StopPtr s) : Response(rid), stop_id(id), stop(s) {}
	std::string ToString() const override;
	Json::Node ToJson() const override;
};

void PrintResponses(const std::vector<ResponsePtr>& responses, std::ostream& stream = std::cout);
Json::Node ResponsesToJson(const std::vector<ResponsePtr>& responses);
