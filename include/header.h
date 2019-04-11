// ================================================================================
// AUTHOR: Jedda Boyle
// A class to handle reading and writing filterbank headers.
// Adheres to the SIGPROC standard.
// http://sigproc.sourceforge.net/sigproc.pdf
// ================================================================================

#ifndef HEADER_H
#define HEADER_H

#include <fstream>
#include <vector>
#include <algorithm>
#include <map>

#include "exceptions.h"
#include "headerfield.h"

class FilterbankHeader
{
  public:
    // Enumerate the valid header data types.
    enum DataType
    {
        STRING,
        DOUBLE,
        INT
    };

    std::map<std::string, HeaderField> header;
    std::map<std::string, DataType> data_types = {
        {"rawdatafile", STRING}, // rawdatafile: the name of the original data file
        {"source_name", STRING}, // source name: the name of the source being observed by the telescope
        {"nchans", INT},         // nchans: number of filterbank channels
        {"nifs", INT},           // ifs: number of seperate IF channels
        {"telescope_id", INT},   // telescope_id : 0=fake data; 1=Arecibo; 2=Ooty
        {"machine_id", INT},     // machine_id: 0=fake; 1=PSPM; 2=WAPP; 3=Ooty
        {"data_type", INT},      // data_type: 1=filterbank; 2=time series
        {"barycentric", INT},    // barycentric: equals 1 if data are barycentric or 0 otherwise
        {"pulsarcentric", INT},  // pulsarcentric: equals 1 if data are pulsarcentric or 0 otherwise
        {"nbits", INT},          // nbits: number of bits per time sample
        {"nsamples", INT},       // nsamples: number of time samples in the data file (rarely used any more)
        {"fch1", DOUBLE},        // fch1: centre frequency (MHz) of first filterbank channel
        {"foff", DOUBLE},        // foff: filterbank channel bandwidth (MHz)
        {"fchannel", DOUBLE},    // fchannel: frequency channel value (MHz)
        {"refdm", DOUBLE},       // refdm: reference dispersion measure (cm−3 pc)
        {"period", DOUBLE},      // period: folding period (s)
        {"az_start", DOUBLE},    // az_start: telescope azimuth at start of scan (degrees)
        {"za_start", DOUBLE},    // za_start: telescope zenith angle at start of scan (degrees)
        {"src_raj", DOUBLE},     // src_raj: right ascension (J2000) of source (hhmmss.s)
        {"src_dej", DOUBLE},     // src_dej: declination (J2000) of source (ddmmss.s)
        {"tstart", DOUBLE},      // tstart: time stamp (MJD) of first sample
        {"tsamp", DOUBLE},       // tsamp: time interval between samples (s)
    };

    size_t nbytes;

    // ================================================================================
    // Constructors
    // ================================================================================

    FilterbankHeader(std::string file_name);

    FilterbankHeader(const FilterbankHeader &header);

    // ================================================================================
    // Public Methods
    // ================================================================================

    void WriteHeader(std::string file_name);

    void AddField(std::string name, DataType type);

    DataType GetFieldType(std::string key);

    // Implemented in header because it is a template function.
    // std::string specialisation of function is in the .cpp file.
    template <typename U>
    U GetField(std::string key)
    {
        if (!data_types.count(key) || !header.count(key))
        {
            throw InvalidHeaderAccess(key);
        }
        else if (data_types[key] == STRING)
        {
            throw InvalidHeaderType(key);
        }
        else
        {
            U to_return;
            std::memcpy((char *)&to_return, (char *)header.at(key).data.data(), sizeof(U));
            return to_return;
        }
        return 0;
    }

  private:
    // ================================================================================
    // Read Methods
    // ================================================================================

    std::string ReadString(std::ifstream &fstream, size_t size);

    int ReadInt(std::ifstream &fstream);

    double ReadDouble(std::ifstream &fstream);

    // ================================================================================
    // Write Methods
    // ================================================================================

    void WriteRawString(std::ofstream &out_file_stream, std::string string);

    void WriteString(std::ofstream &out_file_stream, std::string name, std::string value);

    template <typename U>
    void WriteNumeral(std::ofstream &out_file_stream, std::string name, U value);
};

#endif