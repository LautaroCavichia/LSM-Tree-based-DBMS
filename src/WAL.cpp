#include <fstream>
#include <string>
#include "MemTable.cpp"

using namespace std;

class WAL {
    private:
        string log_path;
        ofstream log_file;
    
    public:
        WAL(string log_path) {
            this->log_path = log_path;
            log_file.open(log_path, ios::app);
        }

        ~WAL() {
            log_file.close();
        }

        void write(string& key, string& value) {
            log_file << key << "," << value << "\n";
            // cout << "Written in WAL: " << key << "," << value << endl;
        }

        void rebuild(MemTable& mem_table) {
            ifstream in(log_path);
            string line, key, value;
            while (getline(in, line)) {
                size_t comma = line.find(',');
                key = line.substr(0, comma);
                value = line.substr(comma + 1);
                mem_table.insert(key, value);
            }
            in.close();
            remove(log_path.c_str());
        }

        void clear() {
            log_file.close();
            ofstream new_log(log_path, ios::trunc);
        }
};