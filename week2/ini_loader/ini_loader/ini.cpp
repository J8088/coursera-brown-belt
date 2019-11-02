#include "ini.h"

#include <iostream>
#include <string_view>

namespace Ini {
	Section& Document::AddSection(string name) {
		return sections[name];
	}

	const Section& Document::GetSection(const string& name) const {
		return sections.at(name);
	}

	size_t Document::SectionCount() const {
		return sections.size();
	}

	pair<string_view, string_view> Split(string_view line, char by) {
		size_t pos = line.find(by);
		string_view left = line.substr(0, pos);

		if (pos < line.size() && pos + 1 < line.size()) {
			return { left, line.substr(pos + 1) };
		}
		else {
			return { left, string_view() };
		}
	}

	string_view Unbracket(string_view value) {
		if (!value.empty() && value.front() == '[') {
			value.remove_prefix(1);
		}
		if (!value.empty() && value.back() == ']') {
			value.remove_suffix(1);
		}
		return value;
	}

	Document Load(istream& input) {
		Document doc;

		Section* sect_ptr = nullptr;
		for (string line; getline(input, line); ) {
			if (!line.length()) continue;

			if (line[0] == '[') {
				sect_ptr = &doc.AddSection(string(Unbracket(line)));
			}
			else {
				auto [key, value] = Split(line, '=');
				sect_ptr->insert({ string(key), string(value) });
			}
		}

		return doc;
	}
}
