#include "MemTable.cpp"
#include "SSTable.cpp"
#include "WAL.cpp"
#include <vector>
#include <filesystem>
#include <mutex>
#include <atomic>
#include <thread>

using namespace std;

class DB {
    private:
        MemTable mem_table;
        vector<SSTable> sstables;
        WAL wal;
        string db_path;
        size_t sstable_count = 0;
        mutex sstable_mutex;
        atomic<bool> compaction_running{false};
        thread compaction_thread;
    
        void flush_memtable() {
            auto sorted_table = mem_table.get_sorted_table();
            SSTable sstable(db_path + "/sst_" + to_string(sstable_count++) + ".sst" );
            sstable.write(sorted_table);

            lock_guard<mutex> lock(sstable_mutex);
            sstables.push_back(sstable);
            mem_table.clear();
            wal.clear();
        }

        void compactSSTables() {
            lock_guard<mutex> lock(sstable_mutex);
            if (sstables.size() < 2) return;

            // size-tiered strategy, oldest first
            auto candidates = vector<SSTable>(sstables.begin(), sstables.begin() + 2);
            map<string, string> merged; // newest overwrites oldest
            for (auto& sstable : candidates) {
                for (const auto& [key, value] : sstable.read_all()) {
                    merged[key] = value;
                }
            }

            string merged_path = db_path + "/sst_" + to_string(sstable_count++) + ".sst";
            SSTable merged_sstable(merged_path);
            merged_sstable.write(merged);

            // remove old SSTables
            for (auto& sstable : candidates) {
                filesystem::remove(sstable.get_path());
            }

            sstables.erase(sstables.begin(), sstables.begin() + 2);
            sstables.push_back(merged_sstable);
        }

        void compactionLoop() {
            while (true){
                this_thread::sleep_for(chrono::seconds(5));

                if (!compaction_running) break;
                compactSSTables();
            }
        }

        void load_existing_sstables() {
            for (const auto& entry : filesystem::directory_iterator(db_path)) {
                if (entry.path().extension() == ".sst") {
                    try {
                        SSTable sstable(entry.path().string());
                        sstables.push_back(sstable);
                        // cout << "Loading SST: " << entry.path() << endl;
                    } catch (const exception& e) {
                        cerr << " Failed to load SSTable: " << e.what() << endl;
                    }
                }
            }
        }

    public:
        DB(size_t memtable_size, const string& path) 
            : mem_table(memtable_size), wal(path + "/wal.log"), db_path(path) {

                filesystem::create_directory(path);
                load_existing_sstables();
                wal.rebuild(mem_table); // recover from WAL

                compaction_running = true;
                compaction_thread = thread(&DB::compactionLoop, this);
        }

        ~DB() {
            compaction_running = false;
            if (compaction_thread.joinable())
                compaction_thread.join();
                
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