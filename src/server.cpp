#include "server.h"
#include "db.h"
#include "httplib.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace Server {
    httplib::Server svr;

    void Start(int port) {
        
        // Serve the UI HTML file
        svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
            
            std::vector<std::string> searchPaths = {
                "ui/index.html",
                "../ui/index.html",
                "../../ui/index.html",
                "../../../ui/index.html",
                "../../../../ui/index.html",
                "C:/Users/onure/Desktop/logger/ui/index.html"
            };

            bool found = false;
            for (const auto& path : searchPaths) {
                std::ifstream file(path);
                if (file.is_open()) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    res.set_content(buffer.str(), "text/html");
                    found = true;
                    break;
                }
            }

            if (!found) {
                res.set_content("UI not found! Please place index.html in ui/ folder.", "text/plain");
            }
        });

        // Provide stats as JSON
        svr.Get("/api/stats", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto topKeys = Database::GetTopKeys(50);
                
                json j = json::array();
                for (const auto& stat : topKeys) {
                    j.push_back({
                        {"key", stat.key},
                        {"count", stat.count}
                    });
                }
                res.set_content(j.dump(-1, ' ', false, json::error_handler_t::replace), "application/json");
            } catch (const std::exception& e) {
                std::cerr << "Stats JSON Error: " << e.what() << "\n";
                res.set_content("[]", "application/json");
            }
        });

        // Provide top words as JSON
        svr.Get("/api/words", [](const httplib::Request& req, httplib::Response& res) {
            try {
                auto topWords = Database::GetTopWords(15);
                
                json j = json::array();
                for (const auto& stat : topWords) {
                    j.push_back({
                        {"word", stat.key},
                        {"count", stat.count}
                    });
                }
                res.set_content(j.dump(-1, ' ', false, json::error_handler_t::replace), "application/json");
            } catch (const std::exception& e) {
                std::cerr << "Words JSON Error: " << e.what() << "\n";
                res.set_content("[]", "application/json");
            }
        });

        svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            res.status = 200;
        });

        int boundPort = port;
        bool bound = false;
        while(boundPort <= port + 20) {
            if(svr.bind_to_port("127.0.0.1", boundPort)) {
                bound = true;
                break;
            }
            boundPort++;
        }

        if(bound) {
            // Write bound port to a file so UI or Tray knows about it
            std::ofstream pFile("current_port.txt");
            if(pFile.is_open()) {
                pFile << boundPort;
                pFile.close();
            }

            svr.listen_after_bind();
        }
    }

    void Stop() {
        svr.stop();
    }
}
