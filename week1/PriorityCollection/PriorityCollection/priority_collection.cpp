#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <map>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
class PriorityCollection {
public:
	using Id = size_t;

	Id Add(T object) {
		Id new_id = objects.size();
		objects.push_back({ move(object) });
		priorities.insert({ 0, new_id });
		return new_id;
	}

	template <typename ObjInputIt, typename IdOutputIt>
	void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
		IdOutputIt id_it = ids_begin;
		for (ObjInputIt obj_it = range_begin; obj_it != range_end; obj_it++, id_it++) {
			*id_it = Add(move(*obj_it));
		}
	}

	bool IsValid(Id id) const {
		return id >= 0 && id < objects.size() && objects[id].priority != NONE_PRIORITY;
	}

	const T& Get(Id id) const {
		return objects[id].data;
	}

	void Promote(Id id) {
		auto& item = objects[id];
		const int old_priority = item.priority;
		const int new_priority = ++item.priority;
		priorities.erase({ old_priority, id });
		priorities.insert({ new_priority, id });
	}

	pair<const T&, int> GetMax() const {
		const auto& item = objects[priorities.rbegin()->second];
		return { item.data, item.priority };
	}

	pair<T, int> PopMax() {
		const auto max_priority_it = prev(priorities.end());
		auto& item = objects[max_priority_it->second];
		priorities.erase(max_priority_it);
		const int priority = item.priority;
		item.priority = NONE_PRIORITY;
		return { move(item.data), priority };
	}

private:
	struct PriorityObject {
		T data;
		int priority = 0;
	};
	static const int NONE_PRIORITY = -1;

	vector<PriorityObject> objects;
	set<pair<int, Id>> priorities;
};


class StringNonCopyable : public string {
public:
	using string::string;  // ѕозвол€ет использовать конструкторы строки
	StringNonCopyable(const StringNonCopyable&) = delete;
	StringNonCopyable(StringNonCopyable&&) = default;
	StringNonCopyable& operator=(const StringNonCopyable&) = delete;
	StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
	PriorityCollection<StringNonCopyable> strings;
	const auto white_id = strings.Add("white");
	const auto yellow_id = strings.Add("yellow");
	const auto red_id = strings.Add("red");

	strings.Promote(yellow_id);
	for (int i = 0; i < 2; ++i) {
		strings.Promote(red_id);
	}
	strings.Promote(yellow_id);
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "red");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "yellow");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "white");
		ASSERT_EQUAL(item.second, 0);
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestNoCopy);
	return 0;
}
