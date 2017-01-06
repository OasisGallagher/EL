#include <sstream>

#include "lr0.h"
#include "debug.h"
#include "table_printer.h"

LR0::LR0() {
}

LR0::~LR0() {
}

void LR0::Setup(const LRSetupParameter& parameter) {
	p_ = parameter; 
}

bool LR0::CreateLR0Itemsets(LR1ItemsetContainer& itemsets, LR1EdgeTable& edges) {
	LR1Itemset itemset;
	AddLR1Items(itemset, p_.grammars->front()->GetLhs());
	itemset.SetName("0");
	CalculateClosure(itemset);
	itemsets_.insert(itemset);

	for (; CreateLR1ItemsetsOnePass();) {
	}

	for (LR1ItemsetContainer::iterator ite = itemsets_.begin(); ite != itemsets_.end(); ++ite) {
		LR1Itemset& item = (LR1Itemset&)*ite;
		item.RemoveNoncoreItems();
		Assert(item.size() > 0, "invalid itemset");
		itemsets.insert(item);
	}

	edges = edges_;

	return true;
}

bool LR0::CreateLR1ItemsetsOnePass() {
	bool setChanged = false;
	LR1ItemsetContainer cont = itemsets_;
	for (LR1ItemsetContainer::iterator ite = cont.begin(); ite != cont.end(); ++ite) {
		for (GrammarSymbolContainer::iterator ite2 = p_.terminalSymbols->begin(); ite2 != p_.terminalSymbols->end(); ++ite2) {
			LR1Itemset itemset;
			setChanged = GetLR1EdgeTarget(itemset, *ite, ite2->second) || setChanged;
		}

		for (GrammarSymbolContainer::iterator ite2 = p_.nonterminalSymbols->begin(); ite2 != p_.nonterminalSymbols->end(); ++ite2) {
			LR1Itemset itemset;
			setChanged = GetLR1EdgeTarget(itemset, *ite, ite2->second) || setChanged;
		}
	}

	return setChanged;
}

void LR0::AddLR1Items(LR1Itemset& answer, const GrammarSymbol& lhs) {
	int index = 0, gi = 0;
	Grammar* g = p_.grammars->FindGrammar(lhs, &gi);
	const CondinateContainer& conds = g->GetCondinates();

	for (CondinateContainer::const_iterator ci = conds.begin(); ci != conds.end(); ++ci, ++index) {
		const Condinate* tc = *ci;
		SymbolVector::const_iterator ite = tc->symbols.begin();
		int dpos = 0;

		for (; ite != tc->symbols.end(); ++ite, ++dpos) {
			LR1Item newItem(Utility::MakeDword(index, gi), dpos);
			answer.insert(newItem);

			if (*ite == GrammarSymbol::epsilon || !IsNullable(*ite)) {
				break;
			}
		}

		if (ite == tc->symbols.end()) {
			LR1Item newItem(Utility::MakeDword(index, gi), dpos);
			answer.insert(newItem);
		}
	}
}

bool LR0::IsNullable(const GrammarSymbol& symbol) {
	GrammarSymbolSet& firsts = p_.firstSetContainer->at(symbol);
	return firsts.find(GrammarSymbol::epsilon) != firsts.end();
}

void LR0::CalculateClosure(LR1Itemset& answer) {
	for (; CalculateClosureOnePass(answer);) {
	}
}

bool LR0::CalculateClosureOnePass(LR1Itemset& answer) {
	LR1Itemset newItems;
	for (LR1Itemset::iterator isi = answer.begin(); isi != answer.end(); ++isi) {
		const LR1Item& current = *isi;
		const Condinate* cond = p_.grammars->GetTargetCondinate(current.cpos, nullptr);

		if (current.dpos >= (int)cond->symbols.size()) {
			continue;
		}

		const GrammarSymbol& b = cond->symbols[current.dpos];

		if (b.SymbolType() == GrammarSymbolTerminal) {
			continue;
		}

		AddLR1Items(newItems, b);
	}

	bool setChanged = false;
	for (LR1Itemset::const_iterator ite = newItems.begin(); ite != newItems.end(); ++ite) {
		setChanged = answer.insert(*ite) || setChanged;
	}

	return setChanged;
}

bool LR0::GetLR1EdgeTarget(LR1Itemset& answer, const LR1Itemset& src, const GrammarSymbol& symbol) {
	LR1EdgeTable::iterator pos = edges_.find(src, symbol);
	if (pos != edges_.end()) {
		answer = pos->second;
		return false;
	}

	bool newSetCreated = CalculateLR1EdgeTarget(answer, src, symbol);
	if (!answer.empty()) {
		edges_.insert(src, symbol, answer);
		return newSetCreated;
	}

	return false;
}

bool LR0::CalculateLR1EdgeTarget(LR1Itemset& answer, const LR1Itemset& src, const GrammarSymbol& symbol) {
	for (LR1Itemset::const_iterator ite = src.begin(); ite != src.end(); ++ite) {
		const Condinate* cond = p_.grammars->GetTargetCondinate(ite->cpos, nullptr);

		if (cond->symbols.front() == GrammarSymbol::epsilon) {
			continue;
		}

		if (ite->dpos >= (int)cond->symbols.size() || cond->symbols[ite->dpos] != symbol) {
			continue;
		}

		LR1Item item(ite->cpos, ite->dpos + 1);
		answer.insert(item);
	}

	CalculateClosure(answer);

	if (answer.empty()) {
		return false;
	}

	std::pair<LR1ItemsetContainer::iterator, bool> status = itemsets_.insert(answer);
	if (answer.GetName().empty()) {
		if (status.second) {
			answer.SetName(std::to_string(itemsets_.size() - 1));
		}
		else {
			answer.SetName(status.first->GetName());
		}
	}

	return status.second;
}