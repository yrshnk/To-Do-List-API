#include <gtest/gtest.h>
#include "E:/To-Do-List_API/service.h"
#include "E:/To-Do-List_API/repository.h"
#include "E:/To-Do-List_API/cache.h"
#include "E:/To-Do-List_API/message_queue.h"
#include <nlohmann/json.hpp>

class MockCache : public RedisCache {
public:
    MockCache() {}
    std::optional<std::string> get(const std::string&) override { return std::nullopt; }
    void set(const std::string&, const std::string&, int) override {}
    void del(const std::string&) override {}
};

class MockMQ : public MessageQueue {
public:
    void publish(const std::string&, const std::string&) override {}
};

TEST(TaskServiceTest, CreateAndGet) {
    TaskRepository repo(":memory:");
    MockCache cache;
    MockMQ mq;
    TaskService service(repo, cache, mq);

    nlohmann::json payload = {{"title","Test"}, {"description","Desc"}, {"status","todo"}};
    Task t = service.create(payload);

    EXPECT_GT(t.id, 0);
    auto fetched = service.get_by_id(t.id);
    ASSERT_TRUE(fetched.has_value());
    EXPECT_EQ(fetched->title, "Test");
}

TEST(TaskServiceTest, UpdateFullAndPartial) {
    TaskRepository repo(":memory:");
    MockCache cache;
    MockMQ mq;
    TaskService service(repo, cache, mq);

    nlohmann::json payload = {{"title","Test"}, {"description","Desc"}, {"status","todo"}};
    Task t = service.create(payload);

    nlohmann::json full_update = {{"title","Updated"}, {"description","New"}, {"status","done"}};
    auto updated = service.update_full(t.id, full_update);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->status, Status::DONE);

    nlohmann::json partial = {{"status","in_progress"}};
    auto partial_updated = service.update_partial(t.id, partial);
    ASSERT_TRUE(partial_updated.has_value());
    EXPECT_EQ(partial_updated->status, Status::IN_PROGRESS);
}

TEST(TaskServiceTest, RemoveTask) {
    TaskRepository repo(":memory:");
    MockCache cache;
    MockMQ mq;
    TaskService service(repo, cache, mq);

    nlohmann::json payload = {{"title","Test"}};
    Task t = service.create(payload);

    bool ok = service.remove(t.id);
    EXPECT_TRUE(ok);

    auto fetched = service.get_by_id(t.id);
    EXPECT_FALSE(fetched.has_value());
}
