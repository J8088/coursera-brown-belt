#include "parse.h"
#include <string>
#include <sstream>

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter) {
	const size_t pos = s.find(delimiter);
	if (pos == s.npos) {
		return { s, nullopt };
	}
	else {
		return { s.substr(0, pos), s.substr(pos + delimiter.length()) };
	}
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter) {
	const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
	return { lhs, rhs_opt.value_or("") };
}

string_view ReadToken(string_view& s, string_view delimiter) {
	const auto [lhs, rhs] = SplitTwo(s, delimiter);
	s = rhs;
	return lhs;
}

int ConvertToInt(string_view str) {
	// use std::from_chars when available to git rid of string copy
	size_t pos;
	const int result = stoi(string(str), &pos);
	if (pos != str.length()) {
		std::stringstream error;
		error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
		throw invalid_argument(error.str());
	}
	return result;
}

double ConvertToDouble(string_view str) {
	size_t pos;
	const double result = stod(string(str), &pos);
	if (pos != str.length()) {
		stringstream error;
		error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
		throw invalid_argument(error.str());
	}
	return result;
}

double ConvertFromKmPerHourToMPerMin(double kmph) {
	return kmph * 1000.0 / 60.0;
}
