# HashUp

[![English badge](https://img.shields.io/badge/%E8%8B%B1%E6%96%87-English-blue)](./README.md)
[![简体中文 badge](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-Simplified%20Chinese-blue)](./README.zh_CN.md)

<img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/HashUp/CMAKE_LINUX.yml?label=Build%20Linux" /> <img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/HashUp/CMAKE_WIN.yml?label=Build%20Windows" /> <img src="https://img.shields.io/badge/Platform-Windows&Linux&MacOS-green" /> <img src="https://img.shields.io/github/license/Ace-Radom/HashUp">

HashUp is a lightweight cross-platform command-line file hash generation and checking tool, supports `MD5`, `SHA1`, `SHA256` and `SHA512`, and also supports multi-threaded acceleration.

## Build

HashUp uses C++17 standard, make sure that your compiler supports C++17.

Target compiler on Windows is MSVC, and on Linux is GCC.

Building HashUp also needs OpenSSL library on your computer, make sure you've already got one and added to env path.

### Windows

```bat
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

You should find `hashup.exe` under `HashUp\build\Release` (because using `cmake --install .` doesn't really make sense under Windows, it won't make things any easier).

### Linux / Mac

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

If you want to build tests, do CMake like:

```sh
cmake .. -DCMAKE_BUILD_TYPE=Release -DMAKE_TEST=ON
```

After build run tests with:

```sh
ctest -C RELEASE
```

Be sure that you need python3 on your computer to run tests.

HashUp will automatically enable OpenSSL EVP if your openssl version is higher than 1.1.1. If you are still using openssl version 1.1.0 or lower, EVP will be disabled.

If you don't want to use OpenSSL EVP functions, do CMake like:

```sh
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_OPENSSL_EVP_FUNCS=OFF
```

Be sure that `SHA3` functions are not available without OpenSSL EVP.

## How to use

You only needs to give in the path (whether an absolute one or a relative one) of the target hash list file with `-f, --file` argument. The parent directory of this hash list will be automatically set as the root working directory.

All relative paths appear in whether hash check or generate will be converted to absolute paths based on the root working directory.

That means: 

- when you are doing hash list generation, HashUp will traverse all files under the root working directory, and save their hashes and their relative paths based on the root working directory to the target hash list.

- when you are doing file verification, you should make sure that the target hash list has been put under the root of the directory you want to verify.

### Available Hash Functions

HashUp supports `MD5`, `SHA1` and all `SHA2` functions.

HashUp also supports all `SHA3` and `SHAKE` functions when OpenSSL EVP is enabled.

| Function | Mode Tag |
| :------: | :-: |
| `MD5` | `md5` |
| `SHA1` | `sha1` |
| `SHA2-224` | `sha224` |
| `SHA2-256` | `sha256` |
| `SHA2-384` | `sha384` |
| `SHA2-512` | `sha512` |
| `SHA3-224` | `sha3-224` |
| `SHA3-256` | `sha3-256` |
| `SHA3-384` | `sha3-384` |
| `SHA3-512` | `sha3-512` |
| `SHAKE128` | `shake128` |
| `SHAKE256` | `shake256` |

Because of the limitations of OpenSSL, the output of the `SHAKE128` function is set to 128 bits, and `SHAKE256` 256 bits.

### Available Arguments

| Arguments | Description | Attention |
| --------- | ----------- | --------- |
| `-?, --help` | Show help page | |
| `-w, --create` | Create a hash list for a directory | Cannot be used with `-r` |
| `-r, --check` | Do hash check for a directory | Cannot be used with `-w` |
| `-f, --file FILE` | Give the path of target hash list file, its parent path will be set as the root working directory | Mandatory argument |
| `-s, --single` | Use single file mode | Should be used with `--hash` together when doing single file hash check |
| `--hash HASH` | Give in file hash | Only available by single file hash check |
| `-m, --mode MODE` | Set hash function | Default as `md5` |
| `-i, --ignore FILE` | Set ignore file path | Only available by hash list creation |
| `-j, --thread NUM` | Set the thread-number of multithreading acceleration | Default as `8` |

### Keyboard Signals (Not available under MacOS)

You can send keyboard signals by pressing specific keys during hash calculation.

These keyboard signals are supported now:

- Press `S` to get the number of finished files, total files, average processing speed, expected remaining time and files in process now.
- Press `P` to pause current process, press `R` to resume process.

### Ignore File

An Ignore File allows HashUp to ignore some (or some kinds of) files or directories during hash list creation.

The format is like gitignore, but easier and provides less functions:

- Only one Ignore File for each creation is allowed. You don't need to put the Ignore File under the root working directory.

- The slash `/` is used as the directory separator.

- A line starting with a hash `#` serves as a comment.

- If there is a separator at the beginning of the pattern, then the pattern is relative to the directory level of the root working directory. Otherwise the pattern may also match at any level below root working directory.

- Same as gitignore, a pattern will only match directories **ONLY IF** there is a separator at the end of the pattern, otherwise the pattern can match both files and directories.

- An asterisk `*` matches anything except a slash. A question mark `?` matches any one character except a slash.

One example:

```
# Use '#' to add comments

# ignore all files / directories end with ".exe"
*.exe

# ignore all files / directories begin with "test" (e.g. 'test1', 'test.cpp', etc.)
test*

# ignore the directories under the root working directory which have one character after "test"
/test?/
```

### Single File Mode

Single file mode is just like `curtutil -hashfile` under Windows and `md5sum`, `sha1sum`, etc. under Linux.

You can use `-s, --single` argument to enable it.

If you want to do single file hash check, you should give in the file hash with `--hash` argument.

### Examples

```sh
hashup -w -f test.md5

hashup -r -f ~/test/test.sha512 -m sha512 -j 16

hashup -sw -f testfile -m md5

hashup -sr -f testfile -m md5 --hash cdcc3d481ed7319c3fccec101126a75d
```

HashUp allows short arguments to be grouped after a hyphen. Actually arguments like `-srf testfile` is also okay, but I personally don't recommend it.

## Config File

The config file should be named as `hashup.ini` and put under the same path as HashUp executable.

The default config file is:

```ini
; This is a config file for HashUp
; Please don't edit sections' and keys' names

[default]
mode=md5
thread=8
```

You may change configs as you want.

## Third Party Components

- [tanakh/cmdline](https://github.com/tanakh/cmdline)
- [progschj/ThreadPool](https://github.com/progschj/ThreadPool)
- [pulzed/mINI](https://github.com/pulzed/mINI)

Sincere thanks to the developers of these projects.

## License

Copyright 2023 Sichen Lyu

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.

You may see the [LICENSE](./LICENSE) here.

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See the License for the specific language governing permissions and limitations under the License.