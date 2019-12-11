#ifndef PASSWORDMANAGER
#define PASSWORDMANAGER

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <array>

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
bool isAlphanumeric(char);
int numberOfNonAlphanumeric(std::string&);

#endif // PASSWORDMANAGER
