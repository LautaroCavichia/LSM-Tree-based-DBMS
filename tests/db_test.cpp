#include <gtest/gtest.h>
#include "../src/DB.cpp"

TEST(DBTest, RecoveryAfterCrash) {
    string data_dir = "./test_data";
    {
        DB db (1024, data_dir);
        string key = "name";
        string value = "Alice";
        db.insert(key, value);
    }
    // simulate crash
    {
        DB db (1024, data_dir);
        string key = "name";
        string value = db.get(key);
        EXPECT_EQ(value, "Alice");
    }
}

TEST(DBTest, MultiFlushAndReload) {
    const string data_dir =  "./test_data_multi_flush";
    const size_t memtable_size = 100;
    int num_entries = 0;

    filesystem::remove_all(data_dir);

    // Insert enough data to trigger 2 flushes
    {
        DB db (memtable_size, data_dir);

        
        while (num_entries < 20) {
            string key = "key" + to_string(num_entries);
            string value = "value" + to_string(num_entries);
            db.insert(key,value);
            num_entries++;
        }

        int sst_count = 1;
        for (const auto& entry : filesystem::directory_iterator(data_dir)) {
            if (entry.path().extension() == ".sst")
                sst_count++;
        }

        ASSERT_GE(sst_count, 2) << "Expected at least 2 tables";
    }

    // reload DB and verify data exists 
    {
        DB db(memtable_size, data_dir);

        for (int i = 0; i < num_entries; i++) {
            // cout << "num_entries: " << num_entries << " i: " << i << endl;
            string key = "key" + to_string(i);
            string expected_value = "value" + to_string(i);
            string actual_value = db.get(key);

            ASSERT_EQ(actual_value, expected_value) << " Mismatch for key = " << key;

        }
    }

    filesystem::remove_all(data_dir);
}

