#include "DB.cpp"
#include "PerformanceChrono.cpp"
#include <fstream>

int main() {
    const string test_dir = "./benchmark_data";
    vector<int> memtable_sizes = {256, 512, 768, 1024, 2048};
    ofstream results("benchmark_results.csv");
    results << "MemtableSizeKB,InsertTimeMs,ReadTimeMs" << endl;
    
    for (int size : memtable_sizes) {
        filesystem::remove_all(test_dir);
        DB db(1024 * size, test_dir);
        
        double insert_time;
        {
            PerformanceChrono timer("Insert 100000 entries");
            for (int i = 0; i < 100000; i++) {
                string key = "key" + to_string(i);
                key = "key" + string(10 - key.size(), '0') + key.substr(3);
                string value = "value" + to_string(i);
                db.insert(key, value);
            }
            insert_time = timer.elapsed();
            cout << "Finished inserting 100,000 entries for Memtable size: " << size << "KB\n";
        }

        // this_thread::sleep_for(20s); // wait for compaction
        
        double read_time;
        {
            PerformanceChrono timer("Read 100000 entries");
            for (int i = 0; i < 100000; i++) {
                string key = "key" + to_string(i);
                key = "key" + string(10 - key.size(), '0') + key.substr(3);
                string value = db.get(key);
            }
            read_time = timer.elapsed();
            cout << "Finished reading 100,000 entries for Memtable size: " << size << "KB\n";
        }
        
        // Save results
        results << size << "," << insert_time << "," << read_time << endl;
        cout << "Finished benchmark for Memtable size: " << size << "KB\n";
    }
    results.close();
    return 0;
}
