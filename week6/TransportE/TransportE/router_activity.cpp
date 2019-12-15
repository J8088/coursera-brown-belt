#include "router_activity.h"

using namespace std;

Json::Node WaitActivity::ToJson() const {
	using namespace Json;
	map<string, Node> nodes_map;
	nodes_map["type"] = type;
	nodes_map["stop_name"] = stop_name;
	nodes_map["time"] = time;
	return Node(nodes_map);
}

Json::Node BusActivity::ToJson() const {
	using namespace Json;
	map<string, Node> nodes_map;
	nodes_map["type"] = type;
	nodes_map["bus"] = bus_id;
	nodes_map["span_count"] = Node((int)span_count);
	nodes_map["time"] = time;
	return Node(nodes_map);
}
