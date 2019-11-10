#include "Common.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

class ValueExpression : public Expression {
public:
	ValueExpression(int value)
		: value_(value)
	{}

	int Evaluate() const override {
		return value_;
	}

	string ToString() const override {
		return to_string(value_);
	}
private:
	const int value_ = 0;
};

class BinaryExpression : public Expression {
public:
	BinaryExpression(char op, ExpressionPtr left, ExpressionPtr right)
		: op_(op)
		, left_(move(left))
		, right_(move(right))
	{}

	string ToString() const override {
		ostringstream os;
		os << "(" << left_->ToString() << ")" << op_ << "(" << right_->ToString() << ")";
		return os.str();
	}
protected:
	const char op_;
	const ExpressionPtr left_, right_;
};

class SumExpression : public BinaryExpression {
public:
	SumExpression(ExpressionPtr left, ExpressionPtr right)
		: BinaryExpression('+', move(left), move(right))
	{}

	int Evaluate() const override {
		return left_->Evaluate() + right_->Evaluate();
	}
};

class ProductExpression : public BinaryExpression {
public:
	ProductExpression(ExpressionPtr left, ExpressionPtr right)
		: BinaryExpression('*', move(left), move(right))
	{}

	int Evaluate() const override {
		return left_->Evaluate() * right_->Evaluate();
	}

	string ToString() const override {
		ostringstream os;
		os << "(" << left_->ToString() << ")*(" << right_->ToString() << ")";
		return os.str();
	}
};

ExpressionPtr Value(int value) {
	return make_unique<ValueExpression>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
	return make_unique<SumExpression>(move(left), move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
	return make_unique<ProductExpression>(move(left), move(right));
}

string Print(const Expression* e) {
	if (!e) {
		return "Null expression provided";
	}
	stringstream output;
	output << e->ToString() << " = " << e->Evaluate();
	return output.str();
}

void Test() {
	ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
	ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

	ExpressionPtr e2 = Sum(move(e1), Value(5));
	ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

	ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, Test);
	return 0;
}