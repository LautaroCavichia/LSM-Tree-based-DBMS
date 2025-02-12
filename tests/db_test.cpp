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

