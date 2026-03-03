#include "db.h"
#include "sqlite3.h"
#include <iostream>

namespace Database {
    sqlite3* db = nullptr;

    bool Initialize(const std::string& dbPath) {
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
            return false;
        }

        const char* sql = "CREATE TABLE IF NOT EXISTS KeyLogs ("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "KeyName TEXT NOT NULL,"
                          "Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
                          ");"
                          "CREATE TABLE IF NOT EXISTS WordLogs ("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "Word TEXT NOT NULL,"
                          "Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
                          ");";

        char* errMsg = nullptr;
        rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << "\n";
            sqlite3_free(errMsg);
            return false;
        }

        std::cout << "Database initialized.\n";
        return true;
    }

    void LogKeyPress(const std::string& keyName) {
        if (!db) return;

        const char* sql = "INSERT INTO KeyLogs (KeyName) VALUES (?);";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Prepare error: " << sqlite3_errmsg(db) << "\n";
            return;
        }

        sqlite3_bind_text(stmt, 1, keyName.c_str(), -1, SQLITE_STATIC);
        
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Log insertion error: " << sqlite3_errmsg(db) << "\n";
        }
        sqlite3_finalize(stmt);
    }

    std::vector<KeyStat> GetTopKeys(int limit) {
        std::vector<KeyStat> stats;
        if (!db) return stats;

        std::string sql = "SELECT KeyName, COUNT(*) as Count FROM KeyLogs "
                          "GROUP BY KeyName ORDER BY Count DESC LIMIT " + std::to_string(limit) + ";";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to fetch top keys\n";
            return stats;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            KeyStat stat;
            stat.key = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            stat.count = sqlite3_column_int(stmt, 1);
            stats.push_back(stat);
        }

        sqlite3_finalize(stmt);
        return stats;
    }

    void LogWord(const std::string& word) {
        if (!db) return;

        const char* sql = "INSERT INTO WordLogs (Word) VALUES (?);";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Prepare error: " << sqlite3_errmsg(db) << "\n";
            return;
        }

        sqlite3_bind_text(stmt, 1, word.c_str(), -1, SQLITE_STATIC);
        
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Word insertion error: " << sqlite3_errmsg(db) << "\n";
        }
        sqlite3_finalize(stmt);
    }

    std::vector<KeyStat> GetTopWords(int limit) {
        std::vector<KeyStat> stats;
        if (!db) return stats;

        std::string sql = "SELECT Word, COUNT(*) as Count FROM WordLogs "
                          "GROUP BY Word ORDER BY Count DESC LIMIT " + std::to_string(limit) + ";";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to fetch top words\n";
            return stats;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            KeyStat stat;
            stat.key = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            stat.count = sqlite3_column_int(stmt, 1);
            stats.push_back(stat);
        }

        sqlite3_finalize(stmt);
        return stats;
    }
}
