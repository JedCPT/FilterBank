#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>

// template<typename U>
// U ReadFromHeader(std::vector<char>& header_data, const std::string key) {
//
//     // Find the location of the data associated with key in the header.
//     auto loc = search(header_data.begin(), header_data.end(), key.begin(), key.end()) + key.size();
//
//     // Throw error if header field can't be found.
//     U value = -1;
//     if (loc != header_data.end()) {
//         std::memcpy(&value, &*loc, sizeof(U));
//     }
//
//     // U value;
//
//     return value;
// }
// template<std::string>
// std::string ReadFromHeader(std::vector<char>& header_data, const std::string key) {

    // Find the location of the data associated with key in the header.
    // auto loc = search(header_data.begin(), header_data.end(), key.begin(), key.end()) + key.size();

    // std::string value = NULL;
    // if (loc != header_data.end()) {
    //
    //     // std::memcpy(&value, &*loc, sizeof(U));
    // }
    // return value;

// }



struct FilterBankHeader {
        int telescope_id;
        int machine_id;
        int data_type;
        std::string rawdatafile;
        std::string source_name;
        int barycentric;
        int pulsarcentric;
        double az_start;
        double za_start;
        double src_raj;
        double src_dej;
        double tstart;
        double tsamp;
        int nbits;
        int nsamples;
        int nbeams;
        int ibeam;
        double fch1;
        double foff;
        // bool FREQUENCY_START;
        double fchannel;
        // bool FREQUENCY_END;
        int nchans;
        int nifs;
        double refdm;
        double period;
        //npuls;
        int nbins;
};

#define GET_HEADER_FIELD(data_field) { \
    std::string s(#data_field); \
    std::string key = s.substr(s.find(".") + 1, std::string::npos ); \
    ReadFromHeader(key, typeid(data_field).name(), &data_field); \
}


template<typename T>
class FilterBank {
public:

    std::ifstream in_file_stream;

    FilterBankHeader header;

    size_t nbytes_header;
    size_t nbytes_data;
    size_t nbytes_per_spectrum;
    size_t nbins;

    std::vector<char> header_data;

    FilterBank(std::string file_name) {
        in_file_stream.open("fake.fil", std::ios::binary | std::ios::ate);

        nbytes_header = ComputeHeaderSize();

        // Read header data into a vector.
        header_data.resize(nbytes_header);
        in_file_stream.seekg(0, std::ios::beg);
        in_file_stream.read(header_data.data(), nbytes_header);

        // TODO add missing string, bool and q fields.
        GET_HEADER_FIELD(header.telescope_id);
        GET_HEADER_FIELD(header.machine_id);
        GET_HEADER_FIELD(header.data_type);
        GET_HEADER_FIELD(header.barycentric);
        GET_HEADER_FIELD(header.pulsarcentric);
        GET_HEADER_FIELD(header.az_start);
        GET_HEADER_FIELD(header.za_start);
        GET_HEADER_FIELD(header.src_raj);
        GET_HEADER_FIELD(header.src_dej);
        GET_HEADER_FIELD(header.tstart);
        GET_HEADER_FIELD(header.tsamp);
        GET_HEADER_FIELD(header.nbits);
        GET_HEADER_FIELD(header.nsamples);
        GET_HEADER_FIELD(header.nbeams);
        GET_HEADER_FIELD(header.ibeam);
        GET_HEADER_FIELD(header.fch1);
        GET_HEADER_FIELD(header.foff);
        GET_HEADER_FIELD(header.fchannel);
        GET_HEADER_FIELD(header.nchans);
        GET_HEADER_FIELD(header.nifs);
        GET_HEADER_FIELD(header.refdm);
        GET_HEADER_FIELD(header.period);
        GET_HEADER_FIELD(header.nbins);

        // Get size of file.
    	in_file_stream.seekg(0, std::ios_base::end);
    	nbytes_data = ((size_t) in_file_stream.tellg() - nbytes_header);

        nbytes_per_spectrum = header.nchans * (header.nbits / 8);

        nbins = nbytes_data / nbytes_per_spectrum;


    }


    void GetSpectra (std::vector<int>& event, const size_t start_bin, const size_t end_bin) {

    	// TODO Check if we are trying to read invalid bins from filterbank file.

        size_t nbytes_to_read = nbytes_per_spectrum * (end_bin - start_bin);
        size_t event_size = nbytes_to_read / sizeof(event[0]);
        size_t start_byte = nbytes_header + (start_bin * nbytes_per_spectrum);

        event.resize(event_size);
    	in_file_stream.seekg(start_byte, std::ios_base::beg);
        in_file_stream.read((char*) event.data(), nbytes_to_read);

    }

    void ReadFromHeader(const std::string key, const std::string type, void* data) {
        // Find the location of the data associated with key in the header.
        auto loc = search(header_data.begin(), header_data.end(), key.begin(), key.end()) + key.size();

        if (loc == header_data.end()) {
            std::cerr << "Warning: " << key << "not found in header.\n";
            return;
        }
        if (type == "i") {
            std::memcpy(data, &*loc, 4);
        }
        else if (type == "d") {
            std::memcpy(data, &*loc, 8);

        }
    }

    size_t ComputeHeaderSize(void) {
        std::string header_end_identifier("HEADER_END");
        std::string next_string(header_end_identifier.size(), '.');
        size_t pos = 0;
        while (next_string != header_end_identifier) {
            in_file_stream.seekg(pos++, std::ios::beg);
            in_file_stream.read(&next_string[0], next_string.size());
        }
        return in_file_stream.tellg();

    }

};

int main () {
    FilterBank f("fake.fil");

    return 0;
}
