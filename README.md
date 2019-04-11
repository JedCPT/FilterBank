# filterbank

This repository includes a C++ class for reading and writing Filterbank files. The implementation adheres to the SIGPROC standard (http://sigproc.sourceforge.net/sigproc.pdf) and provides the same functionality as the Python Presto Filterbank class.

An issue with the Filterbank standard is that people don't adhere to it. Often filterbank headers include custom data fields depending on the application domain. For this reason the implementation in this repositoty includes the functionality to change the valid header data-fields so that non-standard Filterbank are also compatible. For example you can add 'nbeams' and  'ibeams' fields, which are commonly used non-standard header data-fields, by simply typing:
```c++
header.AddField("nbeams", FilterbankHeader::DataType::INT);
header.AddField("ibeams", FilterbankHeader::DataType::INT);
```

