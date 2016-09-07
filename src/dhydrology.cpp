// SampleCppLibrary.cpp : Defines the exported functions for the DLL application.

#include <string>
#include <sstream>

#include "dhydrology.h"

// placeholder function, all dll's need a main.. in windows only
#if defined _WIN32
void main()
{
}
#endif

extern "C" {
    EXPORT_API int initialize()
    {
        return 0;
    }


}

