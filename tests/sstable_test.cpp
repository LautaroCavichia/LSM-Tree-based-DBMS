#include <gtest/gtest.h>
#include "../src/SSTable.cpp"

TEST(SSTableTest, IndexSearch) {
    map<string, string> table;

    for (int i = 0; i < 1000; i++) {
        table["key" + to_string(i)] = "value" + to_string(i);
    }

    SSTable sst("test.sst");
    sst.write(table);

    ASSERT_EQ(sst.read("key0"), "value0");
    ASSERT_EQ(sst.read("key500"), "value500");
    ASSERT_EQ(sst.read("key999"), "value999");

    ASSERT_TRUE(sst.read("invalid_key").empty());

}