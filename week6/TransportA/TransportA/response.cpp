#include "response.h"

using namespace std;

void BusInfoResponse::AddToStream(ostream& os) const {
	os << "Bus " << bus_id << ": ";
	if (bus) {
		os << bus->GetStopsCount() << " stops on route, "
		   << bus->GetStats().unique_stops_count << " unique stops, "
		   << bus->GetStats().route_length << " route length";
	} else {
		os << "not found";
	}
}

ostream& operator<<(ostream& os, const Response& response) {
	response.AddToStream(os);
	return os;
}

void PrintResponses(const vector<BusInfoResponse>& responses, ostream& stream) {
	for (BusInfoResponse response : responses) {
		stream << response << '\n';
	}
}
