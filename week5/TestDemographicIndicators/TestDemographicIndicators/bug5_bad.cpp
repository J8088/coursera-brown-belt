#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class Gender {
	FEMALE,
	MALE
};

struct Person {
	int age;
	Gender gender;
	bool is_employed;
};

template <typename InputIt>
int ComputeMedianAge5(InputIt range_begin, InputIt range_end) {
	if (range_begin == range_end) {
		return 0;
	}
	vector<typename iterator_traits<InputIt>::value_type> range_copy(
		range_begin,
		range_end
	);
	auto middle = begin(range_copy) + range_copy.size() / 2;
	nth_element(
		begin(range_copy), middle, end(range_copy),
		[](const Person& lhs, const Person& rhs) {
			return lhs.age < rhs.age;
		}
	);
	return middle->age;
}

int main5() {
	int person_count;
	cin >> person_count;
	vector<Person> persons;
	persons.reserve(person_count);
	for (int i = 0; i < person_count; ++i) {
		int age, gender, is_employed;
		cin >> age >> gender >> is_employed;
		Person person{
			age,
			static_cast<Gender>(gender),
			is_employed == 1
		};
		persons.push_back(person);
	}

	auto females_end = partition(
		begin(persons), end(persons),
		[](const Person& p) {
			return p.gender == Gender::FEMALE;
		}
	);
	auto employed_females_end = partition(
		begin(persons), females_end,
		[](const Person& p) {
			return p.is_employed;
		}
	);
	auto employed_males_end = partition(
		females_end, end(persons),
		[](const Person& p) {
			return p.is_employed;
		}
	);

	cout << "Median age = "
		<< ComputeMedianAge5(begin(persons), end(persons)) << endl
		<< "Median age for females = "
		<< ComputeMedianAge5(begin(persons), females_end) << endl
		<< "Median age for males = "
		<< ComputeMedianAge5(females_end, end(persons)) << endl
		<< "Median age for employed females = "
		<< ComputeMedianAge5(begin(persons), employed_females_end) << endl
		<< "Median age for unemployed females = "
		<< ComputeMedianAge5(employed_females_end, females_end) << endl
		<< "Median age for employed males = "
		<< ComputeMedianAge5(employed_males_end, females_end) << endl
		<< "Median age for unemployed males = "
		<< ComputeMedianAge5(employed_males_end, end(persons)) << endl;

	return 0;
}

