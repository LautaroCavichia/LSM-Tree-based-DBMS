#include "DB.cpp"

int main() {
    DB db(1024 * 1024, "./data"); // 1MB memtable size
    uint32_t iterations = 1000;

    // write
    for (int i = 0; i < iterations; i++) {
        string key = "key" + to_string(i);
        string value = "value" + to_string(i);
        db.insert(key, value);
    }

    // read
    for (int i = 0; i < iterations; i++) {
        string key = "key" + to_string(i);
        cout << key << " : " << db.get(key) << endl;
    }

    return 0;

}