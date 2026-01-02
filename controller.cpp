#include "controller.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

TaskController::TaskController(TaskService& service, RateLimiter& limiter)
    : service_(service), limiter_(limiter) {}

void TaskController::register_routes(httplib::Server& svr) {

    auto check_limit = [this](const httplib::Request& req, httplib::Response& res) -> bool {
        std::string ip = req.remote_addr;
        if (!limiter_.allow(ip)) {
            res.status = 429;
            res.set_content(R"({"error":"Rate limit exceeded"})", "application/json");
            return false;
        }
        return true;
    };

    svr.Get("/tasks", [this, &check_limit](const httplib::Request& req, httplib::Response& res) {
        if (!check_limit(req, res)) return;

        auto tasks = service_.get_all();
        nlohmann::json j = tasks;
        res.set_content(j.dump(), "application/json");
        res.status = 200;
    });

    svr.Get(R"(/tasks/(\d+))", [this, &check_limit](const httplib::Request& req, httplib::Response& res) {
        if (!check_limit(req, res)) return;

        int id = std::stoi(req.matches[1]);
        auto task = service_.get_by_id(id);
        if (!task) { res.status = 404; return; }

        nlohmann::json j = *task;
        res.set_content(j.dump(), "application/json");
        res.status = 200;
    });

    svr.Post("/tasks", [this, &check_limit](const httplib::Request& req, httplib::Response& res) {
        if (!check_limit(req, res)) return;

        try {
            auto payload = nlohmann::json::parse(req.body);
            Task t = service_.create(payload);
            nlohmann::json j = t;
            res.set_content(j.dump(), "application/json");
            res.status = 201;
        } catch (...) { res.status = 400; }
    });

    svr.Put(R"(/tasks/(\d+))", [this, &check_limit](const httplib::Request& req, httplib::Response& res) {
        if (!check_limit(req, res)) return;

        int id = std::stoi(req.matches[1]);
        try {
            auto payload = nlohmann::json::parse(req.body);
            auto updated = service_.update_full(id, payload);
            if (!updated) { res.status = 404; return; }

            nlohmann::json j = *updated;
            res.set_content(j.dump(), "application/json");
            res.status = 200;
        } catch (...) { res.status = 400; }
    });

    svr.Patch(R"(/tasks/(\d+))", [this, &check_limit](const httplib::Request& req, httplib::Response& res) {
        if (!check_limit(req, res)) return;

        int id = std::stoi(req.matches[1]);
        try {
            auto payload = nlohmann::json::parse(req.body);
            auto updated = service_.update_partial(id, payload);
            if (!updated) { res.status = 404; return; }

            nlohmann::json j = *updated;
            res.set_content(j.dump(), "application/json");
            res.status = 200;
        } catch (...) { res.status = 400; }
    });

    svr.Delete(R"(/tasks/(\d+))", [this, &check_limit](const httplib::Request& req, httplib::Response& res) {
        if (!check_limit(req, res)) return;

        int id = std::stoi(req.matches[1]);
        if (!service_.remove(id)) { res.status = 404; return; }

        res.status = 200;
    });
}
