#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "bus.h"

struct Response {
	virtual void AddToStream(std::ostream& os) const = 0;
};

using ResponsePtr = std::shared_ptr<Response>;

struct BusInfoResponse : Response {
	std::string bus_id;
	BusPtr bus;

	BusInfoResponse(const std::string& id, BusPtr b) : bus_id(id), bus(b) {}
	void AddToStream(std::ostream& os) const override;
};

struct StopInfoResponse : Response {
	std::string stop_id;
	StopPtr stop;

	StopInfoResponse(const std::string& id, StopPtr s) : stop_id(id), stop(s) {}
	void AddToStream(std::ostream& os) const override;
};

std::ostream& operator<<(std::ostream& os, const Response& response);

void PrintResponses(const std::vector<ResponsePtr>& responses, std::ostream& stream = std::cout);
