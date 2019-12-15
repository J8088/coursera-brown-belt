#include "request.h"
#include "tests.h"

using namespace std;

int main() {
	cout.precision(6);

	try {
		Database db;
		const auto input_requests = ReadRequests(Request::Mode::MODIFY);
		ProcessInputRequests(db, input_requests);
		const auto get_requests = ReadRequests(Request::Mode::READ);
		const auto responses = ProcessGetRequests(db, get_requests);
		PrintResponses(responses);
	} catch (const runtime_error& e) {
		cerr << "Exception: " << e.what() << '\n';
	}

	return 0;
}
