// SampleCppLibrary.cpp : Defines the exported functions for the DLL application.

#include <string>
#include <iostream>

#include "dhydrology.h"

// placeholder function, all dll's need a main.. in windows only
#if defined _WIN32
void main()
{
}
#endif

extern "C" {
    EXPORT_API int HYDRO_INITIALIZE()
    {
        std::cout << "Hello world!" << std::endl;

        return 0;
    }


}

