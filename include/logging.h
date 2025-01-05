#ifndef UTILS_H
#define UTILS_H

#include <iostream>

using namespace std;

// Macros for logging
#define LOG_INFO(msg) (cout << "INFO: " << msg << '\n')
#define LOG_ERROR(msg) (cerr << "ERROR: " << msg << " (File: " << __FILE__ << ", Line: " << __LINE__ << ")\n")

#endif