// ================================================================================
// AUTHOR: Jedda Boyle
// Contians HeaderField class which is used to hold the data of each field in
// the header file. The header is stored as a hashmap so this class allows for
// the values in the hashmap to be of different types. All the data is stored as a
// char array and converted to int, double or string by the Filterbank header class
// when accessed. 
// ================================================================================

#ifndef HEADER_FIELD_H
#define HEADER_FIELD_H

#include <string>
#include <vector>

class HeaderField
{

  public:
    std::string name;
    std::vector<char> data;

    HeaderField(char *char_data, size_t data_size)
    {
        data.resize(data_size);
        std::memcpy((char *)data.data(), char_data, data_size);
    }

    HeaderField(const HeaderField &rhs) : name(rhs.name), data(rhs.data) {}
};

#endif