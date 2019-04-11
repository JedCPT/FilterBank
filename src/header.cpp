// ================================================================================
// AUTHOR: Jedda Boyle
// Implementation of FilterbankHeader
// ================================================================================

#include "header.h"

// ================================================================================
// Constructors.
// ================================================================================

FilterbankHeader::FilterbankHeader(std::string file_name)
{

    std::ifstream fstream;
    fstream.open(file_name, std::ios::binary | std::ios::ate);
    fstream.seekg(0, std::ios::beg);

    size_t str_size = ReadInt(fstream);
    std::string current = ReadString(fstream, str_size);

    while (true)
    {
        str_size = ReadInt(fstream);
        current = ReadString(fstream, str_size);
        if (current == "HEADER_END")
        {
            break;
        }
        else if (!data_types.count(current))
        {
            throw InvalidHeaderAccess(current);
        }
        else if (data_types[current] == STRING)
        {
            str_size = ReadInt(fstream);
            std::string val = ReadString(fstream, str_size);
            header.emplace(current, HeaderField((char *)val.data(), val.size()));
        }
        else if (data_types[current] == INT)
        {
            int val = ReadInt(fstream);
            header.emplace(current, HeaderField((char *)&val, sizeof(int)));
        }
        else if (data_types[current] == DOUBLE)
        {
            double val = ReadDouble(fstream);
            header.emplace(current, HeaderField((char *)&val, sizeof(double)));
        }
    }

    nbytes = fstream.tellg();
    fstream.close();
}

FilterbankHeader::FilterbankHeader(const FilterbankHeader &header) : nbytes(header.nbytes)
{
    data_types.clear();
    data_types = header.data_types;
    for (const auto &pair : header.header)
    {
        this->header.emplace(pair.first, pair.second);
    }
}

// ================================================================================
// Public Methods
// ================================================================================

template <>
std::string FilterbankHeader::GetField<std::string>(std::string key)
{
    if (!data_types.count(key) || !header.count(key))
    {
        throw InvalidHeaderAccess(key);
    }
    else if (data_types[key] == INT || data_types[key] == DOUBLE)
    {
        throw InvalidHeaderType(key);
    }
    else
    {
        std::string to_return(header.at(key).data.begin(), header.at(key).data.end());
        return to_return;
    }
    return "";
}

void FilterbankHeader::WriteHeader(std::string file_name)
{
    std::ifstream test_stream(file_name);
    if (test_stream.good())
    {
        test_stream.close();
        throw InvalidOutputFile();
        return;
    }
    test_stream.close();

    std::ofstream fstream;
    fstream.open(file_name, std::ios::binary | std::ios::ate | std::ios::app);
    WriteRawString(fstream, "HEADER_START");
    for (auto const &pair : header)
    {
        if (!data_types.count(pair.first))
        {
            throw InvalidHeaderAccess(pair.first);
        }
        else if (data_types[pair.first] == STRING)
        {
            std::string to_write = GetField<std::string>(pair.first);
            WriteString(fstream, pair.first, to_write);
        }
        else if (data_types[pair.first] == INT)
        {
            int to_write = GetField<int>(pair.first);
            WriteNumeral<int>(fstream, pair.first, to_write);
        }
        else if (data_types[pair.first] == DOUBLE)
        {
            double to_write = GetField<int>(pair.first);
            WriteNumeral<double>(fstream, pair.first, to_write);
        }
    }

    WriteRawString(fstream, "HEADER_END");
}

void FilterbankHeader::AddField(std::string name, FilterbankHeader::DataType type)
{
    data_types.emplace(name, type);
}

FilterbankHeader::DataType FilterbankHeader::GetFieldType(std::string key)
{
    if (!data_types.count(key))
    {
        throw InvalidHeaderAccess(key);
    }
    return data_types.at(key);
}

// ================================================================================
// Read Methods
// ================================================================================

std::string FilterbankHeader::ReadString(std::ifstream &fstream, size_t size)
{
    std::string to_return;
    to_return.resize(size);
    fstream.read((char *)to_return.data(), size);
    return to_return;
}

int FilterbankHeader::ReadInt(std::ifstream &fstream)
{
    int to_return;
    fstream.read((char *)&to_return, 4);
    return to_return;
}

double FilterbankHeader::ReadDouble(std::ifstream &fstream)
{
    double to_return;
    fstream.read((char *)&to_return, 8);
    return to_return;
}

// ================================================================================
// Write Methods
// ================================================================================

void FilterbankHeader::WriteRawString(std::ofstream &out_file_stream, std::string string)
{
    const char *c_str = string.c_str();
    size_t len = strlen(c_str);
    out_file_stream.write((char *)&len, sizeof(int));
    out_file_stream.write((char *)c_str, sizeof(char) * len);
}

void FilterbankHeader::WriteString(std::ofstream &out_file_stream, std::string name, std::string value)
{
    WriteRawString(out_file_stream, name);
    WriteRawString(out_file_stream, value);
}

template <typename U>
void FilterbankHeader::WriteNumeral(std::ofstream &out_file_stream, std::string name, U value)
{
    WriteRawString(out_file_stream, name);
    out_file_stream.write((char *)&value, sizeof(U));
}
