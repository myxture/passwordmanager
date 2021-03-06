#ifndef PASSWORDMANAGER
#define PASSWORDMANAGER

#include <iostream>
#include <fstream>
#include <string>
#include <random>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

void newPassword(const std::string&);
void readPassword(std::string&&);
void generatePassword(int);

std::string setFilename(bool);
std::string setFilename(std::string&&, bool);
std::string acquirePassword(const std::string &type);

bool askYesNo(std::string&&);
int numberOfNonAlphanumeric(std::string&);
void setInputVisibility(bool);

#endif // PASSWORDMANAGER
