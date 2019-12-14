# passwordmanager
A lightweight passwordmanager with a command-line interface.

Tested to work on Windows and Linux (including Android).

Passwords are stored in text files, with each character shifted by the characters of a master password. Characters are expected to be readable C++ `char` types, these are in the \[33, 126\] closed interval according to [this](https://en.cppreference.com/w/cpp/language/ascii) page.

## Usage
<pre>
passwordmanager new|read|generate [arguments]
  new [filename]:    Enter a password to store in file filename.
  read [filename]:   Decode password from file filename.
  generate [length]: Generate a password of given length (default is 8).
</pre>
