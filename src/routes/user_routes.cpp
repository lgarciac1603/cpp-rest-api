#define _WIN32_WINNT 0x0A00

#include "user_routes.h"
#include "../repositories/user_repository.h"


static std::string to_json(const User& u) {
    std::ostringstream s;
    s << "{\"id\":" << u.id
        << ",\"username\":\"" << u.username
        << "\",\"email\":\"" << u.email << "\"}";
    return s.str();
}

void register_user_routes(httplib::Server& api, DBConnection& db) {
    api.Get("/users", [&](const httplib::Request&, httplib::Response& res) {
        UserRepository repo(db.get());
        auto users = repo.findAll();

        std::ostringstream s; s << "[";
        for (size_t i = 0; i < users.size(); ++i) {
            s << to_json(users[i]);
            if (i + 1 < users.size()) s << ",";
        }
        s << "]";
        res.set_content(s.str(), "application/json");
    });

    api.Get(R"(/users/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        UserRepository repo(db.get());
        int id = std::stoi(req.matches[1]);
        auto u = repo.findById(id);
        if (!u) { res.status = 404; return; }
        res.set_content(to_json(*u), "application/json");
    });

    api.Post("/users", [&](const httplib::Request& req, httplib::Response& res) {
        // mínimo: espera form-data o query ?username=&email=&password=
        auto username = req.get_param_value("username");
        auto email = req.get_param_value("email");
        auto pass = req.get_param_value("password");

        UserRepository repo(db.get());
        if (!repo.create(username, email, pass)) {
            res.status = 400; return;
        }
        res.status = 201;
    });

    api.Put(R"(/users/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1]);
        auto email = req.get_param_value("email");

        UserRepository repo(db.get());
        if (!repo.update(id, email)) { res.status = 400; return; }
        res.status = 204;
    });

    api.Delete(R"(/users/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.matches[1]);
        UserRepository repo(db.get());
        if (!repo.remove(id)) { res.status = 400; return; }
        res.status = 204;
    });
}
