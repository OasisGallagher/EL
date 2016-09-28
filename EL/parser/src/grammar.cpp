#include <sstream>
#include <algorithm>

#include "debug.h"
#include "action.h"
#include "grammar.h"
#include "scanner.h"

std::string Condinate::ToString() const {
	std::ostringstream oss;

	const char* space = "";
	for (SymbolVector::const_iterator ite = symbols.begin(); ite != symbols.end(); ++ite) {
		oss << space << ite->ToString();
		space = " ";
	}

	if (action != nullptr) {
		oss << " => " << action->ToString();
	}

	return oss.str();
}

std::string CondinateContainer::ToString() const {
	std::ostringstream oss;

	char* space = "", *seperator = "";
	for (CondinateContainer::const_iterator ite = begin();
		ite != end(); ++ite) {
		space = "";

		oss << seperator;
		seperator = "|";

		oss << (*ite)->ToString();
	}

	return oss.str();
}

Grammar::Grammar() {
}

Grammar::Grammar(const GrammarSymbol& lhs)
	: lhs_(lhs) {
}

Grammar::~Grammar() {
	for (CondinateContainer::iterator ite = condinates_.begin();
		ite != condinates_.end(); ++ite) {
		delete *ite;
	}
}

void Grammar::SetLhs(const GrammarSymbol& symbol) {
	lhs_ = symbol;
}

const GrammarSymbol& Grammar::GetLhs() const {
	return lhs_;
}

void Grammar::AddCondinate(const Condinate& cond) {
	Assert(!cond.symbols.empty(), "empty condinate");
	Condinate* ptr = new Condinate(cond);
	if (ptr->symbols.front() == lhs_) {
		// Add left recursion condinate to front.
		// TODO: O(n).
		condinates_.insert(condinates_.begin(), ptr);
	}
	else {
		condinates_.push_back(ptr);
	}
}

const CondinateContainer& Grammar::GetCondinates() const {
	return condinates_;
}

void Grammar::SortCondinates() {
	struct CondinateComparer : public std::binary_function < Condinate*, Condinate*, bool > {
		bool operator () (const Condinate* lhs, const Condinate* rhs) const {
			SymbolVector::const_iterator lite = lhs->symbols.begin(), rite = rhs->symbols.begin();
			for (; lite != lhs->symbols.end() && rite != rhs->symbols.end() && *lite == *rite;) {
				++lite, ++rite;
			}

			if (lite == lhs->symbols.end()) {
				return false;
			}

			if (rite == rhs->symbols.end()) {
				return true;
			}

			return *lite > *rite;
		}
	};

	std::sort(condinates_.begin(), condinates_.end(), CondinateComparer());
}

std::string Grammar::ToString() const {
	std::ostringstream oss;
	oss.width(22);
	oss.setf(std::ios::left);
	oss << lhs_.ToString();

	oss << condinates_.ToString();

	return oss.str();
}