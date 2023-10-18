# HashUp

[![English badge](https://img.shields.io/badge/%E8%8B%B1%E6%96%87-English-blue)](./README.md)
[![简体中文 badge](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-Simplified%20Chinese-blue)](./README.zh_CN.md)

<img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/HashUp/CMAKE_LINUX.yml?label=Build%20Linux" /> <img src="https://img.shields.io/github/actions/workflow/status/Ace-Radom/HashUp/CMAKE_WIN.yml?label=Build%20Windows" /> <img src="https://img.shields.io/badge/Platform-Windows&Linux-green" /> <img src="https://img.shields.io/github/license/Ace-Radom/HashUp">

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

## 使用方法

你只需要将目标哈希表的路径（绝对相对皆可）通过 `-f, --file` 开关传入HashUp。该目标哈希表的父路径会被自动设置为HashUp的工作根路径。

所有在哈希表生成或校验时遇到的相对路径都会基于该工作根路径被转换为绝对路径。

这意味着：

- 如果你正在进行哈希表生成，HashUp会遍历工作根路径下的所有文件，计算他们的哈希值，并将他们相对于工作根路径的相对路径和他们的哈希值一同写入目标哈希表。

- 如果你正在进行哈希校验，你应该确保目标哈希表被放在了你想要校验的所有文件的最上层文件夹下。

### 可用的命令行参数

| 参数 | 功能 | 注意事项 |
| ---- | ---- | -------- |
| `-?, --help` | 显示帮助页面 | |
| `-w, --create` | 进行哈希表生成 | 不能和 `-r` 连用 |
| `-r, --check` | 进行哈希校验 | 不能和 `w` 连用 |
| `-f, --file` | 传入目标哈希表的路径，其父路径会被设为工作根路径 | 必须参数 |
| `-m, --mode MODE` | 设置哈希函数（`md5`，`sha1`，`sha256`，`sha512`）| 默认为 `md5` |
| `-j, --thread NUM` | 设置多线程加速的线程数量 | 默认为 `8` |

### 示例

```sh
hashup -w -f test.md5

hashup -r -f ~/test/test.sha512 -m sha512 -j 16
```

## 已知的问题

在Windows下只能用 `-f, --file` 开关传入只包含ASCII字符的路径。

这是由Windows下宽字符编码问题造成的。我只能保证HashUp可以处理所有Windows可以接受的文件名（即便他们包含unicode扩展字符）但我不知道怎么处理这里的命令行传入参数的编码转换问题。如果你知道应该怎么做并愿意告诉我，请提交PR。

## 第三方组件

- [tanakh/cmdline](https://github.com/tanakh/cmdline)
- [progschj/ThreadPool](https://github.com/progschj/ThreadPool)

在此对这些项目的开发者致以最诚挚的谢意。

## License

Copyright 2023 Sichen Lyu

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.

You may see the [LICENSE](./LICENSE) here.

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See the License for the specific language governing permissions and limitations under the License.