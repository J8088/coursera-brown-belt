#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include "test_runner.h"

using namespace std;

template <typename It>
class Range {
public:
	Range(It begin, It end) : begin_(begin), end_(end) {}
	It begin() const { return begin_; }
	It end() const { return end_; }
private:
	It begin_, end_;
};

vector<string_view> Split(string_view s, string_view sep = " ") {
	vector<string_view> result;
	while (!s.empty()) {
		size_t pos = s.find(sep);
		result.push_back(s.substr(0, pos));
		s.remove_prefix(pos != s.npos ? pos + 1 : s.size());
	}
	return result;
}

class Domain {
public:
	Domain(string_view domain) {
		vector<string_view> view_parts = Split(domain, ".");
		parts_.assign(view_parts.rbegin(), view_parts.rend());
	}

	auto GetReversedParts() const {
		return Range(begin(parts_), end(parts_));
	}

	size_t GetPartsCount() const {
		return parts_.size();
	}

	friend ostream& operator<<(ostream& ss, const Domain& domain);
	friend bool operator==(const Domain& lhs, const Domain& rhs);
	friend bool operator<(const Domain& lhs, const Domain& rhs);
private:
	vector<string> parts_;
};

ostream& operator<<(ostream& ss, const Domain& domain) {
	return ss << domain.parts_;
}

bool operator==(const Domain& lhs, const Domain& rhs) {
	return lhs.parts_ == rhs.parts_;
}

bool operator<(const Domain& lhs, const Domain& rhs) {
	return lexicographical_compare(lhs.parts_.begin(), lhs.parts_.end(), rhs.parts_.begin(), rhs.parts_.end());
}

vector<Domain> ReadDomains(istream& is = cin) {
	size_t count;
	is >> count;

	is.ignore();

	vector<Domain> domains;
	for (size_t i = 0; i < count; ++i) {
		string domain;
		getline(is, domain);
		domains.push_back(Domain{ domain });
	}
	return domains;
}

bool IsSubdomain(const Domain& subdomain, const Domain& domain) {
	const auto subdomain_parts = subdomain.GetReversedParts(),
		domain_parts = domain.GetReversedParts();
	return subdomain.GetPartsCount() >= domain.GetPartsCount()
		&& equal(begin(domain_parts), end(domain_parts), begin(subdomain_parts));
}

class DomainChecker {
public:
	template <typename InputIt>
	DomainChecker(InputIt begin, InputIt end)
		: domains(begin, end)
	{
		AbsorbSubdomains();
	}

	bool IsSubdomain(const Domain& subdomain) {
		auto it = domains.upper_bound(subdomain);
		return it != domains.begin() && ::IsSubdomain(subdomain, *prev(it));
	}
private:
	set<Domain> domains;

	void AbsorbSubdomains() {
		for (auto it = domains.begin(); it != domains.end();) {
			if (it != domains.begin() && ::IsSubdomain(*it, *prev(it))) {
				it = domains.erase(it);
			}
			else {
				++it;
			}
		}
	}
};

vector<bool> CheckDomains(const vector<Domain>& banned_domains, const vector<Domain>& domains_to_check) {
	DomainChecker checker(begin(banned_domains), end(banned_domains));
	vector<bool> check_results;
	for (const Domain& domain : domains_to_check) {
		check_results.push_back(!checker.IsSubdomain(domain));
	}
	return check_results;
}

void PrintCheckResults(const vector<bool> results, ostream& os = cout) {
	for (bool result : results) {
		os << (result ? "Good" : "Bad") << '\n';
	}
}

// tests
void TestSplitWithExcessEmpty() {
	ASSERT_EQUAL(
		Split("ya.ru", "."),
		vector<string_view>({ "ya", "ru" })
	);
}

void TestDomainReversedParts() {
	const Domain domain("ya.ru");
	const auto reversed_parts_range = domain.GetReversedParts();
	const vector<string_view> reversed_parts(
		begin(reversed_parts_range), end(reversed_parts_range)
	);
	ASSERT_EQUAL(
		reversed_parts,
		vector<string_view>({ "ru", "ya" })
	);
}

void TestIsSubdomainNonReflexive() {
	ASSERT_EQUAL(
		IsSubdomain(Domain("ru"), Domain("ru")),
		true
	);
}

void TestIsSubdomainMixedArgs() {
	ASSERT_EQUAL(
		IsSubdomain(Domain("m.ya.ru"), Domain("ya.ru")),
		true
	);
}

void TestDomainCheckerNoAbsorbation() {
	const vector<Domain> domains = {
		Domain("ya.ru"),
		Domain("m.ya.ru")
	};
	ASSERT_EQUAL(
		DomainChecker(begin(domains), end(domains)).IsSubdomain(Domain("m1.ya.ru")),
		true
	);
}

void TestCheckDomainsInvertBool() {
	ASSERT_EQUAL(
		CheckDomains(vector({ Domain("ya.ru") }), vector({ Domain("m.ya.ru") })),
		vector({ false })
	);
}

void TestPrintCheckResultsGoodOrBad() {
	ostringstream stream;
	PrintCheckResults(vector({ true, false }), stream);
	ASSERT_EQUAL(stream.str(), "Good\nBad\n");
}

void TestReadDomainsExcessEmpty() {
	stringstream stream;
	stream << 1 << '\n' << "ya.ru";
	ASSERT_EQUAL(
		ReadDomains(stream),
		vector({ Domain("ya.ru") })
	);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSplitWithExcessEmpty);
	RUN_TEST(tr, TestDomainReversedParts);
	RUN_TEST(tr, TestIsSubdomainNonReflexive);
	RUN_TEST(tr, TestIsSubdomainMixedArgs);
	RUN_TEST(tr, TestDomainCheckerNoAbsorbation);
	RUN_TEST(tr, TestCheckDomainsInvertBool);
	RUN_TEST(tr, TestPrintCheckResultsGoodOrBad);
	RUN_TEST(tr, TestReadDomainsExcessEmpty);

	const vector<Domain> banned_domains = ReadDomains();
	const vector<Domain> domains_to_check = ReadDomains();
	PrintCheckResults(CheckDomains(banned_domains, domains_to_check));
	return 0;
}
