#include <algorithm>
#include <functional>
#include <cmath>
#include <limits>
#include <cfloat>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

using namespace std;

typedef int64_t I64;
typedef vector<int> vi;
typedef vector<bool> vb;
typedef pair<int, int> pi;

#define fr(i,a,b) for (int i = a; i <= b; i++)
#define fr2(i,a,b) for (int i = a; i < b; i++)

enum class Gender {
	FEMALE,
	MALE
};

struct Person {
	int age;  // возраст
	Gender gender;  // пол
	bool is_employed;  // имеет ли работу
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
	if (range_begin == range_end) {
		return 0;
	}
	vector<typename InputIt::value_type> range_copy(range_begin, range_end);
	auto middle = begin(range_copy) + range_copy.size() / 2;
	nth_element(
		begin(range_copy), middle, end(range_copy),
		[](const Person& lhs, const Person& rhs) {
		return lhs.age < rhs.age;
	}
	);
	return middle->age;
}

void PrintStats(vector<Person> persons) {
	cout << "Median age = " << ComputeMedianAge(begin(persons), end(persons)) << endl;

	auto females_end = partition(begin(persons), end(persons), [](const Person& x) {
		return x.gender == Gender::FEMALE;
	});
	cout << "Median age for females = " << ComputeMedianAge(begin(persons), females_end) << endl;
	cout << "Median age for males = " << ComputeMedianAge(females_end, end(persons)) << endl;

	auto employed_females_end = partition(begin(persons), females_end, [](const Person& x) {
		return x.is_employed;
	});
	cout << "Median age for employed females = " << ComputeMedianAge(begin(persons), employed_females_end) << endl;
	cout << "Median age for unemployed females = " << ComputeMedianAge(employed_females_end, females_end) << endl;

	auto employed_males_end = partition(females_end, end(persons), [](const Person& x) {
		return x.is_employed;
	});
	cout << "Median age for employed males = " << ComputeMedianAge(females_end, employed_males_end) << endl;
	cout << "Median age for unemployed males = " << ComputeMedianAge(employed_males_end, end(persons)) << endl;
}

int main() {
	vector<Person> persons = {
		{ 31, Gender::MALE, false },
		{ 40, Gender::FEMALE, true },
		{ 24, Gender::MALE, true },
		{ 20, Gender::FEMALE, true },
		{ 80, Gender::FEMALE, false },
		{ 78, Gender::MALE, false },
		{ 10, Gender::FEMALE, false },
		{ 55, Gender::MALE, true },
	};

	PrintStats(persons);

	return 0;
}
