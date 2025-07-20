#include "Streams.hpp"
#include <iostream>
#include <ostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

void Streams::main_standardStreams()
{
    //Console Output Stream
    std::cout << "Hello World" << "\n"; // "\n" starts a new line
    std::cout << "Hello " << std::endl << "World " << std::endl << "!" << std::endl;    //std::endl starts a new line and flushes the buffer(is bad for performance)

    const char* test {"test123"};
    std::cout.write(test, std::strlen(test));
    std::cout.put('a');

    std::cout << "My Name is ";
    std::cout.flush();  // "My Name is " is written to the console
    std::cout << "Abeer";
    std::cout.flush();  // "Abeer" is written to the console

    if (std::cout.good())
    {
        std::cout << "All good" << std::endl;
    }
    std::cout.flush();
    if (std::cout.fail())
    {
        std::cerr << "Unable to flush to standard output" << std::endl;
    }

    std::cout.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
    try
    {
        std::cout << "Hello World" << std::endl;
    }
    catch(const std::ios_base::failure& ex)
    {
        std::cerr << "Caught exception: " << ex.what() << ", error code = " << ex.code() << std::endl;
    }
    std::cout.clear();  //To reset error state of a stream

    // Boolean values
    bool myBool { true };
    std::cout << "This is the default: " << myBool << std::endl;
    std::cout << "This should be true: " << std::boolalpha << myBool << std::endl;
    std::cout << "This should be 1: " << std::noboolalpha << myBool << std::endl;

    // Simulate println-style "{:6}" with streams
    int i { 123 };
    //println("This should be ' 123': {:6}", i);
    std::cout << "This should be ' 123': " << std::setw(6) << i << std::endl;

    // Simulate println-style "{:0>6}" with streams
    std::cout << "This should be '000123': " << std::setfill('0') << std::setw(6) << i << std::endl;
    // Fill with *
    std::cout << "This should be '***123': " << std::setfill('*') << std::setw(6) << i << std::endl;
    // Reset fill character
    std::cout << std::setfill(' ');
    // Floating-point values
    double dbl { 1.452 };
    double dbl2 { 5 };
    std::cout << "This should be ' 5': " << std::setw(2) << std::noshowpoint << dbl2 << std::endl;
    std::cout << "This should be @@1.452: " << std::setw(7) << std::setfill('@') << dbl << std::endl;
    // Reset fill character
    std::cout << std::setfill(' ');

    // Instructs cout to start formatting numbers according to your location.
    // Chapter 21 explains the details of the imbue() call and the locale object.
    std::cout.imbue(std::locale { "" });
    // Format numbers according to your location
    std::cout << "This is 1234567 formatted according to your location: " << 1234567 << std::endl;
    // Monetary value. What exactly a monetary value means depends on your
    // location. For example, in the USA, a monetary value of 120000 means 120000
    // dollar cents, which is 1200.00 dollars.
    std::cout << "This should be a monetary value of 120000, " << "formatted according to your location: " << std::put_money("120000") << std::endl;

    // Date and time
    time_t t_t { time(nullptr) }; // Get current system time.
    tm t { *localtime(&t_t) }; // Convert to local time.
    std::cout << "This should be the current date and time " << "formatted according to your location: " << std::put_time(&t, "%c") << std::endl;
    // Quoted string
    std::cout << "This should be: \"Quoted string with \\\"embedded quotes\\\".\": " << std::quoted("Quoted string with \"embedded quotes\".") << std::endl;
}

std::string readName(std::istream& stream) {
    std::string name;
    while (stream)
    {
        int next {stream.get()};
        if (!stream || next !=std::char_traits<char>::eof())
        {
            break;
        }
        name += static_cast<char>(next);
    }
    return name;
}

void Streams::main_inputStream()
{
    std::string input;
    std::cin >> input;
    std::cout << input << std::endl;

    while (std::cin >> input)
    {
        if (input == "stop")
        {
            break;
        }
        
        std::cout << input << std::endl;
    }
    std::cout << "input" << std::endl;
    std::string name { readName(std::cin) };
}

void Streams::main_ostringstream()
{
    std::cout << "Enter Tokens. (Ctrl Z) on Windows to end" << std::endl;
    std::ostringstream outStream;
    bool firstLoop { true };
    while (std::cin)
    {
        std::string nextToken;
        std::cout << "Next Token: ";
        std::cin >> nextToken;

        if (!std::cin || nextToken == "done")
        {
            break;
        }
        if (!firstLoop)
        {
            outStream << ", ";
        }
        outStream << '"' << nextToken << '"';
        firstLoop = false;
    }
    std::cout << "Result: " << outStream.str();
}

void Streams::main_istringstream()
{
    std::string muffin { "\"Raspberry Muffin\" 12 true" };
    
    std::string description;
    int size;
    bool hasChips;

    std::istringstream sstream {muffin};
    sstream >> description >> size >> hasChips;
    std::cout << description << " " << size << " " << hasChips << std::endl;
}

void Streams::main_fileOutStream()
{
    std::ofstream outFile { "test.txt", std::ios_base::trunc};
    if (!outFile.good())
    {
        std::cerr << "Error opening file for output";
        return;
    }
    outFile << "There were lines added to this file" << std::endl;
    outFile << "Another line is added2" << std::endl;
}
