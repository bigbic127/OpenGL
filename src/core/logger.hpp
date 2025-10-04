#pragma once
#include <iostream>

class Logger
{
    public:
        static void ErrorMessage(const char* message)
        {
            std::cerr << "Error: " << message << std::endl;
        }
        static void WindowErrorCallback(int error_code, const char* description)
        {
            std::cerr << "Error Code: " << error_code << ", Description: " << description << std::endl;
        }
};