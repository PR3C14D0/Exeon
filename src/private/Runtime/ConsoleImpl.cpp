#include "Runtime/ConsoleImpl.h"

extern "C" void Console_Debug(MonoString* msg) {
    char* nativeMsg = mono_string_to_utf8(msg);

    Console::GetInstance()->Debug(nativeMsg);
    mono_free(nativeMsg);
}