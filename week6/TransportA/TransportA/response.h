#pragma once

#include <iostream>
#include <vector>
#include "bus.h"

struct Response {
	virtual void AddToStream(std::ostream& os) const = 0;
};

struct BusInfoResponse : Response {
	std::string bus_id;
	BusPtr bus;

	BusInfoResponse(const std::string& id, BusPtr b) : bus_id(id), bus(b) {}
	void AddToStream(std::ostream& os) const override;
};

std::ostream& operator<<(std::ostream& os, const Response& response);

void PrintResponses(const std::vector<BusInfoResponse>& responses, std::ostream& stream = std::cout);
