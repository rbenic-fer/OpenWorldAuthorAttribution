# OpenWorldAuthorAttribution

Binary code attribution software for 'open world' scenarios - University of Zagreb, Faculty of Electrical Engineering and Computing Software Project 2019/2020

Mentor: Stjepan Groš, doc. dr. sc.

Two different methods for code attribution/comparison are provided: n-gram method (owaa_ngram.cpp) and dictionary method (owaa_dict.cpp).

## Building

Compile one or both of the provided source files (owaa_ngram.cpp, owaa_dict.cpp) with a C++ compiler. At least C++11 compatibility is required.

Example for compiling both files using g++:

**g++ -std=c++11 -o owaa_ngram owaa_ngram.cpp**

**g++ -std=c++11 -o owaa_dict owaa_dict.cpp**

Alternatively, using the provided makefile:

**make** - compile both files, same as the two commands above

**make owaa_ngram** - compile only the file owaa_ngram.cpp

**make owaa_dict** - compile only the file owaa_dict.cpp

**make clean** - delete executable files

The CC= and CFLAGS= command-line variables correspond to the compiler executable file and compiler flags, respectively.

## Configuration

For both owaa_ngram and owaa_dict methods, a formats.txt file with formats of binary instructions is required.

Each format should be provided on its own line. Each character in the instruction format corresponds to a single bit, where characters 0 and 1 denote fixed bits (i.e. bits that are part of the operation code) and other characters denote arguments, where consecutive equal characters are interpreted as a multiple-bit argument.

Example:

**0010aaaabbbbcccc** - a 16-bit instruction with the 4-bit operation code 0010 and three 4-bit arguments

In addition, owaa_dict requires a dict.txt file which contains a dictionary of multi-instruction constructs.

Each dictionary entry consists of instruction formats as described above, where equal characters denote arguments which must be equal. Entries are separated by one or more blank lines.

Example:

**0010aaaabbbbcccc** - same as above

**0100aaaabbbbdddd** - an instruction with the operation code 0100, where the first two arguments must match the previous instruction

## Running

Run the compiled executable file from the command line, providing binary files to be tested as parameters.

Example:

**./owaa_ngram /path/to/file1 /path/to/file2 /path/to/file3 /path/to/file4**
