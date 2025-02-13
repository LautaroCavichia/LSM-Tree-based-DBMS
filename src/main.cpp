#include "DB.cpp"
#include "PerformanceChrono.cpp"

int main() {
    const string test_dir = "./benchmark_data";
    filesystem::remove_all(test_dir);
    DB db(1024 * 10, test_dir); //  2MB memtable size

    // Benchmark inserting 100,000 entries
    {
        PerformanceChrono timer("Insert 1000000 entries");
        for (int i = 0; i < 1000; i++) {
            string key = "key" + to_string(i);
            key = "key" + string(10 - key.size(), '0') + key.substr(3);
            string value = "value" + to_string(i);
            db.insert(key, value);
        }
        timer.stop();
    }

    // Benchmark reading 100,000 entries
    {
        PerformanceChrono timer("Read 1000000 entries");
        for (int i = 0; i < 1000; i++) {
            string key = "key" + to_string(i);
            string value = db.get(key);
        }
        timer.stop();
    }
    return 0;
}