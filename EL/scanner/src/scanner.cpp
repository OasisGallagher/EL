#include <cassert>
#include <sstream>
#include <algorithm>
#include "debug.h"
#include "define.h"
#include "scanner.h"
#include "utilities.h"
#include "reader.h"

enum ScannerStateType {
	ScannerStateStart,
	ScannerStateAssign,

	ScannerStateLess,
	ScannerStateGreater,

	ScannerStateMinus,
	ScannerStateDoubleQuotes,

	ScannerStateID,
	ScannerStateNumber,
	ScannerStateDone
};

std::string TokenPosition::ToString() const {
	std::ostringstream oss;
	oss << lineno << ":" << linepos;
	return oss.str();
}

TextScanner::TextScanner() 
	: current_(nullptr), dest_(nullptr) {
	lineBuffer_ = new char[MAX_LINE_CHARACTERS]();
	tokenBuffer_ = new char[MAX_TOKEN_CHARACTERS]();
}

TextScanner::~TextScanner() {
	delete[] lineBuffer_;
	delete[] tokenBuffer_;
}

void TextScanner::SetText(const char* text) {
	size_t length = strlen(text);
	Assert(length > 0 && length < MAX_LINE_CHARACTERS, "invalid line text");

	std::copy(text, text + length, lineBuffer_);
	lineBuffer_[length] = 0;

	start_ = current_ = lineBuffer_;
	dest_ = lineBuffer_ + length + 1;
}

bool TextScanner::GetChar(int* ch) {
	if (current_ == nullptr || current_ == dest_) {
		return false;
	}

	*ch = *current_++;
	return true;
}

void TextScanner::UngetChar() {
	Assert(current_ != nullptr && current_ > lineBuffer_, "unget failed. invalid state");
	--current_;
}

ScannerTokenType TextScanner::GetToken(char* token, int* pos) {
	ScannerStateType state = ScannerStateStart;
	ScannerTokenType tokenType = ScannerTokenError;

	bool savech = false, unget = false;
	int ch = 0;

	int index = 0;

	for (; state != ScannerStateDone;) {
		if (!GetChar(&ch)) {
			if (pos != nullptr) {
				*pos = -1;
			}

			return ScannerTokenEndOfFile;
		}

		savech = true;
		unget = false;

		switch (state) {
		case ScannerStateStart:
			if (ch == '#') {
				return ScannerTokenEndOfFile;
			}

			if (ch == ' ' || ch == '\t' || ch == 0) {
				savech = false;
			}
			else if (isdigit(ch)) {
				state = ScannerStateNumber;
			}
			else if (isalpha(ch) || ch == '_' || ch == '$') {
				state = ScannerStateID;
			}
			else if (ch == '<') {
				state = ScannerStateLess;
			}
			else if (ch == '=') {
				state = ScannerStateAssign;
			}
			else if (ch == '>') {
				state = ScannerStateGreater;
			}
			/*else if (ch == '-') {
				state = ScannerStateMinus;
				}*/
			else if (ch == '"') {
				state = ScannerStateDoubleQuotes;
				savech = false;
			}
			else if (ch == '\n') {
				state = ScannerStateDone;
				tokenType = ScannerTokenNewline;
			}
			else {
				state = ScannerStateDone;
				tokenType = ScannerTokenSign;

				/*
				switch (ch) {
				case '+':
					tokenType = ScannerTokenPlus;
					break;

				case '-':
					tokenType = ScannerTokenMinus;
					break;

				case '*':
					tokenType = ScannerTokenMultiply;
					break;

				case '/':
					tokenType = ScannerTokenDivide;
					break;

				case '|':
					tokenType = ScannerTokenSeperator;
					break;

				case '(':
					tokenType = ScannerTokenLeftParenthesis;
					break;

				case ')':
					tokenType = ScannerTokenRightParenthesis;
					break;

				case ';':
					tokenType = ScannerTokenSemicolon;
					break;

				case ':':
					tokenType = ScannerTokenColon;
					break;
				}*/
			}
			break;

		case ScannerStateAssign:
			state = ScannerStateDone;
			tokenType = ScannerTokenSign;
			if (ch == '=') {
				//tokenType = ScannerTokenEqual;
			}
			else {
				//tokenType = ScannerTokenAssign;
				unget = true;
				savech = false;
			}
			break;
			/*
		case ScannerStateMinus:
			if (isdigit(ch)) {
				state = ScannerStateNumber;
			}
			else if (ch == ' ') {
				savech = false;
			}
			else {
				state = ScannerStateDone;
				tokenType = ScannerTokenSign;
				unget = true;
				savech = false;
			}
			break;
			*/
		case ScannerStateLess:
			state = ScannerStateDone;
			tokenType = ScannerTokenSign;
			if (ch == '=') {
				//tokenType = ScannerTokenLessEqual;
			}
			else {
				//tokenType = ScannerTokenLess;
				unget = true;
				savech = false;
			}
			break;

		case ScannerStateGreater:
			state = ScannerStateDone;
			tokenType = ScannerTokenSign;
			if (ch == '=') {
				//tokenType = ScannerTokenGreaterEqual;
			}
			else {
				//tokenType = ScannerTokenGreater;
				unget = true;
				savech = false;
			}
			break;

		case ScannerStateDoubleQuotes:
			if (ch == '"') {
				tokenType = ScannerTokenString;
				savech = false;
				state = ScannerStateDone;
			}
			break;

		case ScannerStateID:
			if (!isdigit(ch) && !isalpha(ch) && ch != '_' && ch != '$') {
				state = ScannerStateDone;
				tokenType = ScannerTokenIdentifier;
				unget = true;
				savech = false;
			}
			break;

		case ScannerStateNumber:
			if (!isdigit(ch)) {
				state = ScannerStateDone;
				tokenType = ScannerTokenNumber;
				unget = true;
				savech = false;
			}
			break;
		}

		if (unget) {
			UngetChar();
		}

		if (savech) {
			Assert(index < MAX_TOKEN_CHARACTERS, "invalid token.");
			tokenBuffer_[index++] = ch;
		}
	}
	
	strncpy(token, tokenBuffer_, index);
	token[index] = 0;

	if (pos != nullptr) {
		*pos = 1 + current_ - start_ - index;
	}

	return tokenType;
}

