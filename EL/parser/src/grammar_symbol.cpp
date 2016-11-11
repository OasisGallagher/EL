#include <sstream>
#include "token.h"
#include "grammar_symbol.h"

GrammarSymbol GrammarSymbol::null;
GrammarSymbol GrammarSymbol::zero = new Zero();
GrammarSymbol GrammarSymbol::number = new Number();
GrammarSymbol GrammarSymbol::string = new String();
GrammarSymbol GrammarSymbol::newline = new Newline();
GrammarSymbol GrammarSymbol::epsilon = new Epsilon();
GrammarSymbol GrammarSymbol::positive = new Positive();
GrammarSymbol GrammarSymbol::negative = new Negative();
GrammarSymbol GrammarSymbol::identifier = new Identifier();

GrammarSymbol::GrammarSymbol()
	: symbol_(nullptr) {
}

GrammarSymbol::GrammarSymbol(GrammarSymbolBase* symbol)
	: symbol_(symbol) {
}

GrammarSymbol::GrammarSymbol(const GrammarSymbol& other) {
	symbol_ = other.symbol_;
	if (symbol_ != nullptr) {
		symbol_->IncRefCount();
	}
}

GrammarSymbol& GrammarSymbol::operator=(const GrammarSymbol& other) {
	if (other.symbol_ != nullptr) {
		other.symbol_->IncRefCount();
	}

	if (symbol_ != nullptr && symbol_->DecRefCount() == 0) {
		delete symbol_;
	}

	symbol_ = other.symbol_;

	return *this;
}

GrammarSymbol::~GrammarSymbol() {
	if (symbol_ != nullptr && symbol_->DecRefCount() == 0) {
		delete symbol_;
	}
}

GrammarSymbol::operator bool() const {
	return symbol_ != nullptr;
}

bool GrammarSymbol::operator == (const GrammarSymbol& other) const {
	return symbol_ == other.symbol_;
}

bool GrammarSymbol::operator != (const GrammarSymbol& other) const {
	return symbol_ != other.symbol_;
}

bool GrammarSymbol::operator < (const GrammarSymbol& other) const {
	return symbol_ < other.symbol_;
}

bool GrammarSymbol::operator >(const GrammarSymbol& other) const {
	return symbol_ > other.symbol_;
}

GrammarSymbolType GrammarSymbol::SymbolType() const {
	return symbol_->SymbolType();
}

bool GrammarSymbol::Match(const ScannerToken& token) const {
	return symbol_->Match(token);
}

std::string GrammarSymbol::ToString() const {
	return symbol_->ToString();
}

std::string GrammarSymbolContainer::ToString() const {
	std::ostringstream oss;
	const char* seperator = "";
	for (const_iterator ite = begin(); ite != end(); ++ite) {
		oss << seperator;
		seperator = " ";
		oss << ite->second.ToString();
	}

	return oss.str();
}

std::string GrammarSymbolSetTable::ToString() const {
	std::ostringstream oss;

	const char* newline = "";
	for (const_iterator ite = begin(); ite != end(); ++ite) {
		if (ite->first.SymbolType() == GrammarSymbolTerminal) {
			continue;
		}

		oss << newline;
		newline = "\n";

		oss.width(22);
		oss.setf(std::ios::left);

		oss << ite->first.ToString();
		oss << "{";

		const char* seperator = "";
		for (GrammarSymbolSet::const_iterator ite2 = ite->second.begin(); ite2 != ite->second.end(); ++ite2) {
			oss << seperator;
			seperator = " ";
			oss << ite2->ToString();
		}

		oss << "}";
	}

	return oss.str();
}
GrammarSymbol SymbolFactory::Create(const std::string& text) {
	if (Utility::IsTerminal(text)) {
		return new TerminalSymbol(text);
	}

	return new NonterminalSymbol(text);
}

bool TerminalSymbol::Match(const ScannerToken& token) const {
	return text_ == token.text;
}

GrammarSymbolType TerminalSymbol::SymbolType() const {
	return GrammarSymbolTerminal;
}

TerminalSymbol::TerminalSymbol(const std::string& text) : GrammarSymbolBase(text) {

}

bool NonterminalSymbol::Match(const ScannerToken& token) const {
	Assert(false, "match nonterminal symbol");
	return false;
}

GrammarSymbolType NonterminalSymbol::SymbolType() const {
	return GrammarSymbolNonterminal;
}

NonterminalSymbol::NonterminalSymbol(const std::string& text) : GrammarSymbolBase(text) {

}

bool Zero::Match(const ScannerToken& token) const {
	Assert(false, "unable to compare zero with text");
	return false;
}

Zero::Zero() : TerminalSymbol("zero") {

}

bool Epsilon::Match(const ScannerToken& token) const {
	return true;
}

Epsilon::Epsilon() : TerminalSymbol("epsilon") {

}

bool Identifier::Match(const ScannerToken& token) const {
	return token.tokenType == ScannerTokenIdentifier;
}

Identifier::Identifier() : TerminalSymbol("identifier") {

}

bool Positive::Match(const ScannerToken& token) const {
	return token.tokenType == ScannerTokenPositive;
}

Positive::Positive() : TerminalSymbol(POSITIVE_SIGN) {

}

bool Negative::Match(const ScannerToken& token) const {
	return token.tokenType == ScannerTokenNegative;
}

Negative::Negative() : TerminalSymbol(NEGATIVE_SIGN) {

}

bool Number::Match(const ScannerToken& token) const {
	return token.tokenType == ScannerTokenNumber;
}

Number::Number() : TerminalSymbol("number") {

}

bool String::Match(const ScannerToken& token) const {
	return token.tokenType == ScannerTokenString;
}

String::String() : TerminalSymbol("string") {

}

bool Newline::Match(const ScannerToken& token) const {
	return token.tokenType == ScannerTokenNewline;
}

Newline::Newline() : TerminalSymbol("newline") {

}

const std::string& GrammarSymbolBase::ToString() {
	return text_;
}

GrammarSymbolBase::GrammarSymbolBase(const std::string& text) : text_(text) {

}

GrammarSymbolBase::~GrammarSymbolBase() {

}
