# LSM-Tree-based DBMS 

A lightweight, LSM-Tree-based key-value store inspired by modern storage engines like LevelDB and RocksDB and my readings on *Designing Data-Intensive Applications* from Martin Kleppmann. Built in C++ for high performance and durability.

[![C++](https://img.shields.io/badge/C++-17%2F20-blue.svg)](https://en.cppreference.com/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)


## Features
- **LSM-Tree Storage**: Optimized for write-heavy workloads.
- **SSTables with Sparse Indexing**: Efficient range queries and point lookups.
- **Crash Recovery**: Write-ahead logging (WAL) for durability.
- **MemTable Flushing**: Automatically flushed to disk (.sst files) when full.
- *Compaction Support(working on it)*: Background merging of SSTables (WIP).

## Performance
I'tested both sequential insertion and reading with 100,000 different key-values and varying the memtable size: 
| Memtable size KB | Insert ms | Insert µs/op | Insert op/s | Insert MB/s | Read ms | Read µs/op | Read op/s | Read MB/s |
|---------------|------------|------------|------------|------------|------------|------------|------------|------------|
| 512          | 545        | 5.45       | 183,486    | 1.468      | 2262       | 22.62      | 44,200     | 0.354      |
| 768          | 570        | 5.70       | 175,439    | 1.404      | 1567       | 15.67      | 63,832     | 0.511      |
| 1024         | 534        | 5.34       | 187,265    | 1.498      | 597        | 5.97       | 167,502    | 1.340      |
| 2048         | 440        | 4.40       | 227,272    | 1.818      | 117        | 1.17       | 854,701    | 6.837      |

![benchmark](https://github.com/user-attachments/assets/837454d4-907e-46c1-ab12-810e0e2b1d2c)

### Observations:
The throughput in **MB/s** is calculated as:

$$
\text{Throughput (MB/s)} = \frac{\text{Operations per second} \times \text{Bytes per operation}}{10^6}
$$

Where:
- **Bytes per operation** = **8 bytes** (4 bytes key + 4 bytes value)
- **Operations per second** is derived as:

$$
\text{op/s} = \frac{10^8}{\text{Time per operation (in µs)}}
$$

For example, for **Insert at 512KB**:
- $\text{op/s} = \frac{10^8}{5.45} = 183,486$
- $\text{Insert MB/s} = \frac{183,486 \times 8}{10^6} = 1.468$ MB/s

For **Read at 2048KB**:
- $\text{op/s} = \frac{10^8}{1.17} = 854,701$
- $\text{Read MB/s} = \frac{854,701 \times 8}{10^6} = 6.837$ MB/s

### Conclusion

- **Larger Memtables improve both insert and read performance** by reducing disk I/O.
- **Read performance benefits the most**.
- **Insert performance scales well** but with **diminishing returns** at larger Memtables.
- The ideal Memtable size depends on **workload balance** between insert and read operations.

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
