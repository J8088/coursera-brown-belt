#include "response.h"
#include <iostream>
#include <sstream>

using namespace std;

string BusInfoResponse::ToString() const {
	ostringstream os;
	os << "Bus " << bus_id << ": ";
	if (bus) {
		const auto& bus_stats = bus->GetStats();
		os << bus->GetStopsCount() << " stops on route, "
		   << bus_stats.unique_stops_count << " unique stops, "
		   << bus_stats.route_length << " route length, "
		   << bus_stats.curvature << " curvature";
	} else {
		os << "not found";
	}
	return os.str();
}

Json::Node BusInfoResponse::ToJson() const {
	using namespace Json;
	map<string, Node> nodes_map;
	nodes_map["request_id"] = Node((int)request_id);
	if (bus) {
		const auto& bus_stats = bus->GetStats();
		nodes_map["route_length"] = Node(bus_stats.route_length);
		nodes_map["curvature"] = Node(bus_stats.curvature);
		nodes_map["stop_count"] = Node((int)bus->GetStopsCount());
		nodes_map["unique_stop_count"] = Node((int)bus_stats.unique_stops_count);
	}
	else {
		nodes_map["error_message"] = Node(string("not found"));
	}
	return Node(nodes_map);
}

string StopInfoResponse::ToString() const {
	ostringstream os;
	os << "Stop " << stop_id << ": ";
	if (stop) {
		const auto& buses = stop->GetBuses();
		if (!buses.empty()) {
			os << "buses ";
			for (const string& bus : buses) {
				os << bus << ' ';
			}
		} else {
			os << "no buses";
		}
	} else {
		os << "not found";
	}
	return os.str();
}

Json::Node StopInfoResponse::ToJson() const {
	using namespace Json;
	map<string, Node> nodes_map;
	nodes_map["request_id"] = Node((int)request_id);
	if (stop) {
		const auto& buses = stop->GetBuses();
		vector<Node> nodes;
		for (const auto& bus : buses) {
			nodes.push_back(Node(bus));
		}
		nodes_map["buses"] = Node(nodes);
	} else {
		nodes_map["error_message"] = Node(string("not found"));
	}
	return Node(nodes_map);
}

void PrintResponses(const vector<ResponsePtr>& responses, ostream& stream) {
	for (const ResponsePtr& response : responses) {
		stream << response->ToString() << '\n';
	}
}

Json::Node ResponsesToJson(const vector<ResponsePtr>& responses) {
	using namespace Json;
	vector<Node> nodes;
	for (const ResponsePtr& response : responses) {
		nodes.push_back(response->ToJson());
	}
	return Node(nodes);
}
