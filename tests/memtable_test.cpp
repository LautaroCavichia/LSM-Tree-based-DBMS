#include <gtest/gtest.h>
#include "../src/MemTable.cpp"

TEST(MemTableTest, FlushOnThreshold)
{
    MemTable mem_table(100); // 100 bytes
    string key = "key", value = "value";
    int count = 0;

    /* insert until threshold is crossed considering 
    that .insert returns true if the current size is bigger than the threshold */

    while (!mem_table.insert(key, value))
    {
        if (count > 23) 
            FAIL() << "Threshold not working";
        value += "a";
        count++;
    }
}
