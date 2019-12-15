#include "json.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>

using namespace std;

namespace Json {

	Document::Document(Node root) : root(move(root)) {
	}

	const Node& Document::GetRoot() const {
		return root;
	}

	Node LoadNode(istream& input);

	Node LoadArray(istream& input) {
		vector<Node> result;

		for (char c; input >> c && c != ']'; ) {
			if (c != ',') {
				input.putback(c);
			}
			result.push_back(LoadNode(input));
		}

		return Node(move(result));
	}

	Node LoadIntOrDouble(istream& input) {
		bool is_negative = false, is_double = false, is_frac_part = false;
		int int_result = 0;
		double double_result = 0.0, factor = 1.0;
		while (isdigit(input.peek()) || input.peek() == '.' || input.peek() == '-') {
			if (input.peek() == '-') {
				input.get();
				is_negative = true;
			} else if (input.peek() == '.') {
				input.get();
				is_double = true;
				is_frac_part = true;
				double_result = int_result;
			} else if (is_double) {
				int c = input.get() - '0';
				if (is_frac_part) {
					factor *= 0.1;
					double_result += factor * (double)c;
				} else {
					double_result *= 10;
					double_result += c;
				}
			} else {
				int_result *= 10;
				int_result += input.get() - '0';
			}
		}
		if (is_negative) {
			if (is_double) double_result = -double_result;
			else int_result = -int_result;
		}
		return is_double ? Node(double_result) : Node(int_result);
	}

	Node LoadBool(istream& input) {
		bool result = (input.peek() == 't');
		while (islower(input.peek())) {
			input.get();
		}
		return Node(result);
	}

	Node LoadString(istream& input) {
		string line;
		getline(input, line, '"');
		return Node(move(line));
	}

	Node LoadDict(istream& input) {
		map<string, Node> result;

		for (char c; input >> c && c != '}'; ) {
			if (c == ',') {
				input >> c;
			}

			string key = LoadString(input).AsString();
			input >> c;
			result.emplace(move(key), LoadNode(input));
		}

		return Node(move(result));
	}

	Node LoadNode(istream& input) {
		char c;
		input >> c;

		if (c == '[') {
			return LoadArray(input);
		}
		else if (c == '{') {
			return LoadDict(input);
		}
		else if (c == '"') {
			return LoadString(input);
		}
		else if (c == 't' || c == 'f') {
			input.putback(c);
			return LoadBool(input);
		}
		else {
			input.putback(c);
			return LoadIntOrDouble(input);
		}
	}

	Document Load(istream& input) {
		return Document{ LoadNode(input) };
	}

	void Node::AddToStream(ostream& os) const {
		if (holds_alternative<vector<Node>>(*this)) {
			const auto& nodes = AsArray();
			os << "[\n";
			bool first = true;
			for (const auto& node : nodes) {
				if (!first) {
					os << ",\n";
				}
				first = false;
				os << node;
			}
			os << "\n]";
		}
		else if (holds_alternative<map<string, Node>>(*this)) {
			const auto& nodes_map = AsMap();
			os << "{\n";
			bool first = true;
			for (const auto& [key, node] : nodes_map) {
				if (!first) {
					os << ",\n";
				}
				first = false;
				os << '"' << key << "\": " << node;
			}
			os << "\n}";
		}
		else if (holds_alternative<int>(*this)) {
			os << AsInt();
		}
		else if (holds_alternative<double>(*this)) {
			os << fixed << AsDouble();
		}
		else if (holds_alternative<bool>(*this)) {
			os << (AsBool() ? "true" : "false");
		}
		else {
			os << '"' << AsString() << '"';
		}
	}

}

ostream& operator<<(ostream& os, const Json::Node& node) {
	node.AddToStream(os);
	return os;
}
