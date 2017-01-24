#include <sstream>

#include "debug.h"
#include "grammar.h"
#include "lr_impl.h"
#include "lr_table.h"
#include "grammar_symbol.h"

static const char* actionTexts[] = { "err", "s", "r", "acc" };

bool LRAction::operator == (const LRAction& other) const {
	return actionType == other.actionType && actionParameter == other.actionParameter;
}

bool LRAction::operator != (const LRAction& other) const {
	return actionType != other.actionType || actionParameter != other.actionParameter;
}

std::string LRAction::ToString(const GrammarContainer& grammars) const {
	std::ostringstream oss;
	oss << "(";
	oss << actionTexts[actionType];

	if (actionType == LRActionShift) {
		oss << actionParameter;
	}
	else if (actionType == LRActionReduce) {
		Grammar* g = nullptr;
		const Condinate* cond = grammars.GetTargetCondinate(actionParameter, &g);
		oss << g->GetLhs().ToString() + " : " + cond->ToString();
	}

	oss << ")";

	return oss.str();
}

std::string LRGotoTable::ToString() const {
	const char* seperator = "";
	std::ostringstream oss;
	for (const_iterator ite = begin(); ite != end(); ++ite) {
		oss << seperator;
		seperator = "\n";
		oss << "(";
		oss << ite->first.first;
		oss << ", ";
		oss << ite->first.second.ToString();
		oss << ")";
		oss << " => ";
		oss << ite->second;
	}

	return oss.str();
}

std::string LRActionTable::ToString(const GrammarContainer& grammars) const {
	const char* seperator = "";
	std::ostringstream oss;
	for (const_iterator ite = begin(); ite != end(); ++ite) {
		oss << seperator;
		seperator = "\n";
		oss << "(";
		oss << ite->first.first;
		oss << ", ";
		oss << ite->first.second.ToString();
		oss << ")";
		oss << " => ";
		oss << ite->second.ToString(grammars);
	}

	return oss.str();
}
