samplefq
========

License
-------
Copyright (C) 2015 Jonas Maaskola  
Provided under GNU General Public License Version 3 or later.  
See the file COPYING provided with this software for details of the license.

Purpose
-------
This program samples sequences without replacement from one or a pair of FASTQ
files. The purpose for doing this is saturation analysis, i.e. to determine
whether the performed sequencing has reached saturation.

How to use
----------
Sequences sampled from the first (or only) FASTQ file are written to the
standard output stream, while sequences sampled from the second FASTQ file are
written to the standard error stream.

Features
--------
Apart from FASTQ files, sampling can also be done from FASTA files. Input files
can be compressed with gzip or bzip2, and are uncompressed on the fly. Two
threads are used during output when pairs of files are processed.

Installation
------------
1. clone the repository
2. move into the directory
3. create a directory where the code should be compiled
4. change to it
5. invoke CMake, specifying the path where the software should be installed
6. compile
7. install

```sh
git clone https://github.com/maaskola/samplefq.git
cd samplefq
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/where/to/install
make
make install
```

Run it
------
Assuming ```read1.fastq``` and ```read2.fastq``` are paired FASTQ files, the
following will sample 100 sequences without replacement:

```sh
samplefq -k 100 -1 read1.fastq -2 read2.fastq > read1_sample100.fastq 2> read2_sample100.fastq
```
