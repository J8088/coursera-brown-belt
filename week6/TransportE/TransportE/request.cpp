#include "request.h"
#include "parse.h"

using namespace std;

void AddStopRequest::ParseFrom(string_view input) {
	params.name = ReadToken(input, ": ");
	params.lat = ConvertToDouble(ReadToken(input, ", "));
	params.lon = ConvertToDouble(ReadToken(input, ", "));
	while (!input.empty()) {
		int distance = ConvertToInt(ReadToken(input, "m to "));
		string other_stop = string(ReadToken(input, ", "));
		params.distances[move(other_stop)] = distance;
	}
}

void AddStopRequest::ParseFrom(const Json::Node& node) {
	using namespace Json;

	const auto& attrs = node.AsMap();
	params.name = attrs.at("name").AsString();
	params.lat = attrs.at("latitude").AsDouble();
	params.lon = attrs.at("longitude").AsDouble();

	const auto& distances = attrs.at("road_distances").AsMap();
	for (const auto& [other_stop, distanceNode] : distances) {
		int distance = distanceNode.AsInt();
		params.distances[other_stop] = distance;
	}
}

void AddStopRequest::Process(Database& db) const {
	db.AddOrUpdateStop(move(params));
}


void AddBusWithRouteRequest::ParseFrom(string_view input) {
	params.id = ReadToken(input, ": ");
	while (!input.empty()) {
		params.stops_names.push_back(string(ReadToken(input, " - ")));
	}
}

void AddBusWithRouteRequest::ParseFrom(const Json::Node& node) {
	using namespace Json;

	const auto& attrs = node.AsMap();
	params.id = attrs.at("name").AsString();

	const auto& stops_nodes = attrs.at("stops").AsArray();
	for (const auto& stop_node : stops_nodes) {
		params.stops_names.push_back(stop_node.AsString());
	}
}

void AddBusWithRouteRequest::Process(Database& db) const {
	db.AddBusWithRoute(move(params));
}


void AddBusWithRingRouteRequest::ParseFrom(string_view input) {
	params.id = ReadToken(input, ": ");
	while (!input.empty()) {
		params.stops_names.push_back(string(ReadToken(input, " > ")));
	}
}

void AddBusWithRingRouteRequest::ParseFrom(const Json::Node& node) {
	using namespace Json;

	const auto& attrs = node.AsMap();
	params.id = attrs.at("name").AsString();

	const auto& stops_nodes = attrs.at("stops").AsArray();
	for (const auto& stop_node : stops_nodes) {
		params.stops_names.push_back(stop_node.AsString());
	}
}

void AddBusWithRingRouteRequest::Process(Database& db) const {
	db.AddBusWithRingRoute(move(params));
}


void AddRouterSettingsRequest::ParseFrom(const Json::Node& node) {
	using namespace Json;

	const auto& attrs = node.AsMap();
	params.bus_wait_time = attrs.at("bus_wait_time").AsInt();
	params.bus_velocity = ConvertFromKmPerHourToMPerMin(attrs.at("bus_velocity").AsDouble());
}

void AddRouterSettingsRequest::Process(Database& db) const {
	db.SetRouterSettings(move(params));
}


void GetBusInfoRequest::ParseFrom(string_view input) {
	bus_id = input;
}

void GetBusInfoRequest::ParseFrom(const Json::Node& node) {
	using namespace Json;

	const auto& attrs = node.AsMap();
	bus_id = attrs.at("name").AsString();
	request_id = attrs.at("id").AsInt();
}

ResponsePtr GetBusInfoRequest::Process(const Database& db) const {
	return make_shared<BusInfoResponse>(request_id, bus_id, db.GetBus(bus_id));
}


void GetStopInfoRequest::ParseFrom(string_view input) {
	stop_id = input;
}

void GetStopInfoRequest::ParseFrom(const Json::Node& node) {
	using namespace Json;

	const auto& attrs = node.AsMap();
	stop_id = attrs.at("name").AsString();
	request_id = attrs.at("id").AsInt();
}

ResponsePtr GetStopInfoRequest::Process(const Database& db) const {
	return make_shared<StopInfoResponse>(request_id, stop_id, db.GetStop(stop_id));
}


void GetRouteBetweenStopsRequest::ParseFrom(const Json::Node& node) {
	using namespace Json;

	const auto& attrs = node.AsMap();
	from = attrs.at("from").AsString();
	to = attrs.at("to").AsString();
	request_id = attrs.at("id").AsInt();
}

ResponsePtr GetRouteBetweenStopsRequest::Process(const Database& db) const {
	StopPtr from_stop = db.GetStop(from);
	StopPtr to_stop = db.GetStop(to);
	TransportRouterPtr router = db.GetRouter();
	TransportGraphPtr graph = db.GetGraph();
	const auto& route = router->BuildRoute(from_stop->GetIndex(), to_stop->GetIndex());
	if (route) {
		const int n = route->edge_count;
		vector<RouterActivityPtr> activities(2 * n);
		for (int i = 0; i < n; ++i) {
			const auto edge_id = router->GetRouteEdge(route->id, i);
			activities[2 * i] = db.GetWaitActivityByEdge(edge_id);
			activities[2 * i + 1] = db.GetBusActivityByEdge(edge_id);
		}
		router->ReleaseRoute(route->id);
		return make_shared<RouteBetweenStopsInfoResponse>(true, request_id, route->weight, move(activities));
	} else {
		return make_shared<RouteBetweenStopsInfoResponse>(false, request_id);
	}
}


