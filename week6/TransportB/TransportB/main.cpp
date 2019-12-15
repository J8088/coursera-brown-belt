#include "request.h"

using namespace std;

int main() {
	cout.precision(6);

	Database db;
	const auto input_requests = ReadRequests(Request::Mode::MODIFY);
	ProcessInputRequests(db, input_requests);
	const auto get_requests = ReadRequests(Request::Mode::READ);
	const auto responses = ProcessGetRequests(db, get_requests);
	PrintResponses(responses);

	return 0;
}