FileScanner::FileScanner(const char* path) 
	: reader_(new FileReader(path, true, true)), lineno_(0) {
	/*symbols_ = new Table < Symbol >();
	numberLiterals_ = new Table < NumberLiteral >();
	stringLiterals_ = new Table < StringLiteral >();*/
}

FileScanner::~FileScanner() {
	delete reader_;
	/*
	delete symbols_;
	delete numberLiterals_;
	delete stringLiterals_;
	*/
}

bool FileScanner::GetToken(ScannerToken* token, TokenPosition* pos) {
	char buffer[MAX_TOKEN_CHARACTERS] = { 0 };
	ScannerTokenType tokenType = textScanner_.GetToken(buffer, &pos->linepos);

	char line[MAX_LINE_CHARACTERS];
	for (; tokenType == ScannerTokenEndOfFile; ) {
		if (!reader_->ReadLine(line, MAX_LINE_CHARACTERS, &lineno_)) {
			tokenType = ScannerTokenEndOfFile;
			break;
		}

		if (Utility::IsBlankText(line)) {
			continue;
		}

		textScanner_.SetText(line);
		tokenType = textScanner_.GetToken(buffer, &pos->linepos);
	}

	pos->lineno = lineno_;

	if(tokenType == ScannerTokenError) {
		return false;
	}

	token->tokenType = tokenType;
	strcpy(token->text, buffer);

	return true;
	/*
	token->token = &dummyToken;

	if (tokenType == ScannerTokenNumber) {
		token->token = numberLiterals_->Add(buffer);
	}
	else if (tokenType == ScannerTokenString) {
		token->token = stringLiterals_->Add(buffer);
	}
	*/

	//if (tokenType == ScannerTokenID) {
		//ScannerTokenType reserveTokenType = GetReserveTokenType(buffer);
		//if (reserveTokenType != ScannerTokenError) {
		//	token->tokenType = reserveTokenType;
		//}
	//}
	
	//return true;
}

ScannerTokenType FileScanner::GetReserveTokenType(const char* name) {
// 	if (strcmp(name, "if") == 0) {
// 		return ScannerTokenIf;
// 	}
// 
// 	if (strcmp(name, "else") == 0) {
// 		return ScannerTokenElse;
// 	}
// 
// 	if (strcmp(name, "then") == 0) {
// 		return ScannerTokenThen;
// 	}
// 
// 	if (strcmp(name, "end") == 0) {
// 		return ScannerTokenEnd;
// 	}

	return ScannerTokenError;
}
