#include "test_runner.h"

#include <iostream>
#include <string>

using namespace std;

void Assert(bool b, const string& hint = {}) {
	AssertEqual(b, true, hint);
}

TestRunner::~TestRunner() {
	if (fail_count > 0) {
		cerr << fail_count << " tests failed" << endl;
		exit(1);
	}
}
