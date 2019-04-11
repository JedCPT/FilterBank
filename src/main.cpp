// ================================================================================
// AUTHOR: Jedda Boyle
// Example of how to use the filterbank file class.
// ================================================================================

#include <vector>

#include "header.h"
#include "filterbank.h"




int main(int argc, char const *argv[])
{
    // Open filterbank file.
    FilterBank<uint8_t> file("example.fil");

    // Add field to header.
    FilterbankHeader header = file.header;
    header.AddField("nbeams", FilterbankHeader::DataType::INT);
    header.AddField("ibeams", FilterbankHeader::DataType::INT);

    // Create new filerbank file.
    FilterBank<uint8_t> out_file("out_example.fil", header);
    
    // Read first 100 time bins.
    std::vector<uint8_t> spectra;
    file.GetSpectra(spectra, 0, 100);

    // Read first second of data.
    file.GetTimeSlice(spectra, 0, 1);
    
    // Loop over file in chunks of size 100 time samples.
    while (file.NextSpectra(spectra, 100)) {
        // Copy contents of file into outfile.
        out_file.AppendSpectra(spectra);
    }

    // Return next bin to read index to start of the data.
    file.Seek(0);

    return 0;
}
