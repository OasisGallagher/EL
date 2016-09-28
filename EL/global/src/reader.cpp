#include <algorithm>
#include <functional>

#include "debug.h"
#include "reader.h"
#include "utilities.h"
#include "constants.h"

FileReader::FileReader(const char* path, bool skipBlankline, bool appendNewline)
	: ifs_(path), lineNumber_(0), skipBlankline_(skipBlankline), appendNewline_(appendNewline) {

}

FileReader::~FileReader() {
	ifs_.close();
}

bool FileReader::ReadLine(char* buffer, size_t length, int* lineNumber) {
	for (; ifs_.getline(buffer, length);) {
		++lineNumber_;
		if (!skipBlankline_ || !Utility::IsBlankText(buffer)) {
			break;
		}
	}

	if (appendNewline_ && !ifs_.eof()) {
		size_t ctext = strlen(buffer);
		Assert(ctext + 1 < length, "buffer too small");
		buffer[ctext] = '\n';
		buffer[ctext + 1] = 0;
	}

	if (lineNumber != nullptr) {
		*lineNumber = lineNumber_;
	}

	return !!ifs_;
}

GrammarReader::GrammarReader(const char* file) : fileReader_(file, false, false) {
	ReadGrammars();
}

const GrammarDefContainer& GrammarReader::GetGrammars() const {
	return grammars_;
}

const char* GrammarReader::SplitGrammar(char*& text) {
	text += strspn(text, ":|\t\n ");
	return std::find(text, text + strlen(text), '\t');
}

void GrammarReader::ReadGrammars() {
	char buffer[MAX_LINE_CHARACTERS];
	char* ptr = buffer;

	GrammarDef g;
	std::string text;

	for (; fileReader_.ReadLine(ptr, MAX_LINE_CHARACTERS, nullptr);) {
		const char* pos = nullptr;
		if (Utility::IsBlankText(ptr, &pos)) {
			if (!g.Empty()) {
				grammars_.push_back(g);
				g.Clear();
			}

			continue;
		}

		if (g.Empty()) {
			g.lhs = Utility::Trim(ptr);
			continue;
		}

		const char* tabpos = SplitGrammar(ptr);

		g.productions.push_back(std::make_pair(Utility::Trim(std::string(ptr, tabpos)), Utility::Trim(std::string(tabpos))));
	}

	if (!g.Empty()) {
		grammars_.push_back(g);
	}
}

bool GrammarDef::Empty() const {
	return lhs.empty();
}

void GrammarDef::Clear() {
	lhs.clear(); productions.clear();
}
