#include <gtest/gtest.h>
#include "E:/To-Do-List_API/repository.h"
#include "E:/To-Do-List_API/model.h"

TEST(TaskRepositoryTest, CreateAndGetTask) {
    TaskRepository repo(":memory:"); // in-memory DB
    Task t;
    t.title = "Test";
    t.description = "Test desc";
    t.status = Status::TODO;

    Task created = repo.create(t);
    EXPECT_GT(created.id, 0);

    auto fetched = repo.get_by_id(created.id);
    ASSERT_TRUE(fetched.has_value());
    EXPECT_EQ(fetched->title, "Test");
    EXPECT_EQ(fetched->status, Status::TODO);
}

TEST(TaskRepositoryTest, UpdateTaskFull) {
    TaskRepository repo(":memory:");
    Task t{"", "Task1", "Desc1", Status::TODO};
    Task created = repo.create(t);

    Task new_t{"", "Updated", "Updated desc", Status::DONE};
    auto updated = repo.update_full(created.id, new_t);

    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->title, "Updated");
    EXPECT_EQ(updated->status, Status::DONE);
}

TEST(TaskRepositoryTest, UpdateTaskPartial) {
    TaskRepository repo(":memory:");
    Task t{"", "Task1", "Desc1", Status::TODO};
    Task created = repo.create(t);

    nlohmann::json patch = {{"status","done"}};
    auto updated = repo.update_partial(created.id, patch);

    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->status, Status::DONE);
}

TEST(TaskRepositoryTest, DeleteTask) {
    TaskRepository repo(":memory:");
    Task t{"", "Task1", "Desc1", Status::TODO};
    Task created = repo.create(t);

    bool ok = repo.remove(created.id);
    EXPECT_TRUE(ok);

    auto fetched = repo.get_by_id(created.id);
    EXPECT_FALSE(fetched.has_value());
}
