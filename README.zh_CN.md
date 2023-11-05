# HashUp

[![English badge](https://img.shields.io/badge/%E8%8B%B1%E6%96%87-English-blue)](./README.md)
[![简体中文 badge](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-Simplified%20Chinese-blue)](./README.zh_CN.md)

<img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/HashUp/CMAKE_LINUX.yml?label=Build%20Linux" /> <img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/HashUp/CMAKE_WIN.yml?label=Build%20Windows" /> <img src="https://img.shields.io/badge/Platform-Windows&Linux&MacOS-green" /> <img src="https://img.shields.io/github/license/Ace-Radom/HashUp">

HashUp是一个轻量级的，跨平台的命令行文件哈希批量生成和校验工具，支持 `MD5`，`SHA1`，`SHA256` 和 `SHA512` 哈希算法，并也支持多线程加速。

## 编译

HashUp是基于C++17标准编写的，因此需要保证编译器支持C++17。

在Windows下的目标编译器为MSVC，而在Linux下则为GCC。

同时，在编译HashUp前需要在系统内安装OpenSSL库，请确保你已经安装好了并已将其路径加入了环境变量。

### Windows

```bat
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

你应该可以在 `HashUp\build\Release` 下找到 `hashup.exe` （因为在Windows下使用 `cmake --install .` 并没有什么意义：他通常并不会简化任何步骤。）

### Linux

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

如果你想测试HashUp的功能，可以如此调用CMake：

```sh
cmake .. -DCMAKE_BUILD_TYPE=Release -DMAKE_TEST=ON
```

在完成构建后如此启动测试：

```sh
ctest -C RELEASE
```

请注意你需要在你的电脑上安装Python3以运行测试。

如果你的OpenSSL版本大于1.1.1，HashUp会自动启用OpenSSL EVP；反之EVP则会被自动禁用。

如果你不想使用OpenSSL EVP系列函数，你可以如此调用CMake：

```sh
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_OPENSSL_EVP_FUNCS=OFF
```

请注意 `SHA3` 系列哈希函数在禁用OpenSSL EVP时将不可用。

## 使用方法

你只需要将目标哈希表的路径（绝对相对皆可）通过 `-f, --file` 开关传入HashUp。该目标哈希表的父路径会被自动设置为HashUp的工作根路径。

所有在哈希表生成或校验时遇到的相对路径都会基于该工作根路径被转换为绝对路径。

这意味着：

- 如果你正在进行哈希表生成，HashUp会遍历工作根路径下的所有文件，计算他们的哈希值，并将他们相对于工作根路径的相对路径和他们的哈希值一同写入目标哈希表。

- 如果你正在进行哈希校验，你应该确保目标哈希表被放在了你想要校验的所有文件的最上层文件夹下。

### 可用的哈希函数

HashUp支持 `MD5`，`SHA1` 和所有 `SHA2` 系列函数。

当OpenSSL EVP被启用时HashUp也支持 `SHA3` `SHAKE` 系列函数。

| 哈希函数 | 模式Tag |
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

由于OpenSSL的限制，`SHAKE128` 函数的输出长度被固定为128位，`SHAKE256` 则为256位。

### 可用的命令行参数

| 参数 | 功能 | 注意事项 |
| ---- | ---- | -------- |
| `-?, --help` | 显示帮助页面 | |
| `-w, --create` | 进行哈希表生成 | 不能和 `-r` 连用 |
| `-r, --check` | 进行哈希校验 | 不能和 `-w` 连用 |
| `-f, --file` | 传入目标哈希表的路径，其父路径会被设为工作根路径 | 必须参数 |
| `-s, --single` | 启用单文件模式 | 当进行单文件哈希校验时需要通过 `--hash` 开关传入哈希 |
| `--hash HASH` | 传入文件哈希值 | 只在进行单文件哈希校验时可用 |
| `-m, --mode MODE` | 设置哈希函数（`md5`，`sha1`，`sha256`，`sha512`）| 默认为 `md5` |
| `-i, --ignore FILE` | 设置忽略文件路径 | 只在进行哈希表生成时可用 |
| `-j, --thread NUM` | 设置多线程加速的线程数量 | 默认为 `8` |

### 键盘信号

你可以在HashUp在进行哈希计算时按下键盘上的特定键以向HashUp发送键盘信号。

目前支持的键盘信号有：

- 按下 `S` 查看当前完成处理的文件数量、总文件数量、平均处理速度、预计剩余时间和当前正在处理的文件。

- 按下 `P` 暂停当前任务。在暂停过程中按下 `R` 恢复任务。

### 忽略文件

忽略文件允许HashUp在创建哈希列表时忽略某些（或某些类型）的文件或目录。

其格式类似于gitignore，但更简单，提供的功能也更少：

- 每次创建只允许使用一个忽略文件，并且无需将忽略文件放在根工作目录下。

- 用斜杠 `/` 分隔目录。

- 以井号 `#` 开头添加注释。

- 如果忽略规则开头有分隔符，则该规则将被设定为相对于工作根路径的目录级别。否则，该规则也可以匹配工作根路径以下的任何一级目录。

- 与gitignore一样，**只有**在规则末尾有分隔符的情况下，规则才会只匹配目录，否则该忽略规则既可以匹配文件，也可以匹配目录。

- 星号 `*` 匹配除斜线以外的任何内容。问号 `?` 匹配除斜线以外的任何单个字符。

一个例子：

```
# 用井号添加注释

# 忽略所有以 '.exe' 结尾的文件或文件夹
*.exe

# 忽略所有以 'test' 开头的文件或文件夹（比如 'test1' 'test.cpp' 等等）
test*

# 忽略在工作根路径下的，在 'test' 之后还有一个任意字符的文件夹
/test?/
```

### 单文件模式

单文件模式的功能和Windows下的 `certutil -hashfile` 和Linux下的 `md5sum`，`sha1sum` 等等的功能类似。

你可以通过 `-s, --single` 开关启用单文件模式。

如果你想进行单文件哈希校验，你应当通过 `--hash` 开关传入文件哈希值。

**自v1.4.2起**无需再在使用单文件模式时指定 `-r, --check` 或 `-w, --create` 参数。当传入 `--hash` 开关时HashUp会自动进行哈希校验；反之则会进行哈希生成。

### 示例

```sh
hashup -w -f test.md5

hashup -r -f ~/test/test.sha512 -m sha512 -j 16

hashup -s -f testfile -m md5

hashup -s -f testfile -m md5 --hash cdcc3d481ed7319c3fccec101126a75d
```

HashUp允许在一个中划线后同时传入多个短参数。其实类似于 `-sf testfile` 的传入方法也是合法的，但我个人不推荐这么做。

## 配置文件

HashUp的配置文件的文件名应当为 `hashup.ini` 并且被保存在和HashUp的可执行文件同一文件夹下。

默认的配置文件为：

```ini
; This is a config file for HashUp
; Please don't edit sections' and keys' names

[default]
mode=md5
thread=8
```

你可以自行调整配置文件内的默认值。

## 第三方组件

- [tanakh/cmdline](https://github.com/tanakh/cmdline)
- [progschj/ThreadPool](https://github.com/progschj/ThreadPool)
- [pulzed/mINI](https://github.com/pulzed/mINI)

在此对这些项目的开发者致以最诚挚的谢意。

## License

Copyright 2023 Sichen Lyu

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.

You may see the [LICENSE](./LICENSE) here.

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See the License for the specific language governing permissions and limitations under the License.
