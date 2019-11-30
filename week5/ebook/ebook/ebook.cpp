#include <numeric>
#include <iomanip>
#include <iostream>
#include <map>
#include <utility>

using namespace std;

class ReadingManager {
public:
	void Read(int user_id, int page_count) {
		if (users_results.count(user_id)) {
			const int old_result = users_results[user_id];
			results_counts[old_result]--;
			if (results_counts[old_result] == 0) {
				results_counts.erase(old_result);
			}
		}
		users_results[user_id] = page_count;
		results_counts[page_count]++;
	}

	double Cheer(int user_id) const {
		if (!users_results.count(user_id)) {
			return 0;
		}

		const int total_users_count = users_results.size();
		if (total_users_count == 1) {
			return 1;
		}

		const int user_result = users_results.at(user_id);
		auto user_result_pos = results_counts.lower_bound(user_result);
		int worse_users_count = 0;
		for (auto it = results_counts.begin(); it != user_result_pos; it++) {
			worse_users_count += it->second;
		}

		return worse_users_count * 1.0 / (total_users_count - 1);
	}

private:
	map<int, int> users_results;
	map<int, int> results_counts;
};


int main() {
	ios::sync_with_stdio(false);

	ReadingManager manager;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;
		int user_id;
		cin >> user_id;

		if (query_type == "READ") {
			int page_count;
			cin >> page_count;
			manager.Read(user_id, page_count);
		}
		else if (query_type == "CHEER") {
			cout << setprecision(6) << manager.Cheer(user_id) << "\n";
		}
	}

	return 0;
}
