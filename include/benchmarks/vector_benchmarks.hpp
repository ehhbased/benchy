#include <benchmark/benchmark.h>
#include <vector>
#include "../containers/vector.hpp"
#include "../utils/utils.hpp"

namespace benchy {

    /**
     * Performance comparison suite between custom vector and std::vector implementations.
     * Measures two key aspects:
     * 1. Dynamic growth efficiency through push_back operations
     * 2. Random access performance using operator[]
     * 
     * Each benchmark runs with varying input sizes from 8 to 8192 elements
     * to analyze performance characteristics at different scales.
     */

    static void BM_CustomVectorPushBack(benchmark::State& state) {
        for (auto _ : state) {
            shared::vector<int> v;
            for (int i = 0; i < state.range(0); ++i) {
                v.push_back(i);
            }
        }
    }

    static void BM_StdVectorPushBack(benchmark::State& state) {
        for (auto _ : state) {
            std::vector<int> v;
            for (int i = 0; i < state.range(0); ++i) {
                v.push_back(i);
            }
        }
    }

    static void BM_CustomVectorAccess(benchmark::State& state) {
        shared::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }

        for (auto _ : state) {
            for (int i = 0; i < state.range(0); ++i) {
                benchmark::DoNotOptimize(v[i]);
            }
        }
    }

    static void BM_StdVectorAccess(benchmark::State& state) {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }

        for (auto _ : state) {
            for (int i = 0; i < state.range(0); ++i) {
                benchmark::DoNotOptimize(v[i]);
            }
        }
    }
}

// Register benchmarks with exponentially increasing sizes
BENCHMARK(benchy::BM_CustomVectorPushBack)->Range(8, 8 << 10);
BENCHMARK(benchy::BM_StdVectorPushBack)->Range(8, 8 << 10);
BENCHMARK(benchy::BM_CustomVectorAccess)->Range(8, 8 << 10);
BENCHMARK(benchy::BM_StdVectorAccess)->Range(8, 8 << 10); 