#include <gtest/gtest.h>
#include "E:/To-Do-List_API/message_queue.h"

TEST(MessageQueueTest, Publish) {
    MessageQueue mq;
    EXPECT_NO_THROW(mq.publish("test_channel", "hello world"));
}
