#include "passwordmanager.hpp"

#define CHAR_LOWEST 33
#define CHAR_HIGHEST 126

int main(int argc, char** argv) {
    std::string action;
    if (argc == 1) {
        std::cout << "Usage: passwordmanager new|read|generate [arguments]" << std::endl <<
                  "  new [filename]:    Enter a password to store in file filename." << std::endl <<
                  "  read [filename]:   Decode password from file filename." << std::endl <<
                  "  generate [length]: Generate a password of given length (default is 8)." << std::endl << std::endl <<
                  "Specify an action: ";
        std::getline(std::cin, action);
    } else
        action = argv[1];
    while (true) {
        if (action == std::string{"new"}) {
            argc > 2 ? newPassword(setFilename(argv[2], true)) :
                       newPassword(setFilename(true));
            return EXIT_SUCCESS;
        } else if (action == std::string{"read"}) {
            argc > 2 ? readPassword(setFilename(argv[2], false)) :
                       readPassword(setFilename(false));
            return EXIT_SUCCESS;
        } else if (action == std::string{"generate"}) {
            argc > 2 ? generatePassword(std::stoi(argv[2])) :
                       generatePassword(8);
            return EXIT_SUCCESS;
        }
        std::cout << "Unknown action. Please try again: ";
        std::getline(std::cin, action);
    }
}

void newPassword(const std::string& filename) {
    std::ofstream passwordFile{filename};
    if (!passwordFile.is_open())
        throw std::runtime_error("Cannot open file to write.");
    std::string masterPassword = acquirePassword("master password");
    std::string passwordToStore = acquirePassword("password to store");
    while (masterPassword.length() < passwordToStore.length())
        masterPassword += masterPassword;
    for (int i = 0; i < passwordToStore.length(); ++i)
        passwordToStore[i] = passwordToStore[i] + masterPassword[i] - CHAR_LOWEST > CHAR_HIGHEST ?
                             passwordToStore[i] + masterPassword[i] - CHAR_HIGHEST - 1 :
                             passwordToStore[i] + masterPassword[i] - CHAR_LOWEST;
    passwordFile << passwordToStore;
}

void readPassword(std::string&& filename) {
    std::ifstream passwordFile{filename};
    if (!passwordFile.is_open())
        throw std::runtime_error("Cannot open file to read.");
    std::string codedPassword{std::istreambuf_iterator<char>{passwordFile}, std::istreambuf_iterator<char>{}};
    if (codedPassword.length() > 30 && askYesNo("The file is quite large for a password. Are you sure you want to continue?"))
        return;
    std::cout << "Master password: ";
    std::string masterPassword;
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
#else
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
    std::getline(std::cin, masterPassword);
#ifdef _WIN32
    SetConsoleMode(hStdin, mode);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    std::cout << std::endl;
    while (masterPassword.length() < codedPassword.length())
        masterPassword += masterPassword;
    for (int i = 0; i < codedPassword.length(); ++i)
        codedPassword[i] = codedPassword[i] - masterPassword[i] + CHAR_LOWEST < CHAR_LOWEST ?
                           codedPassword[i] - masterPassword[i] + CHAR_HIGHEST + 1 :
                           codedPassword[i] - masterPassword[i] + CHAR_LOWEST;
    std::cout << codedPassword;
}

void generatePassword(int n) {
    std::mt19937 engine{std::random_device{}()};
    std::uniform_int_distribution<int> dist{CHAR_LOWEST, CHAR_HIGHEST};
    std::string password;
    do {
        do {
            password = "";
            for (int i = 0; i < n; ++i)
                password += (char) dist(engine);
        } while (numberOfNonAlphanumeric(password) > 2);
        std::cout << "Your new password is: " << password << std::endl;
    } while (!askYesNo("Does this look good?"));
    if (askYesNo("Do you want to store this right away?")) {
        std::string filename = setFilename(true);
        std::ofstream passwordFile{filename};
        if (!passwordFile.is_open())
            throw std::runtime_error("Cannot open file to write.");
        std::string masterPassword = acquirePassword("master password");
        while (masterPassword.length() < password.length())
            masterPassword += masterPassword;
        for (int i = 0; i < password.length(); ++i)
            password[i] += masterPassword[i];
        passwordFile << password;
    }
}

std::string setFilename(bool write) {
    std::cout << "Specify filename: ";
    std::string filename;
    std::getline(std::cin, filename);
    return setFilename(std::move(filename), write);
}

std::string setFilename(std::string&& filename, bool write) {
    if (filename.find(".pwd") != filename.length() - 4 || filename.size() < 4)
        filename += ".pwd";
    std::cout << "Target file: " << filename << std::endl;
    if (write) {
        if (std::ifstream{filename}.is_open()) {
            if (!askYesNo("A file with that name already exists. Do you want to overwrite it?"))
                return setFilename(write);
        }
        return filename;
    } else {
        if (!std::ifstream{filename}.is_open()) {
            std::cout << "The file doesn't exist!" << std::endl;
            return setFilename(write);
        }
        return filename;
    }
}

std::string acquirePassword(const std::string &type) {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
#else
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
#endif
    while (true) {
#ifdef _WIN32
        SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
#else
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
        std::cout << "Set " << type << ": ";
        std::string password;
        std::getline(std::cin, password);
        std::cout << std::endl << "Confirm " << type << ": ";
        std::string passwordCheck;
        std::getline(std::cin, passwordCheck);
#ifdef _WIN32
        SetConsoleMode(hStdin, mode);
#else
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
        std::cout << std::endl;
        if (password == passwordCheck)
            return password;
        else
            std::cout << "The passwords you entered are not the same. Please try again." << std::endl;
    }
}

bool askYesNo(std::string&& message) {
    std::cout << message << " [y/n] ";
    char c;
    std::cin >> c;
    std::cin.ignore();
    return c == 'y';
}

int numberOfNonAlphanumeric(std::string& s) {
    int num = 0;
    for (char c : s)
        num += !std::isalnum(c);
    return num;
}
