#pragma once
#include <iostream>
#include <mono/jit/jit.h>
#include "Core/Editor/Console.h"

extern "C" {
    void Console_Debug(MonoString* msg);
}