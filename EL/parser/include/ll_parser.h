#pragma once
#include <map>
#include <vector>
#include "grammar.h"
#include "parser.h"

class SyntaxTree;
class TextScanner;
class ParsingTable;
struct ScannerToken;

// LL(1).
class LLParser : public Parser {
public:
	LLParser();
	~LLParser();

public:
	virtual bool ParseFile(SyntaxTree* tree, FileScanner* fileScanner);
	virtual std::string ToString() const;

protected:
	virtual bool ParseGrammars();
	virtual void Clear();

private:
	// 移除简单, 直观的左递归.
	void RemoveLeftRecursion();
	bool RemoveImmidiateLeftRecursion(Grammar* g, GrammarContainer* newGrammars);

	int LongestCommonPrefix(const Condinate* first, const Condinate* second);
	bool CalculateLongestFactor(Grammar* g, int* range, int* length);
	void LeftFactoring();
	bool LeftFactoringOnGrammar(Grammar* g, GrammarContainer* newGrammars);

	void CreateFirstSets();
	bool CreateFirstSetsOnePass();
	void GetFirstSet(GrammarSymbolSet* answer, SymbolVector::iterator first, SymbolVector::iterator last);

	void CreateFollowSets();
	bool CreateFollowSetsOnePass();

	bool BuildParingTable(Grammar* g);
	bool CreateParsingTable();
	void InsertParsingTable(const GrammarSymbol& k1, const GrammarSymbol& k2, Condinate* c);

	void AddAsyncSymbol();
	
private:
	ParsingTable* parsingTable_;

	// Vn的first集合.
	GrammarSymbolSetTable firstSetContainer_;
	GrammarSymbolSetTable followSetContainer_;
};
