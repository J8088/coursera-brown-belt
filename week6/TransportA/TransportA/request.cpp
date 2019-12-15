#include "request.h"
#include "parse.h"

using namespace std;

void AddStopRequest::ParseFrom(string_view input) {
	name = ReadToken(input, ": ");
	lat = ConvertToDouble(ReadToken(input, ", "));
	lon = ConvertToDouble(input);
}

void AddStopRequest::Process(Database& db) const {
	db.AddOrUpdateStop(name, lat, lon);
}

void AddBusWithRouteRequest::ParseFrom(string_view input) {
	id = ReadToken(input, ": ");
	stops_names.push_back(string(ReadToken(input, " - "))); // first stop
	while (!input.empty()) {
		stops_names.push_back(string(ReadToken(input, " - ")));
	}
}

void AddBusWithRouteRequest::Process(Database& db) const {
	db.AddBusWithRoute(id, stops_names);
}

void AddBusWithRingRouteRequest::ParseFrom(string_view input) {
	id = ReadToken(input, ": ");
	stops_names.push_back(string(ReadToken(input, " > "))); // first stop
	while (!input.empty()) {
		stops_names.push_back(string(ReadToken(input, " > ")));
	}
}

void AddBusWithRingRouteRequest::Process(Database& db) const {
	db.AddBusWithRingRoute(id, stops_names);
}

void GetBusInfoRequest::ParseFrom(string_view input) {
	id = input;
}

BusInfoResponse GetBusInfoRequest::Process(const Database& db) const {
	return BusInfoResponse{ id, db.GetBus(id) };
}

RequestHolder Request::Create(Request::Type type) {
	switch (type) {
	case Request::Type::ADD_STOP:
		return make_unique<AddStopRequest>();
	case Request::Type::ADD_BUS_WITH_ROUTE:
		return make_unique<AddBusWithRouteRequest>();
	case Request::Type::ADD_BUS_WITH_RING_ROUTE:
		return make_unique<AddBusWithRingRouteRequest>();
	case Request::Type::GET_BUS_INFO:
		return make_unique<GetBusInfoRequest>();
	default:
		return nullptr;
	}
}

optional<Request::Type> ConvertRequestTypeFromString(Request::Mode request_mode, string_view str) {
	if (request_mode == Request::Mode::MODIFY) {
		if (str.find("Bus") == 0) {
			if (str.find('>') != str.npos) {
				return Request::Type::ADD_BUS_WITH_RING_ROUTE;
			} else {
				return Request::Type::ADD_BUS_WITH_ROUTE;
			}
		} else if (str.find("Stop") == 0) {
			return Request::Type::ADD_STOP;
		}
	} else if (request_mode == Request::Mode::READ) {
		if (str.find("Bus") == 0) {
			return Request::Type::GET_BUS_INFO;
		}
	}

	return nullopt;
}

RequestHolder ParseRequest(Request::Mode request_mode, string_view request_str) {
	const auto request_type = ConvertRequestTypeFromString(request_mode, request_str);
	if (!request_type) {
		return nullptr;
	}
	RequestHolder request = Request::Create(*request_type);
	if (request) {
		ReadToken(request_str);
		request->ParseFrom(request_str);
	};
	return request;
}

vector<RequestHolder> ReadRequests(Request::Mode request_mode, istream& in_stream) {
	const size_t request_count = ReadNumberOnLine<size_t>(in_stream);

	vector<RequestHolder> requests;
	requests.reserve(request_count);

	for (size_t i = 0; i < request_count; ++i) {
		string request_str;
		getline(in_stream, request_str);
		if (auto request = ParseRequest(request_mode, request_str)) {
			requests.push_back(move(request));
		}
	}
	return requests;
}

void ProcessInputRequests(Database& db, const vector<RequestHolder>& requests) {
	for (const auto& request_holder : requests) {
		const auto& request = static_cast<const ModifyRequest&>(*request_holder);
		request.Process(db);
	}
	db.UpdateAllBusesStats();
}

vector<BusInfoResponse> ProcessGetRequests(Database& db, const vector<RequestHolder>& requests) {
	vector<BusInfoResponse> responses;
	for (const auto& request_holder : requests) {
		const auto& request = static_cast<const GetBusInfoRequest&>(*request_holder);
		responses.push_back(request.Process(db));
	}
	return responses;
}