RequestHolder Request::Create(Request::Type type) {
	switch (type) {
	case Request::Type::ADD_STOP:
		return make_unique<AddStopRequest>();
	case Request::Type::ADD_BUS_WITH_ROUTE:
		return make_unique<AddBusWithRouteRequest>();
	case Request::Type::ADD_BUS_WITH_RING_ROUTE:
		return make_unique<AddBusWithRingRouteRequest>();
	case Request::Type::ADD_ROUTER_SETTINGS:
		return make_unique<AddRouterSettingsRequest>();
	case Request::Type::GET_BUS_INFO:
		return make_unique<GetBusInfoRequest>();
	case Request::Type::GET_STOP_INFO:
		return make_unique<GetStopInfoRequest>();
	case Request::Type::GET_ROUTE_BETWEEN_STOPS:
		return make_unique<GetRouteBetweenStopsRequest>();
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
		} else if (str.find("Stop") == 0) {
			return Request::Type::GET_STOP_INFO;
		}
	}

	return nullopt;
}

optional<Request::Type> ConvertRequestTypeFromJson(Request::Mode request_mode, const Json::Node& node) {
	using namespace Json;

	const string& type = node.AsMap().at("type").AsString();
	if (request_mode == Request::Mode::MODIFY) {
		if (type == "Bus") {
			if (node.AsMap().at("is_roundtrip").AsBool()) {
				return Request::Type::ADD_BUS_WITH_RING_ROUTE;
			} else {
				return Request::Type::ADD_BUS_WITH_ROUTE;
			}
		} else if (type == "Stop") {
			return Request::Type::ADD_STOP;
		}
	} else if (request_mode == Request::Mode::READ) {
		if (type == "Bus") {
			return Request::Type::GET_BUS_INFO;
		} else if (type == "Stop") {
			return Request::Type::GET_STOP_INFO;
		} else if (type == "Route") {
			return Request::Type::GET_ROUTE_BETWEEN_STOPS;
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
		request->ParseFrom(string_view(request_str));
	};
	return request;
}

RequestHolder ParseRequest(Request::Mode request_mode, const Json::Node& json_request) {
	using namespace Json;

	const auto request_type = ConvertRequestTypeFromJson(request_mode, json_request);
	if (!request_type) {
		return nullptr;
	}
	RequestHolder request = Request::Create(*request_type);
	if (request) {
		request->ParseFrom(json_request);
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
		if (auto request = ParseRequest(request_mode, string_view(request_str))) {
			requests.push_back(move(request));
		}
	}
	return requests;
}

vector<RequestHolder> ReadJsonRequests(const std::string& requests_section, const Json::Document& doc) {
	using namespace Json;

	static const unordered_map<string, Request::Mode> sections_to_mode = {
		{"base_requests", Request::Mode::MODIFY},
		{"routing_settings", Request::Mode::MODIFY},
		{"stat_requests", Request::Mode::READ},
	};
	const auto request_mode = sections_to_mode.at(requests_section);

	const Node& root = doc.GetRoot();

	if (requests_section == "routing_settings") {
		vector<RequestHolder> requests;
		requests.reserve(1);
		RequestHolder request = Request::Create(Request::Type::ADD_ROUTER_SETTINGS);
		if (request) {
			request->ParseFrom(root.AsMap().at(requests_section));
			requests.push_back(move(request));
		};
		return requests;
	} else {
		const vector<Node>& json_requests = root.AsMap().at(requests_section).AsArray();
		const size_t request_count = json_requests.size();

		vector<RequestHolder> requests;
		requests.reserve(request_count);

		for (const Node& json_request : json_requests) {
			if (auto request = ParseRequest(request_mode, json_request)) {
				requests.push_back(move(request));
			}
		}
		return requests;
	}
}

void ProcessBaseRequests(Database& db, const vector<RequestHolder>& requests) {
	for (const auto& request_holder : requests) {
		const auto& request = static_cast<const ModifyRequest&>(*request_holder);
		request.Process(db);
	}
	db.UpdateAllBusesStats();
}

void ProcessSettingsRequests(Database& db, const std::vector<RequestHolder>& requests) {
	for (const auto& request_holder : requests) {
		const auto& request = static_cast<const ModifyRequest&>(*request_holder);
		request.Process(db);
	}
	db.UpdateGraphAndRouter();
}

vector<ResponsePtr> ProcessStatRequests(Database& db, const vector<RequestHolder>& requests) {
	vector<ResponsePtr> responses;
	for (const auto& request_holder : requests) {
		const auto& request = static_cast<const ReadRequest&>(*request_holder);
		responses.push_back(request.Process(db));
	}
	return responses;
}
