#include "request.h"
#include "tests.h"

using namespace std;

int main() {
	//RunAllTests();

	cout.precision(6);

	try {
		Json::Document doc = Json::Load(cin);
		Database db;
		const auto input_requests = ReadJsonRequests(Request::Mode::MODIFY, doc);
		ProcessInputRequests(db, input_requests);
		const auto get_requests = ReadJsonRequests(Request::Mode::READ, doc);
		const auto responses = ProcessGetRequests(db, get_requests);
		cout << ResponsesToJson(responses);
	} catch (const runtime_error& e) {
		cerr << "Exception: " << e.what() << '\n';
	}

	return 0;
}
