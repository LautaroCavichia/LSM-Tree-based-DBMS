# LSM-Tree-based DBMS 

A lightweight, LSM-Tree-based key-value store inspired by modern storage engines like LevelDB and RocksDB and my reading on *Designing Data-Intensive Applications* from Martin Kleppmann. Built in C++ for high performance and durability.

[![C++](https://img.shields.io/badge/C++-17%2F20-blue.svg)](https://en.cppreference.com/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)


## Features
- **LSM-Tree Storage**: Optimized for write-heavy workloads.
- **SSTables with Sparse Indexing**: Efficient range queries and point lookups.
- **Crash Recovery**: Write-ahead logging (WAL) for durability.
- **MemTable Flushing**: Automatically flushed to disk (.sst files) when full.
- *Compaction Support(working on it)*: Background merging of SSTables (WIP).

## Getting Started
### Prerequisites
- C++20 compiler (GCC/Clang)
- CMake 3.15+
- Google Test (for testing)

### Build
```bash
git clone https://github.com/LautaroCavichia/LSM-Tree-based-DBMS
cd LSM-Tree-based-DBMS
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```


### Usage Example
```cpp
int main() {
    DB db(1024 * 1024, "./data"); // 1MB MemTable
    db.insert("name", "Alice");
    std::cout << db.get("name") << std::endl;
    return 0;
}
```
