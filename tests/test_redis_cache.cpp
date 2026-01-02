#include <gtest/gtest.h>
#include "E:/To-Do-List_API/cache.h"

TEST(RedisCacheTest, SetAndGet) {
    RedisCache cache;
    cache.set("test_key", "123", 10);
    auto val = cache.get("test_key");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), "123");

    cache.del("test_key");
    val = cache.get("test_key");
    EXPECT_FALSE(val.has_value());
}
