#include "test_runner.h"
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
	string from;
	string to;
	string body;
};


class Worker {
public:
	virtual ~Worker() = default;
	virtual void Process(unique_ptr<Email> email) = 0;
	virtual void Run() {
		// только первому worker-у в пайплайне нужно это имплементировать
		throw logic_error("Unimplemented");
	}

protected:
	void PassOn(unique_ptr<Email> email) const {
		if (next_) {
			next_->Process(move(email));
		}
	}

public:
	void SetNext(unique_ptr<Worker> next) {
		next_ = move(next);
	}

	unique_ptr<Worker> next_;
};


class Reader : public Worker {
public:
	explicit Reader(istream& is)
		: is_(is)
	{
	}

	void Process(unique_ptr<Email> email) override {
		PassOn(move(email));
	}

	void Run() override {
		while (is_) {
			string from, to, body;
			getline(is_, from);
			if (!is_) break;
			getline(is_, to);
			if (!is_) break;
			getline(is_, body);
			PassOn(make_unique<Email>(Email{ from, to, body }));
		}
	}
private:
	istream& is_;
};


class Filter : public Worker {
public:
	using Function = function<bool(const Email&)>;
public:
	explicit Filter(Function pred)
		: pred_(pred)
	{
	}

	void Process(unique_ptr<Email> email) override {
		if (pred_(*email)) {
			PassOn(move(email));
		}
	}

private:
	Function pred_;
};


class Copier : public Worker {
public:
	explicit Copier(const string& to)
		: to_(to)
	{
	}

	void Process(unique_ptr<Email> email) override {
		string from = email->from, orig_to = email->to, body = email->body;
		PassOn(move(email));
		if (orig_to != to_) {
			PassOn(make_unique<Email>(Email{ from, to_, body }));
		}
	}
private:
	string to_;
};


class Sender : public Worker {
public:
	explicit Sender(ostream& os)
		: os_(os)
	{
	}

	void Process(unique_ptr<Email> email) override {
		os_ << email->from << '\n' << email->to << '\n' << email->body << '\n';
		PassOn(move(email));
	}
private:
	ostream& os_;
};


class PipelineBuilder {
public:
	explicit PipelineBuilder(istream& in)
		: head(make_unique<Reader>(in))
		, current(head.get())
	{
	}

	PipelineBuilder& FilterBy(Filter::Function filter) {
		auto next = make_unique<Filter>(filter);
		current->SetNext(move(next));
		current = current->next_.get();
		return *this;
	}

	PipelineBuilder& CopyTo(string recipient) {
		auto next = make_unique<Copier>(recipient);
		current->SetNext(move(next));
		current = current->next_.get();
		return *this;
	}

	PipelineBuilder& Send(ostream& out) {
		auto next = make_unique<Sender>(out);
		current->SetNext(move(next));
		current = current->next_.get();
		return *this;
	}

	unique_ptr<Worker> Build() {
		return move(head);
	}
private:
	unique_ptr<Worker> head;
	Worker* current;
};


void TestSanity() {
	string input = (
		"erich@example.com\n"
		"richard@example.com\n"
		"Hello there\n"

		"erich@example.com\n"
		"ralph@example.com\n"
		"Are you sure you pressed the right button?\n"

		"ralph@example.com\n"
		"erich@example.com\n"
		"I do not make mistakes of that kind\n"
	);
	istringstream inStream(input);
	ostringstream outStream;

	PipelineBuilder builder(inStream);
	builder.FilterBy([](const Email& email) {
		return email.from == "erich@example.com";
	});
	builder.CopyTo("richard@example.com");
	builder.Send(outStream);
	auto pipeline = builder.Build();

	pipeline->Run();

	string expectedOutput = (
		"erich@example.com\n"
		"richard@example.com\n"
		"Hello there\n"

		"erich@example.com\n"
		"ralph@example.com\n"
		"Are you sure you pressed the right button?\n"

		"erich@example.com\n"
		"richard@example.com\n"
		"Are you sure you pressed the right button?\n"
	);

	ASSERT_EQUAL(expectedOutput, outStream.str());
}

void TestEmptyMsg() {
	string input = (
		"erich@example.com\n"
		"richard@example.com\n"
		"\n"
	);
	istringstream inStream(input);
	ostringstream outStream;

	PipelineBuilder builder(inStream);
	builder.Send(outStream);
	auto pipeline = builder.Build();

	pipeline->Run();

	string expectedOutput = (
		"erich@example.com\n"
		"richard@example.com\n"
		"\n"
	);

	ASSERT_EQUAL(expectedOutput, outStream.str());
}

void TestIncorrect() {
	string input = (
		"erich@example.com\n"
	);
	istringstream inStream(input);
	ostringstream outStream;

	PipelineBuilder builder(inStream);
	builder.Send(outStream);
	auto pipeline = builder.Build();

	pipeline->Run();

	string expectedOutput = "";

	ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSanity);
	RUN_TEST(tr, TestEmptyMsg);
	RUN_TEST(tr, TestIncorrect);
	return 0;
}
