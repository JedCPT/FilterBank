// ================================================================================
// AUTHOR: Jedda Boyle
// Exceptions throw by Filterbank and FilterbankHeader classes.
// ================================================================================

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

// Trying to read an invalid data range.
struct InvalidAccessRange : public std::exception
{
    const char *what() const throw()
    {
        return "Filterbank data access out of range.";
    }
};

// Overwriting a file when writing header to file. 
struct InvalidOutputFile : public std::exception
{
    const char *what() const throw()
    {
        return "Can't write to file that already exists.";
    }
};

// Overwriting a file when writing header to file. 
struct InvalidSpectraDimensions : public std::exception
{
    const char *what() const throw()
    {
        return "Spectra dimension my divide the number of channels per time bin.";
    }
};

// Trying to access a header field that doesn't exist.
class InvalidHeaderAccess : public std::exception
{
    std::string key;

  public:
    InvalidHeaderAccess(const std::string key) : key(key) {}

    const char *what() const throw()
    {
        std::string message("Invalid header key: ");
        message.append(key);
        return message.c_str();
    }
};

// The header field type does not match what is expected.
class InvalidHeaderType : public std::exception
{
    std::string key;

  public:
    InvalidHeaderType(const std::string key) : key(key) {}

    const char *what() const throw()
    {
        std::string message("Invalid header key-type match: ");
        message.append(key);
        return message.c_str();
    }
};

#endif