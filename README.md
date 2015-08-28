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

Installation
------------
1. create a directory where the code should be compiled
2. change to it
3. invoke CMake (assuming ```$SAMPLEFQ_ROOT_DIR``` is the path of this repository)
4. compile
5. install

```sh
mkdir build
cd build
cmake $SAMPLEFQ_ROOT_DIR -DCMAKE_INSTALL_PREFIX=/where/to/install
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
