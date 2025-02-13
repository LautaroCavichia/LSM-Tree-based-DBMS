#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>

using namespace std;

class SSTable {
private:
    string file_path;
    vector<pair<string, uint64_t>> sparse_index;
    uint32_t index_interval = 64;
    bool index_loaded = false;
    
    // Cache frequently accessed metadata
    uint32_t table_size = 0;
    uint64_t data_section_end = 0;

    void write_string(ofstream& out, const string& s) {
        uint32_t size = s.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));
        out.write(s.c_str(), size);
    }

    string read_string(ifstream& in) {
        uint32_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        string s;
        s.resize(size); // Pre-allocate to avoid reallocation
        in.read(&s[0], size);
        return s;
    }

    // Load index only once and cache it
    void ensure_index_loaded() {
        if (index_loaded) return;
        
        ifstream in(file_path, ios::binary | ios::ate);
        if (!in.is_open()) return;

        // Read index size from the end
        auto file_size = in.tellg();
        in.seekg(-static_cast<int>(sizeof(uint32_t)), ios::end);
        uint32_t index_size;
        in.read(reinterpret_cast<char*>(&index_size), sizeof(index_size));

        // Calculate index position and read index
        auto index_position = static_cast<streamoff>(file_size) - sizeof(uint32_t) - 
                            (index_size * (sizeof(uint64_t) + 256));
        in.seekg(index_position);

        sparse_index.reserve(index_size); // Pre-allocate vector capacity
        for(uint32_t i = 0; i < index_size; i++) {
            string key = read_string(in);
            uint64_t offset;
            in.read(reinterpret_cast<char*>(&offset), sizeof(offset));
            sparse_index.emplace_back(key, offset);
        }

        // Cache data section end position
        data_section_end = index_position;
        index_loaded = true;
    }

    // Optimized binary search
    pair<uint64_t, uint64_t> find_key_range(const string& target_key) {
        auto it = lower_bound(sparse_index.begin(), sparse_index.end(), target_key,
            [](const pair<string, uint64_t>& p, const string& key) {
                return p.first < key;
            });

        uint64_t start_offset;
        uint64_t end_offset;

        if (it == sparse_index.begin()) {
            start_offset = sizeof(uint32_t) * 3; // Skip header
        } else {
            --it;
            start_offset = it->second;
        }

        if (it == sparse_index.end()) {
            end_offset = data_section_end;
        } else {
            end_offset = it->second;
        }

        return {start_offset, end_offset};
    }

public:
    SSTable(string file_path) : file_path(file_path) {}

    void write(const map<string, string>& sorted_table) {
        ofstream out(file_path, ios::binary);
        
        const uint32_t magic = 0x535354;
        table_size = sorted_table.size();
        
        // Write header
        out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        out.write(reinterpret_cast<const char*>(&table_size), sizeof(table_size));
        out.write(reinterpret_cast<const char*>(&index_interval), sizeof(index_interval));

        uint32_t count = 0;
        uint64_t offset = out.tellp();
        sparse_index.clear();
        sparse_index.reserve(table_size / index_interval + 1);

        // Write data section
        for(const auto& [key, value] : sorted_table) {
            if (count % index_interval == 0) {
                sparse_index.emplace_back(key, offset);
            }
            
            write_string(out, key);
            write_string(out, value);
            
            offset = out.tellp();
            count++;
        }

        // Write index section
        const uint32_t index_size = sparse_index.size();
        for(const auto& [key, offset] : sparse_index) {
            write_string(out, key);
            out.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }
        
        out.write(reinterpret_cast<const char*>(&index_size), sizeof(index_size));
        out.close();
        
        // Reset cached state
        index_loaded = false;
    }

    string read(const string& target_key) {
        ensure_index_loaded();
        
        ifstream in(file_path, ios::binary);
        if (!in.is_open()) return "";

        auto [start_offset, end_offset] = find_key_range(target_key);
        in.seekg(start_offset);

        // Read only the necessary section
        while (in.tellg() < end_offset) {
            string key = read_string(in);
            string value = read_string(in);
            
            if (key == target_key) return value;
            if (key > target_key) break;
        }

        return "";
    }

    map<string, string> read_all() {
        map<string, string> tables;
        ifstream in(file_path, ios::binary);
        if (!in.is_open()) return tables;

        uint32_t magic, num_entries;
        in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        in.read(reinterpret_cast<char*>(&num_entries), sizeof(num_entries));
        in.read(reinterpret_cast<char*>(&index_interval), sizeof(index_interval));

        if (magic != 0x535354) return tables;
        
        for (uint32_t i = 0; i < num_entries; i++) {
            string key = read_string(in);
            string value = read_string(in);
            tables.emplace(move(key), move(value));
        }
        
        return tables;
    }

    string get_path() {
        return file_path;
    }
};