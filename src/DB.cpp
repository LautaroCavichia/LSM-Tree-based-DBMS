#include "MemTable.cpp"
#include "SSTable.cpp"
#include "WAL.cpp"
#include <vector>
#include <filesystem>

using namespace std;

class DB {
    private:
        MemTable mem_table;
        vector<SSTable> sstables;
        WAL wal;
        string db_path;
        size_t sstable_count = 0;
    
        void flush_memtable() {
            auto sorted_table = mem_table.get_sorted_table();
            SSTable sstable(db_path + "/sst_" + to_string(sstable_count++));
            sstable.write(sorted_table);
            sstables.push_back(sstable);
            mem_table.clear();
            wal.clear();
        }

    public:
        DB(size_t memtable_size, const string& path) 
            : mem_table(memtable_size), wal(path + "/wal.log"), db_path(path) {

                filesystem::create_directory(path);
                wal.rebuild(mem_table); // recover from WAL

                // load existing SSTables from disk
                for (const auto& entry : filesystem::directory_iterator(path)) {
                    if (entry.path().extension() == ".sst") {
                        SSTable sstable(entry.path());
                        sstables.push_back(sstable);
                    }
                }
        }

        ~DB() {
            flush_memtable();
        }

        void insert(string& key, string& value) {
            wal.write(key, value);
            if (mem_table.insert(key, value)) 
                flush_memtable();
        }

        string get(string& key) {
            string value = mem_table.get(key);
            if (!value.empty())
                return value;
            
            // search in SSTables
            for (auto it = sstables.rbegin(); it != sstables.rend(); ++it) {
                value = it->read(key);
                if (!value.empty())
                    return value;
            }

            return "";
        }
};