#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>

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
    int nbeams; // NOT DOCUMENTED BUT IN USE IN THE SIGPROC CODE
    int ibeam; // NOT DOCUMENTED BUT IN USE IN THE SIGPROC CODE
    double fch1;
    double foff;
    // bool FREQUENCY_START;
    double fchannel;
    // bool FREQUENCY_END;
    int nchans;
    int nifs;
    double refdm;
    double period;

    int nbins;
};


void WriteRawString (std::ofstream& out_file_stream, std::string string){
	const char* c_str = string.c_str();
	size_t len = strlen(c_str);
	out_file_stream.write((char*) &len, sizeof(int));
	out_file_stream.write((char*) c_str, sizeof(char) * len);
}


void WriteString (std::ofstream& out_file_stream, std::string name, std::string value) {
	WriteRawString(out_file_stream, name);
	WriteRawString(out_file_stream, value);
}

template<typename U>
void WriteNumeral (std::ofstream& out_file_stream, std::string name, U value) {
    WriteRawString(out_file_stream, name);
    out_file_stream.write((char*) &value, sizeof(U));
}

void WriteHeader (std::ofstream& out_file_stream, const FilterBankHeader& header) {
	WriteRawString(out_file_stream, "HEADER_START");

    WriteString(out_file_stream, "rawdatafile", header.rawdatafile);
	WriteString(out_file_stream, "source_name", header.source_name);

    WriteNumeral(out_file_stream, "pulsarcentric", header.pulsarcentric);
    WriteNumeral(out_file_stream, "telescope_id", header.telescope_id);
	WriteNumeral(out_file_stream, "barycentric", header.barycentric);
    WriteNumeral(out_file_stream, "machine_id", header.machine_id);
	WriteNumeral(out_file_stream, "data_type", header.data_type);
	WriteNumeral(out_file_stream, "az_start", header.az_start);
	WriteNumeral(out_file_stream, "za_start", header.za_start);
	WriteNumeral(out_file_stream, "src_raj", header.src_raj);
	WriteNumeral(out_file_stream, "src_dej", header.src_dej);
    WriteNumeral(out_file_stream, "nchans", header.nchans);
	WriteNumeral(out_file_stream, "nbeams", header.nbeams);
    WriteNumeral(out_file_stream, "tstart", header.tstart);
	WriteNumeral(out_file_stream, "ibeam", header.ibeam);
    WriteNumeral(out_file_stream, "tsamp", header.tsamp);
	WriteNumeral(out_file_stream, "nbits", header.nbits);
	WriteNumeral(out_file_stream, "nifs", header.nifs);
    WriteNumeral(out_file_stream, "fch1", header.fch1);
	WriteNumeral(out_file_stream, "foff", header.foff);

    WriteRawString(out_file_stream, "HEADER_END");

}

void CreateFilterBankFile (const std::string& file_name, const FilterBankHeader& header) {
    std::ofstream out_file_stream;
	out_file_stream.open(file_name, std::ios::binary | std::ios::out);
    WriteHeader (out_file_stream, header);
    out_file_stream.close();

}

#define GET_HEADER_FIELD(data_field) { \
    std::string s(#data_field); \
    std::string key = s.substr(s.find(".") + 1, std::string::npos ); \
    ReadFromHeader(key, typeid(data_field).name(), &data_field); \
}


template<typename T>
class FilterBank {
public:

    FilterBankHeader header;

    std::ofstream out_file_stream;
    std::ifstream in_file_stream;

    size_t nbytes_header;
    size_t nbytes_data;
    size_t nbytes_per_spectrum;
    size_t nbins;
    size_t nbins_to_stream = 10000;
    size_t current_bin = 0;
    bool verbose;

    std::vector<char> header_data;

