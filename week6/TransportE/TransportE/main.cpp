#include "request.h"
#include "tests.h"

using namespace std;

int main() {
	RunAllTests();

	cout.precision(6);

	try {
		Json::Document doc = Json::Load(cin);
		Database db;
		const auto base_requests = ReadJsonRequests("base_requests", doc);
		ProcessBaseRequests(db, base_requests);
		const auto settings_requests = ReadJsonRequests("routing_settings", doc);
		ProcessSettingsRequests(db, settings_requests);
		const auto stat_requests = ReadJsonRequests("stat_requests", doc);
		const auto responses = ProcessStatRequests(db, stat_requests);
		cout << ResponsesToJson(responses);
	} catch (const runtime_error& e) {
		cerr << "Exception: " << e.what() << '\n';
	}

	return 0;
}
