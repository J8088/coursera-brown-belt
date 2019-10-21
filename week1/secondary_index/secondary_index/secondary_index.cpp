#include "test_runner.h"
#include "profile.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <deque>
#include <memory>

using namespace std;

struct Record {
	string id;
	string title;
	string user;
	int timestamp;
	int karma;
};

struct IndexIterators {
	multimap<int, shared_ptr<Record>>::iterator karma_iter;
	multimap<int, shared_ptr<Record>>::iterator timestamp_iter;
	multimap<string, shared_ptr<Record>>::iterator user_iter;
};

template <typename Iterator, typename Callback>
void IterateOverRange(Iterator begin, Iterator end, Callback callback) {
	for (auto it = begin; it != end; ++it) {
		if (!callback(*(it->second))) {
			return;
		}
	}
}

class Database {
public:
	bool Put(const Record& record) {
		auto it = make_shared<Record>(record);
		auto res = data.emplace(record.id, it);
		if (!res.second) {
			return false;
		}
		auto karma_iter = data_by_karma.emplace(record.karma, it);
		auto timestamp_iter = data_by_timestamp.emplace(record.timestamp, it);
		auto user_iter = data_by_user.emplace(record.user, it);
		index_iters[record.id] = { karma_iter, timestamp_iter, user_iter };
		return true;
	}

	const Record* GetById(const string& id) const {
		auto it = data.find(id);
		if (it == data.end()) {
			return nullptr;
		}
		return it->second.get();
	}

	bool Erase(const string& id) {
		auto it = data.find(id);
		if (it == data.end()) {
			return false;
		}
		auto iters = index_iters.find(id);
		data_by_karma.erase(iters->second.karma_iter);
		data_by_timestamp.erase(iters->second.timestamp_iter);
		data_by_user.erase(iters->second.user_iter);
		index_iters.erase(iters);
		data.erase(it);
		return true;
	}

	template <typename Callback>
	void RangeByTimestamp(int low, int high, Callback callback) const {
		auto begin = data_by_timestamp.lower_bound(low);
		auto end = data_by_timestamp.upper_bound(high);
		IterateOverRange(begin, end, callback);
	}

	template <typename Callback>
	void RangeByKarma(int low, int high, Callback callback) const {
		auto begin = data_by_karma.lower_bound(low);
		auto end = data_by_karma.upper_bound(high);
		IterateOverRange(begin, end, callback);
	}

	template <typename Callback>
	void AllByUser(const string& user, Callback callback) const {
		auto range = data_by_user.equal_range(user);
		IterateOverRange(range.first, range.second, callback);
	}
private:
	unordered_map<string, shared_ptr<Record>> data;
	unordered_map<string, IndexIterators> index_iters;
	multimap<string, shared_ptr<Record>> data_by_user;
	multimap<int, shared_ptr<Record>> data_by_timestamp;
	multimap<int, shared_ptr<Record>> data_by_karma;
};

void TestRangeBoundaries() {
	const int good_karma = 1000;
	const int bad_karma = -10;

	Database db;
	db.Put({ "id1", "Hello there", "master", 1536107260, good_karma });
	db.Put({ "id2", "O>>-<", "general2", 1536107260, bad_karma });

	int count = 0;
	db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
		++count;
		return true;
	});

	ASSERT_EQUAL(2, count);
}

void TestSameUser() {
	Database db;
	db.Put({ "id1", "Don't sell", "master", 1536107260, 1000 });
	db.Put({ "id2", "Rethink life", "master", 1536107260, 2000 });

	int count = 0;
	db.AllByUser("master", [&count](const Record&) {
		++count;
		return true;
	});

	ASSERT_EQUAL(2, count);
}

void TestReplacement() {
	const string final_body = "Feeling sad";

	Database db;
	db.Put({ "id", "Have a hand", "not-master", 1536107260, 10 });
	db.Erase("id");
	db.Put({ "id", final_body, "not-master", 1536107260, -10 });

	auto record = db.GetById("id");
	ASSERT(record != nullptr);
	ASSERT_EQUAL(final_body, record->title);
}

void TestErase() {
	const string final_body = "Feeling sad";

	Database db;
	db.Put({ "id1", "Have a hand", "master", 1536107260, 10 });
	db.Put({ "id2", "Second", "master", 1536107260, -10 });
	db.Put({ "id3", final_body, "master", 1536107260, -10 });
	db.Erase("id1");

	int count = 0;
	db.AllByUser("master", [&count](const Record&) {
		++count;
		return true;
		});

	ASSERT_EQUAL(2, count);
}

void TestHighload() {	
	Database db;
	{
		LOG_DURATION("put");
		for (int i = 1; i < 10000; i++) {
			db.Put({ "id" + to_string(i), "Have a hand", "master", 1536107260, 10 });
		}
	}

	{
		LOG_DURATION("erase");
		for (int i = 1; i < 10000; i++) {
			db.Erase("id" + to_string(i));
		}
	}

	int count = 0;
	db.AllByUser("master", [&count](const Record&) {
		++count;
		return true;
	});

	ASSERT_EQUAL(0, count);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestRangeBoundaries);
	RUN_TEST(tr, TestSameUser);
	RUN_TEST(tr, TestReplacement);
	RUN_TEST(tr, TestErase);
	RUN_TEST(tr, TestHighload);
	return 0;
}
