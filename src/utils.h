#pragma once
#include <string>
#include <array>
#include <memory>
#include <cstdio>

namespace Utils {
    // Executes a shell command and returns the output
    inline std::string exec(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        
        // Use function pointer type instead of decltype to avoid warning
        std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) {
            return "";
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }

        // Remove trailing newline
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
        
        return result;
    }
    
    inline std::string exec(const std::string& cmd) {
        return exec(cmd.c_str());
    }
}