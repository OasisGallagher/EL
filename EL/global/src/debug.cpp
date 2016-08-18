#include <string>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include "debug.h"

const int red = 12;
const int white = 7;
const int yellow = 14;

void Debug::Log(const std::string& text) {
	SetConsoleColor(white);
	std::cout << text << "\n";
}

void Debug::LogWarning(const std::string& text) {
	SetConsoleColor(yellow);
	std::cout << "[W] " << text << "\n";
	SetConsoleColor(white);
}

void Debug::LogError(const std::string& text) {
	SetConsoleColor(red);
	std::cout << "[E] " << text << "\n";
	SetConsoleColor(white);
}

void Debug::SetConsoleColor(int color) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, color);
}

void Debug::Break(const std::string& message, const char* file, int line) {
	std::ostringstream oss;
	oss << message << "\n";
	oss << "at " << file << ":" << line;
	Debug::LogError(oss.str());

	abort();
}