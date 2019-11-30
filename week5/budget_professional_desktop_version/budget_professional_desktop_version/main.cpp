#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

struct Date {
	Date() {}
	Date(int year, int month, int day)
		: year_(year), month_(month), day_(day)
	{}

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
	int year_;
	int month_;
	int day_;
};

istream& operator>>(istream& is, Date& date) {
	is >> date.year_;
	is.ignore();
	is >> date.month_;
	is.ignore();
	is >> date.day_;
	return is;
}

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
	const time_t timestamp_to = date_to.AsTimestamp();
	const time_t timestamp_from = date_from.AsTimestamp();
	static constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
	return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

static const Date DATE_START = Date{ 2000, 1, 1 };
static const Date DATE_END = Date{ 2100, 1, 1 };
static const size_t MAX_DAYS = ComputeDaysDiff(DATE_END, DATE_START);

int ComputeDayIndex(const Date& date) {
	return ComputeDaysDiff(date, DATE_START);
}

class Budget {
public:
	Budget()
		: stats(MAX_DAYS)
	{
	}

	void Earn(const Date& date_from, const Date& date_to, double sum) {
		double income_per_day = sum / (ComputeDaysDiff(date_to, date_from) + 1);
		int start = ComputeDayIndex(date_from), end = ComputeDayIndex(date_to);
		for (int i = start; i <= end; ++i) {
			stats[i].earned += income_per_day;
		}
	}

	void Spend(const Date& date_from, const Date& date_to, double sum) {
		double amount_per_day = sum / (ComputeDaysDiff(date_to, date_from) + 1);
		int start = ComputeDayIndex(date_from), end = ComputeDayIndex(date_to);
		for (int i = start; i <= end; ++i) {
			stats[i].spent += amount_per_day;
		}
	}

	void PayTax(const Date& date_from, const Date& date_to, int percentage) {
		double tax = ((double)(100 - percentage)) / 100.0;
		int start = ComputeDayIndex(date_from), end = ComputeDayIndex(date_to);
		for (int i = start; i <= end; ++i) {
			stats[i].earned *= tax;
		}
	}

	double ComputeIncome(const Date& date_from, const Date& date_to) {
		double sum = 0;
		int start = ComputeDayIndex(date_from), end = ComputeDayIndex(date_to);
		for (int i = start; i <= end; ++i) {
			sum += stats[i].ComputeIncome();
		}
		return sum;
	}
private:
	struct DayStats {
		double earned;
		double spent;

		double ComputeIncome() const {
			return earned - spent;
		}
	};

	vector<DayStats> stats;
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
		else if (command == "Spend") {
			double value;
			cin >> value;
			budget.Spend(from, to, value);
		}
		else if (command == "PayTax") {
			int percentage;
			cin >> percentage;
			budget.PayTax(from, to, percentage);
		}
		else {
			cout << fixed << setprecision(25) << budget.ComputeIncome(from, to) << '\n';
		}
	}

	return 0;
}
