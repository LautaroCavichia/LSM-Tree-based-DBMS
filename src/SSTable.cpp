#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>

using namespace std;

class SSTable {
    private:
        string file_path;
        vector<pair<string, uint64_t>> sparse_index; // key, offset
        uint32_t index_interval = 128; 

        void write_string(ofstream& out, const string& s) {
            uint32_t size = s.size();
            out.write(reinterpret_cast<const char*>(&size), sizeof(size));
            out.write(s.c_str(), size);
        }

    public:
        SSTable(string file_path) {
            this->file_path = file_path;
        }

        void write(map<string, string>& sorted_table) {
            ofstream out(file_path, ios::binary);

            // header: magic, table_size, index_interval
            const uint32_t magic = 0x535354; // SST
            const uint32_t table_size = sorted_table.size();
            out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
            out.write(reinterpret_cast<const char*>(&table_size), sizeof(table_size));
            out.write(reinterpret_cast<const char*>(&index_interval), sizeof(index_interval));

            uint32_t count = 0;
            uint64_t offset = out.tellp(); // tracking offset for sparse index

            for(const auto& [key, value] : sorted_table) {
                if (count % index_interval == 0) 
                    sparse_index.emplace_back(key, offset); // emplace_back is more efficient than push_back
                
                write_string(out, key);
                write_string(out, value);

                offset = out.tellp();
                count++;
            }

            const uint32_t index_size = sparse_index.size();
            out.write(reinterpret_cast<const char*>(&index_size), sizeof(index_size));

            for(const auto& [key, offset] : sparse_index) {
                write_string(out, key);
                out.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
            }

            out.close();
        }

        string read(const string& target_key) {
            ifstream in(file_path, ios::binary);
            if (!in.is_open()) return "";

            uint32_t magic, table_size, index_interval;
            in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
            in.read(reinterpret_cast<char*>(&table_size), sizeof(table_size));
            in.read(reinterpret_cast<char*>(&index_interval), sizeof(index_interval));

            if (magic != 0x535354) return ""; 

            in.seekg(-static_cast<int>(sizeof(uint32_t)), ios::end); // move to index_size position
            uint32_t index_size;
            in.read(reinterpret_cast<char*>(&index_size), sizeof(index_size));
            sparse_index.resize(index_size);

            in.seekg(-static_cast<int>(sizeof(uint32_t)) + index_size * (sizeof(uint64_t) + 256), ios::end); // move to the first index entry

            for(uint32_t i = 0; i < index_size; i++) {
                uint32_t key_size;
                in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
                string key(key_size, '\0');
                in.read(&key[0], key_size);
                uint64_t offset;
                in.read(reinterpret_cast<char*>(&offset), sizeof(offset));
                sparse_index[i] = {key, offset};
            }

            // binary search
            auto it = upper_bound(sparse_index.begin(), sparse_index.end(), target_key, 
                [](const string& key, const pair<string, uint64_t>& p) { return key < p.first; });

            uint64_t start_offset = (it == sparse_index.begin()) ? sizeof(magic) + sizeof(table_size) + sizeof(index_interval) : (it - 1)->second;
            uint64_t end_offset = (it == sparse_index.end()) ? static_cast<uint64_t>(in.tellg()) : it->second;

            in.seekg(start_offset);
            string key, value;
            while (in.tellg() < end_offset) {
                uint32_t key_size, value_size;
                in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
                key.resize(key_size);
                in.read(&key[0], key_size);
                in.read(reinterpret_cast<char*>(&value_size), sizeof(value_size));
                value.resize(value_size);
                in.read(&value[0], value_size);
                if (key == target_key) return value;
                else if (key > target_key) break; // keys are sorted, no need to continue
            }
            return "";
        }
};