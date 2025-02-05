#pragma once
#include <random>
#include <vector>
#include <string>
#include <type_traits>

namespace benchy {
    namespace utils { // Utility functions
        
        // Generate test data
        template<typename T>
        std::vector<T> generate_random_data(size_t size) {
            std::vector<T> data;
            std::random_device rd;
            std::mt19937 gen(rd());
            
            data.reserve(size);
            
            if constexpr (std::is_same_v<T, std::string>) {
                std::uniform_int_distribution<int> len_dis(5, 15);
                std::uniform_int_distribution<int> char_dis(97, 122);
                
                for (size_t i = 0; i < size; ++i) {
                    std::string str;
                    int len = len_dis(gen);
                    str.reserve(len);
                    for (int j = 0; j < len; ++j) {
                        str += static_cast<char>(char_dis(gen));
                    }
                    data.push_back(str);
                }
            } else {
                static_assert(std::is_arithmetic_v<T>, "Type must be either string or arithmetic");
                std::uniform_int_distribution<T> dis(1, 1000000);
                
                for (size_t i = 0; i < size; ++i) {
                    data.push_back(dis(gen));
                }
            }
            
            return data;
        }

    }
}