#pragma once

#include <iostream>
#include <sstream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <memory>
#include "database.h"
#include "response.h"

struct Request;
using RequestHolder = std::unique_ptr<Request>;

struct Request {
	enum class Type {
		ADD_STOP,
		ADD_BUS_WITH_ROUTE,
		ADD_BUS_WITH_RING_ROUTE,
		GET_BUS_INFO,
		GET_STOP_INFO,
	};

	enum class Mode {
		READ,
		MODIFY,
	};

	Request(Type type) : type(type) {}
	static RequestHolder Create(Type type);
	virtual void ParseFrom(std::string_view input) = 0;
	virtual ~Request() = default;

	const Type type;
};

struct ModifyRequest : Request {
	using Request::Request;
	virtual void Process(Database& db) const = 0;
};

struct AddStopRequest : ModifyRequest {
	AddStopRequest() : ModifyRequest(Type::ADD_STOP) {}
	void ParseFrom(std::string_view input) override;
	void Process(Database& db) const override;
private:
	Database::StopParams params;
};

struct AddBusWithRouteRequest : ModifyRequest {
	AddBusWithRouteRequest() : ModifyRequest(Type::ADD_BUS_WITH_ROUTE) {}
	void ParseFrom(std::string_view input) override;
	void Process(Database& db) const override;
private:
	Database::BusParams params;
};

struct AddBusWithRingRouteRequest : ModifyRequest {
	AddBusWithRingRouteRequest() : ModifyRequest(Type::ADD_BUS_WITH_RING_ROUTE) {}
	void ParseFrom(std::string_view input) override;
	void Process(Database& db) const override;
private:
	Database::BusParams params;
};


struct ReadRequest : Request {
	using Request::Request;
	virtual ResponsePtr Process(const Database& db) const = 0;
};

struct GetBusInfoRequest : ReadRequest {
	GetBusInfoRequest() : ReadRequest(Type::GET_BUS_INFO) {}
	void ParseFrom(std::string_view input) override;
	ResponsePtr Process(const Database& db) const override;
private:
	std::string id;
};

struct GetStopInfoRequest : ReadRequest {
	GetStopInfoRequest() : ReadRequest(Type::GET_STOP_INFO) {}
	void ParseFrom(std::string_view input) override;
	ResponsePtr Process(const Database& db) const override;
private:
	std::string id;
};

template <typename Number>
Number ReadNumberOnLine(std::istream& stream) {
	Number number;
	stream >> number;
	std::string dummy;
	getline(stream, dummy);
	return number;
}

std::optional<Request::Type> ConvertRequestTypeFromString(Request::Mode request_mode, std::string_view str);
RequestHolder ParseRequest(Request::Mode request_mode, std::string_view request_str);

std::vector<RequestHolder> ReadRequests(Request::Mode request_mode, std::istream& in_stream = std::cin);
void ProcessInputRequests(Database& db, const std::vector<RequestHolder>& requests);
std::vector<ResponsePtr> ProcessGetRequests(Database& db, const std::vector<RequestHolder>& requests);
