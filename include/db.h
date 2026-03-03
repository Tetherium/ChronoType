#pragma once

#include <string>
#include <vector>

namespace Database {
    // Structure to represent a keypress stat
    struct KeyStat {
        std::string key;
        int count;
    };

    // Initialize the SQLite database
    bool Initialize(const std::string& dbPath);

    // Insert a new keypress
    void LogKeyPress(const std::string& keyName);

    // Get the top N most pressed keys
    std::vector<KeyStat> GetTopKeys(int limit = 50);

    // Insert a distinct word
    void LogWord(const std::string& word);

    // Get the top N most typed words
    std::vector<KeyStat> GetTopWords(int limit = 15);
}
