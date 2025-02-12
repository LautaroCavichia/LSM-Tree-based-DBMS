#pragma once 

#include <iostream>
#include <string>
#include <map>

using namespace std;

class MemTable {
    private:
        map<string, string> table;
        size_t max_size;
        size_t current_size = 0;

    public:
        MemTable(size_t max_size) {
            this->max_size = max_size;
        }

        ~MemTable() {
            table.clear();
        }

        bool insert(string& key, string& value) {
            auto it = table.find(key);
            if (it != table.end())
                current_size -= it->second.size();
            table[key] = value;
            current_size += value.size() + key.size();
            return current_size > max_size; // signal to flush to disk
        }

        string get(string& key) {
            auto it = table.find(key);
            if (it != table.end())
                return it->second;
            return "";
        }

        void clear() {
            table.clear();
            current_size = 0;
        }

        map<string, string> get_sorted_table() {
            return table;
        }
};