#include <gtest/gtest.h>
#include "E:/To-Do-List_API/rate_limiter.h"

TEST(RateLimiterTest, AllowRequests) {
    RateLimiter limiter("127.0.0.1", 6379, 3, 60); 
    std::string key = "127.0.0.1";

    EXPECT_TRUE(limiter.allow(key));
    EXPECT_TRUE(limiter.allow(key));
    EXPECT_TRUE(limiter.allow(key));
    EXPECT_FALSE(limiter.allow(key)); 
}
