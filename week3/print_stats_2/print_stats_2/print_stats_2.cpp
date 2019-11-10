#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <optional>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
	IteratorRange(Iterator begin, Iterator end)
		: first(begin)
		, last(end)
	{
	}

	Iterator begin() const {
		return first;
	}

	Iterator end() const {
		return last;
	}

private:
	Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
	return IteratorRange{ v.begin(), next(v.begin(), min(top, v.size())) };
}

struct Person {
	string name;
	int age, income;
	bool is_male;
};

struct StatsData {
	optional<string> most_popular_male_name;
	optional<string> most_popular_female_name;
	vector<int> sum_wealth;
	vector<Person> sorted_by_age;
};

vector<Person> ReadPeople(istream& input) {
	int count;
	input >> count;

	vector<Person> result(count);
	for (Person& p : result) {
		char gender;
		input >> p.name >> p.age >> p.income >> gender;
		p.is_male = gender == 'M';
	}

	return result;
}

template <typename Iter>
optional<string> FindMostPopularName(IteratorRange<Iter> range) {
	if (range.begin() == range.end()) {
		return nullopt;
	}
	else {
		sort(range.begin(), range.end(), [](const Person& lhs, const Person& rhs) {
			return lhs.name < rhs.name;
		});
		const string* most_popular_name_ptr = &range.begin()->name;
		int count = 1;
		for (auto it = range.begin(); it != range.end(); ) {
			auto same_name_end = find_if_not(it, range.end(), [it](const Person& p) {
				return p.name == it->name;
			});
			auto cur_name_count = distance(it, same_name_end);
			if (cur_name_count > count || (cur_name_count == count && it->name < *most_popular_name_ptr)) {
				count = cur_name_count;
				most_popular_name_ptr = &it->name;
			}
			it = same_name_end;
		}
		return *most_popular_name_ptr;
	}
}

StatsData BuildStatsData(vector<Person> people) {
	StatsData result;

	{
		IteratorRange males{
			people.begin(),
			partition(people.begin(), people.end(), [](const Person& p) {
				return p.is_male;
			})
		};
		IteratorRange females{ males.end(), people.end() };

		result.most_popular_male_name = FindMostPopularName(males);
		result.most_popular_female_name = FindMostPopularName(females);
	}

	{
		sort(people.begin(), people.end(), [](const Person& lhs, const Person& rhs) {
			return lhs.income > rhs.income;
		});

		auto& wealth = result.sum_wealth;
		wealth.resize(people.size());
		if (!people.empty()) {
			wealth[0] = people[0].income;
			for (size_t i = 1; i < people.size(); i++) {
				wealth[i] = wealth[i - 1] + people[i].income;
			}
		}
	}

	sort(people.begin(), people.end(), [](const Person& lhs, const Person& rhs) {
		return lhs.age < rhs.age;
	});
	result.sorted_by_age = move(people);

	return result;
}

int main() {
	const StatsData stats = BuildStatsData(ReadPeople(cin));

	for (string command; cin >> command; ) {
		if (command == "AGE") {
			int adult_age;
			cin >> adult_age;

			auto adult_begin = lower_bound(
				begin(stats.sorted_by_age), end(stats.sorted_by_age), adult_age, [](const Person& lhs, int age) {
					return lhs.age < age;
				}
			);

			cout << "There are " << std::distance(adult_begin, end(stats.sorted_by_age))
				<< " adult people for maturity age " << adult_age << '\n';
		}
		else if (command == "WEALTHY") {
			int count;
			cin >> count;

			cout << "Top-" << count << " people have total income " << stats.sum_wealth[count - 1] << '\n';
		}
		else if (command == "POPULAR_NAME") {
			char gender;
			cin >> gender;
			const auto& most_popular_name = gender == 'M'
				? stats.most_popular_male_name
				: stats.most_popular_female_name;
			if (most_popular_name) {
				cout << "Most popular name among people of gender " << gender << " is "
					<< *most_popular_name << '\n';
			}
			else {
				cout << "No people of gender " << gender << '\n';
			}
		}
	}
}
