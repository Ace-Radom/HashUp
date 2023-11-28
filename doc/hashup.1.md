% HASHUP(1) HashUp 1.5.4 | HashUp Manual
% Ace-Radom <https://github.com/Ace-Radom>
% 2023.11.27

# NAME
HashUp \- light-weight command-line file hash generation and checking tool

# SYNOPSIS
**hashup** [ **-?**  | **--help** ] [ **-v** | **--version** ]

**hashup** [ [ **-w** | **--create** [ **--overlay** ] ] [ **-r** | **--check** ] ] \
\ \ \ \ \ \ \ [ **-f** | **--file** *hash-list-file* ] [ **-m** | **--mode** *hash-mode* ] \
\ \ \ \ \ \ \ [ **-i** | **--ignore** *ignore-file* ] [ **-j** | **--thread** *thread-num* ]

**hashup** [ **-s** | **--single** [ **--hash** *hash* ] ]

# DESCRIPTION
**HashUp** is a lightweight cross-platform command-line file hash generation and checking tool written in C++, supports multiple hash functions, and multi-threaded acceleration.

**HashUp** can not only do high-performance batch file hash generation and validation for files under a same directory, but also calculate the hash of a single file, just like how **md5sum** and other hash calculation programs work.

**HashUp** also provides a simple ignore file format (like gitignore) to allow the users to ignore some kinds of files under the working directory.

# OPTIONS

**\-?**, **\--help**
: Show help page

**\-v**, **\--version**
: Show HashUp version

**\-w**, **\--create**
: Create a hash list for a directory (Cannot be used with **\-r**)

**\-r**, **\--check**
: Do hash check for a directory based on a hash list (Cannot be used with **\-w**)

**\-f**, **\--file** *hash-list-file*
: The path of the target hash list, its parent path will be set as the root working directory

**\-s**, **\--single**
: Use single file mode

**\--hash** *hash*
: Hash for the given file (Only work under single file mode)

**\-m**, **\--mode** *hash-mode*
: Set hash function (See **AVAILABLE HASH FUNCTIONS** section)

**\-i**, **\--ignore** *ignore_file*
: Ignore file path (Only work with hash list creation)

**\-j**, **\--thread** *thread-num*
: The thread-number of multithreading acceleration (Default as **8**)

# SUPPORTED HASH FUNCTIONS

**MD5**
: Mode Tag: *md5*

**SHA1**
: Mode Tag: *sha1*

**SHA2 Series**
: Includes: **SHA2-224**, **SHA2-256**, **SHA2-384**, **SHA2-512**\
\ \ \ \ \ \ \ Mode Tag: *sha224*, *sha256*, *sha384*, *sha512*

**SHA3 Series**
: Includes: **SHA3-224**, **SHA3-256**, **SHA3-384**, **SHA3-512**, **SHAKE128**, **SHAKE256**\
\ \ \ \ \ \ \ Mode Tag: *sha3-224*, *sha3-256*, *sha3-384*, *sha3-512*, *shake128*, *shake256*

**SHA3 Series** is only supported when *OpenSSL EVP* is enabled in build config.

Because of the limitations of *OpenSSL*, the output of **SHAKE128** function is set to 128 bits, and **SHAKE256** 256 bits.

# EXAMPLES
**hashup -w -f test.md5**
: Create a hash-list for current directory with default hash function and write the output to *test.md5*.

**hashup -r -f ~/test/test.sha512 -m sha512 -j 16**
: Read the hash-list-file *~/test/test.sha512* and verify all files in the hash-list with **SHA512** function in 16 threads.

**hashup -s -f testfile**
: Calculate the file hash of *testfile* with default hash function.

**hashup -s -f testfile --hash cdcc3d481ed7319c3fccec101126a75d**
: Calculate the file hash of *testfile* with default hash function, see if it is *cdcc3d481ed7319c3fccec101126a75d*.

# SEE ALSO
**hashup-cfg**(5), **hashup-ignore**(5), **renalog**(5)

# COPYRIGHT

Copyright 2023 Sichen Lyu

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See the License for the specific language governing permissions and limitations under the License.

