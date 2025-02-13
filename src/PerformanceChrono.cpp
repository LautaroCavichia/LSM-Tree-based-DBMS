#pragma once
#include <chrono>
#include <iostream>

class PerformanceChrono {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::string label;

public:
    explicit PerformanceChrono(const std::string& label = "Operation") 
        : label(label) {
        start();
    }

    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time
        ).count();
        std::cout << "[Perf] " << label << " took " << duration << " ms\n";
    }
};