#pragma once
#include <fstream>

class LRTable;
class GrammarContainer;
class GrammarSymbolContainer;
struct SyntaxerSetupParameter;

class Serializer {
public:
	static bool LoadSyntaxer(const char* filePath, SyntaxerSetupParameter& p);
	static bool SaveSyntaxer(const char* filePath, const SyntaxerSetupParameter& p);

private:
	static bool SaveLRTable(std::ofstream& file, const LRTable& table);
	static bool SaveGrammars(std::ofstream& file, const GrammarContainer& cont);
	static bool SaveSymbols(std::ofstream& file, const GrammarSymbolContainer& cont);

	static bool LoadSymbols(std::ifstream& file, GrammarSymbolContainer& cont);
	static bool LoadLRTable(std::ifstream& file, GrammarSymbolContainer& terminalSymbols, GrammarSymbolContainer& nonterminalSymbols, LRTable& table);
	static bool LoadGrammars(std::ifstream& file, GrammarSymbolContainer& terminalSymbols, GrammarSymbolContainer& nonterminalSymbols, GrammarContainer& grammars);

private:
	static bool LoadCondinates(GrammarSymbolContainer& terminalSymbols, GrammarSymbolContainer& nonterminalSymbols, std::ifstream& file, Grammar* grammar);

	static bool SaveLRGotoTable(std::ofstream& file, const LRGotoTable &gotos);
	static bool SaveLRActionTable(std::ofstream& file, const LRActionTable &actionTable);

	static bool LoadLRGotoTable(std::ifstream& file, GrammarSymbolContainer& terminalSymbols, LRTable &table);
	static bool LoadLRActionTable(std::ifstream& file, GrammarSymbolContainer& terminalSymbols, LRTable &table);

private:
	static bool WriteInteger(std::ofstream& file, int x);
	static bool WriteString(std::ofstream& file, const std::string& str);

	static bool ReadInteger(std::ifstream& file, int& integer);
	static bool ReadString(std::ifstream& file, std::string& str);
};
