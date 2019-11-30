#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

#define DECLARE_INT_PARAM(Name)					\
	struct Name {								\
		int value;								\
		explicit Name(int v) : value(v) {}		\
		operator int() const { return value; }	\
	};

DECLARE_INT_PARAM(Year);
DECLARE_INT_PARAM(Month);
DECLARE_INT_PARAM(Day);

struct Date {
	Date() {}
	Date(Year year, Month month, Day day)
		: year_(year)
		, month_(month)
		, day_(day)
	{
	}

	time_t AsTimestamp() const {
		std::tm t;
		t.tm_sec = 0;
		t.tm_min = 0;
		t.tm_hour = 0;
		t.tm_mday = day_;
		t.tm_mon = month_ - 1;
		t.tm_year = year_ - 1900;
		t.tm_isdst = 0;
		return mktime(&t);
	}

	friend istream& operator>>(istream& is, Date& date);
private:
	Year year_ = Year{ 0 };
	Month month_ = Month{ 1 };
	Day day_ = Day{ 1 };
};

istream& operator>>(istream& is, Date& date) {
	is >> date.year_.value;
	is.ignore();
	is >> date.month_.value;
	is.ignore();
	is >> date.day_.value;
	return is;
}

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
	const time_t timestamp_to = date_to.AsTimestamp();
	const time_t timestamp_from = date_from.AsTimestamp();
	static const int SECONDS_IN_DAY = 60 * 60 * 24;
	return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

int DaysFrom2000(const Date& date) {
	static const time_t timestamp_from = Date{ Year{ 2000 }, Month{ 1 }, Day{ 1 } }.AsTimestamp();
	static const int SECONDS_IN_DAY = 60 * 60 * 24;
	const time_t timestamp_to = date.AsTimestamp();
	return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

const int MAX_DAYS = 365 * 100;

class Budget {
public:
	Budget()
		: incomes(MAX_DAYS)
	{
	}

	void Earn(const Date& date_from, const Date& date_to, double sum) {
		double income_per_day = sum / (ComputeDaysDiff(date_to, date_from) + 1);
		int start = DaysFrom2000(date_from), end = DaysFrom2000(date_to);
		for (int i = start; i <= end; ++i) {
			incomes[i] += income_per_day;
		}
	}

	void PayTax(const Date& date_from, const Date& date_to) {
		int start = DaysFrom2000(date_from), end = DaysFrom2000(date_to);
		for (int i = start; i <= end; ++i) {
			incomes[i] *= 0.87;
		}
	}

	double ComputeIncome(const Date& date_from, const Date& date_to) {
		double sum = 0;
		int start = DaysFrom2000(date_from), end = DaysFrom2000(date_to);
		for (int i = start; i <= end; ++i) {
			sum += incomes[i];
		}
		return sum;
	}
private:
	vector<double> incomes;
};

int main() {
	int q;
	cin >> q;

	Budget budget;
	for (int i = 0; i < q; ++i) {
		string command;
		Date from, to;
		cin >> command >> from >> to;
		if (command == "Earn") {
			double value;
			cin >> value;
			budget.Earn(from, to, value);
		}
		else if (command == "PayTax") {
			budget.PayTax(from, to);
		}
		else {
			cout << fixed << setprecision(25) << budget.ComputeIncome(from, to) << '\n';
		}
	}

	return 0;
}
