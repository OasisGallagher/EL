#if PLATFORM_LINUX

#include "os.h"

void OS::Break(const char* message) {
}

void OS::SetConsoleColor(Color color) {
}

void OS::EnableMemoryLeakCheck() {
}

time_t OS::GetFileLastWriteTime(const char* fileName) {
	return 0;
}

void OS::CopyFile(const char* oldFileName, const char* newFileName) {
}

bool OS::Prompt(const char* message) {
	return true;
}

#endif
