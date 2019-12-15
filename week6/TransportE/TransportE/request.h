#pragma once

#include <iostream>
#include <sstream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <memory>
#include "database.h"
#include "response.h"
#include "json.h"

struct Request;
using RequestHolder = std::unique_ptr<Request>;

struct Request {
	enum class Type {
		ADD_STOP,
		ADD_BUS_WITH_ROUTE,
		ADD_BUS_WITH_RING_ROUTE,
		ADD_ROUTER_SETTINGS,
		GET_BUS_INFO,
		GET_STOP_INFO,
		GET_ROUTE_BETWEEN_STOPS,
	};

	enum class Mode {
		READ,
		MODIFY,
	};

	Request(Type type) : type(type) {}
	static RequestHolder Create(Type type);
	virtual void ParseFrom(std::string_view input) {};
	virtual void ParseFrom(const Json::Node& node) = 0;
	virtual ~Request() = default;

	const Type type;
	size_t request_id = 0;
};

struct ModifyRequest : Request {
	using Request::Request;
	virtual void Process(Database& db) const = 0;
};

struct AddStopRequest : ModifyRequest {
	AddStopRequest() : ModifyRequest(Type::ADD_STOP) {}
	void ParseFrom(std::string_view input) override;
	void ParseFrom(const Json::Node& node) override;
	void Process(Database& db) const override;
private:
	Database::StopParams params;
};

struct AddBusWithRouteRequest : ModifyRequest {
	AddBusWithRouteRequest() : ModifyRequest(Type::ADD_BUS_WITH_ROUTE) {}
	void ParseFrom(std::string_view input) override;
	void ParseFrom(const Json::Node& node) override;
	void Process(Database& db) const override;
private:
	Database::BusParams params;
};

struct AddBusWithRingRouteRequest : ModifyRequest {
	AddBusWithRingRouteRequest() : ModifyRequest(Type::ADD_BUS_WITH_RING_ROUTE) {}
	void ParseFrom(std::string_view input) override;
	void ParseFrom(const Json::Node& node) override;
	void Process(Database& db) const override;
private:
	Database::BusParams params;
};

struct AddRouterSettingsRequest : ModifyRequest {
	AddRouterSettingsRequest() : ModifyRequest(Type::ADD_ROUTER_SETTINGS) {}
	void ParseFrom(const Json::Node& node) override;
	void Process(Database& db) const override;
private:
	Database::RouterSettings params;
};


struct ReadRequest : Request {
	using Request::Request;
	virtual ResponsePtr Process(const Database& db) const = 0;
};

struct GetBusInfoRequest : ReadRequest {
	GetBusInfoRequest() : ReadRequest(Type::GET_BUS_INFO) {}
	void ParseFrom(std::string_view input) override;
	void ParseFrom(const Json::Node& node) override;
	ResponsePtr Process(const Database& db) const override;
private:
	std::string bus_id;
};

struct GetStopInfoRequest : ReadRequest {
	GetStopInfoRequest() : ReadRequest(Type::GET_STOP_INFO) {}
	void ParseFrom(std::string_view input) override;
	void ParseFrom(const Json::Node& node) override;
	ResponsePtr Process(const Database& db) const override;
private:
	std::string stop_id;
};

struct GetRouteBetweenStopsRequest : ReadRequest {
	GetRouteBetweenStopsRequest() : ReadRequest(Type::GET_ROUTE_BETWEEN_STOPS) {}
	void ParseFrom(const Json::Node& node) override;
	ResponsePtr Process(const Database& db) const override;
private:
	std::string from, to;
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
std::optional<Request::Type> ConvertRequestTypeFromJson(Request::Mode request_mode, const Json::Node& node);

RequestHolder ParseRequest(Request::Mode request_mode, std::string_view request_str);
RequestHolder ParseRequest(Request::Mode request_mode, const Json::Node& json_request);

std::vector<RequestHolder> ReadRequests(Request::Mode request_mode, std::istream& in_stream = std::cin);
std::vector<RequestHolder> ReadJsonRequests(const std::string& requests_section, const Json::Document& doc);

void ProcessBaseRequests(Database& db, const std::vector<RequestHolder>& requests);
void ProcessSettingsRequests(Database& db, const std::vector<RequestHolder>& requests);
std::vector<ResponsePtr> ProcessStatRequests(Database& db, const std::vector<RequestHolder>& requests);

