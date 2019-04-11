// ================================================================================
// AUTHOR: Jedda Boyle
// Implementation of filterbank file reader and write.
// ================================================================================

#ifndef FILTERBANK_H
#define FILTERBANK_H

#include <iostream>

#include "math.h"

#include "exceptions.h"
#include "header.h"


template <typename T>
class Filterbank
{
  private:
    // filestreams to the same file.
    std::ifstream in_file;
    std::ofstream out_file;

  public:
    FilterbankHeader header; // File information.

    size_t nbytes;             // How many bytes the data of the filterbank file.
    size_t nbytes_per_timebin; // How many bytes is each time bin (sizeof(T) * nchans_per_time_bin)
    size_t nbins;              // How many samples does the data contain.
    size_t current_bin;        // Used to keep track of the location previously read from file. Equivalent to seekg()

    // ================================================================================
    // Constructors.
    // ================================================================================

    // Create new filterbank file.
    Filterbank(std::string file_name, FilterbankHeader header) : header(header), nbytes(0), nbins(0), current_bin(0)
    {
        header.WriteHeader(file_name);
        in_file.open(file_name, std::ios::binary | std::ios::ate);
        out_file.open(file_name, std::ios::binary | std::ios::ate | std::ios::app);
        nbytes_per_timebin = header.GetField<int>("nchans") * (header.GetField<int>("nbits") / 8);
    }

    // Init filterbank file from filterbank file on disk.
    Filterbank(std::string file_name) : header(file_name), current_bin(0)
    {
        in_file.open(file_name, std::ios::binary | std::ios::ate);
        out_file.open(file_name, std::ios::binary | std::ios::ate | std::ios::app);
        in_file.seekg(0, std::ios_base::end);
        nbytes = ((size_t)in_file.tellg()) - header.nbytes;
        nbytes_per_timebin = header.GetField<int>("nchans") * (header.GetField<int>("nbits") / 8);
        nbins = nbytes / nbytes_per_timebin;
    }

    // Destructor.
    ~Filterbank(void)
    {
        in_file.close();
        out_file.close();
    }

    // ================================================================================
    // Read Methods.
    // ================================================================================

    // Return data between two given sample bins.
    void GetSpectra(std::vector<T> &spectra, const size_t start_bin, const size_t read_nbins)
    {
        if (start_bin + read_nbins > nbins)
        {
            throw InvalidAccessRange();
        }

        spectra.resize(header.GetField<int>("nchans") * read_nbins);
        in_file.seekg(header.nbytes + start_bin * nbytes_per_timebin, std::ios_base::beg);
        in_file.read((char *)spectra.data(), spectra.size() * sizeof(T));
    }

    // Return data between two given times (seconds).
    void GetTimeSlice(std::vector<T> &spectra, const double start_time, const double end_time)
    {
        if (end_time <= start_time)
        {
            throw InvalidAccessRange();
        }
        size_t start_bin = (size_t)round(start_time / header.GetField<double>("tsamp"));
        size_t end_bin = (size_t)round(end_time / header.GetField<double>("tsamp"));

        GetSpectra(spectra, start_bin, end_bin - start_bin);
    }

    // Return the next read_nbins beyind current_bin and update current_bin for future acceses.
    size_t NextSpectra(std::vector<T> &spectra, size_t read_nbins)
    {
        if (current_bin == nbins)
        {
            spectra.clear();
            return 0;
        }
        read_nbins = std::min(read_nbins, nbins - current_bin);
        GetSpectra(spectra, current_bin, read_nbins);
        current_bin += read_nbins;
        return read_nbins;
    }

    // Set the point that NextSpectra function will start reading from.
    void Seek(size_t time_bin) {
        current_bin = time_bin;
    }

    // ================================================================================
    // Write Methods.
    // ================================================================================

    // Append data to the end of the filterbank file.
    void AppendSpectra(std::vector<T> &spectra)
    {
        if (spectra.size() % header.GetField<int>("nchans") != 0)
        {
            throw InvalidSpectraDimensions();
        }
        nbins += spectra.size() / header.GetField<int>("nchans");
        nbytes += spectra.size() * sizeof(T);
        out_file.write((char *)spectra.data(), spectra.size() * sizeof(T));
    }

    // Print the contains of the filterbank header.
    void PrintHeader(void)
    {
        for (const auto &pair : header.header)
        {
            FilterbankHeader::DataType type = header.GetFieldType(pair.first);
            if (type == FilterbankHeader::DataType::STRING)
            {
                std::cout << pair.first << ":" << header.GetField<std::string>(pair.first) << "\n";
            }
            else if (type == FilterbankHeader::DataType::INT)
            {
                std::cout << pair.first << ":" << header.GetField<int>(pair.first) << "\n";
            }
            else if (type == FilterbankHeader::DataType::DOUBLE)
            {
                std::cout << pair.first << ":" << header.GetField<double>(pair.first) << "\n";
            }
        }
    }
};

#endif