    FilterBank(std::string file_name, bool _verbose = false): verbose(_verbose) {

        in_file_stream.open(file_name, std::ios::binary | std::ios::ate);
        out_file_stream.open(file_name, std::ios::binary | std::ios::ate| std::ios::app);

        // Compute the number of bytes in the header by searching for HEADER_END.
        std::string header_end_identifier("HEADER_END");
        std::string next_string(header_end_identifier.size(), '.');
        size_t pos = 0;
        while (next_string != header_end_identifier) {
            in_file_stream.seekg(pos++, std::ios::beg);
            in_file_stream.read(&next_string[0], next_string.size());
        }
        nbytes_header = in_file_stream.tellg();

        // Read header data into a vector.
        header_data.resize(nbytes_header);
        in_file_stream.seekg(0, std::ios::beg);
        in_file_stream.read(header_data.data(), nbytes_header);

        // Extract information from header.
        GET_HEADER_FIELD(header.pulsarcentric);
        GET_HEADER_FIELD(header.telescope_id);
        GET_HEADER_FIELD(header.barycentric);
        GET_HEADER_FIELD(header.source_name);
        GET_HEADER_FIELD(header.rawdatafile);
        GET_HEADER_FIELD(header.machine_id);
        GET_HEADER_FIELD(header.data_type);
        GET_HEADER_FIELD(header.fchannel);
        GET_HEADER_FIELD(header.nsamples);
        GET_HEADER_FIELD(header.az_start);
        GET_HEADER_FIELD(header.za_start);
        GET_HEADER_FIELD(header.src_raj);
        GET_HEADER_FIELD(header.src_dej);
        GET_HEADER_FIELD(header.nbeams);
        GET_HEADER_FIELD(header.tstart);
        GET_HEADER_FIELD(header.nchans);
        GET_HEADER_FIELD(header.period);
        GET_HEADER_FIELD(header.tsamp);
        GET_HEADER_FIELD(header.nbits);
        GET_HEADER_FIELD(header.ibeam);
        GET_HEADER_FIELD(header.refdm);
        GET_HEADER_FIELD(header.nbins);
        GET_HEADER_FIELD(header.fch1);
        GET_HEADER_FIELD(header.foff);
        GET_HEADER_FIELD(header.nifs);

        // Compute meta data about filterbank file.
    	in_file_stream.seekg(0, std::ios_base::end);
    	nbytes_data = ((size_t) in_file_stream.tellg() - nbytes_header);
        nbytes_per_spectrum = header.nchans * (header.nbits / 8);
        nbins = nbytes_data / nbytes_per_spectrum;

        // Check that
        if (sizeof(T) != (header.nbits / 8)) {
            throw std::runtime_error("sizeof(T) != nbits / 8");
        }

    }

    bool ReadInSpectra(std::vector<T>& spectra) {
        if (current_bin > nbins) {
            return true;
        }
        GetSpectra(spectra, current_bin, current_bin + nbins_to_stream);
        current_bin += nbins_to_stream;
        return false;


    }


    void GetSpectra (std::vector<T>& spectra, const size_t start_bin, const size_t end_bin) {

    	// TODO Check if we are trying to read invalid bins from filterbank file.
        size_t nbytes_to_read = nbytes_per_spectrum * (std::min(nbins, end_bin) - start_bin);
        size_t event_size = nbytes_to_read / sizeof(T);
        size_t start_byte = nbytes_header + (start_bin * nbytes_per_spectrum);

        spectra.resize(event_size);
    	in_file_stream.seekg(start_byte, std::ios_base::beg);
        in_file_stream.read((char*) spectra.data(), nbytes_to_read);

    }

    void AppendSpectra(std::vector<T>& spectra) {
        size_t bytes_to_write = spectra.size() * sizeof(T);
        std::cout << spectra.size() << std::endl;
        out_file_stream.write((char*) spectra.data(), bytes_to_write);

    }

    void ReadFromHeader(const std::string key, const std::string type, void* data) {
        // Find the location of the data associated with key in the header.
        auto loc = search(header_data.begin(), header_data.end(), key.begin(), key.end()) + key.size();

        if (loc == header_data.end() + key.size()) {
            if (verbose) {
                std::cerr << "\tWarning: " << key << " not found in header.\n";
            }
            return;
        }
        if (type == "i") {
            std::memcpy(data, &*loc, 4);
        }
        else if (type == "d") {
            std::memcpy(data, &*loc, 8);

        }
        else {
            int str_len;
            std::memcpy(&str_len, &*loc, 4);
            std::memcpy(data, &*(loc + 4), str_len);
        }
    }

};

int main () {
    FilterBank<uint8_t> f("fake.fil");
    std::cout << sizeof(uint8_t) << std::endl;

    CreateFilterBankFile("test.fil", f.header);

    FilterBank<uint8_t> ff("test.fil");

    std::vector<uint8_t> v;
    // size_t current_bin = 0;
    // size_t size = 250368;
    // f.GetSpectra(v, current_bin, current_bin + size);
    while(!f.ReadInSpectra(v)) {
        // std::cout << "here" << std::endl;
        ff.AppendSpectra(v);
    }
        // current_bin += size;
    // ff.AppendSpectra(v);
    // current_bin += size;
    // f.GetSpectra(v, current_bin, current_bin + size);
    // while(f.GetSpectra(v, current_bin, current_bin + size)) {
    //     current_bin += size;
    //     ff.AppendSpectra(v);
    // }







    return 0;
}
