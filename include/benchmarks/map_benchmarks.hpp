#include <benchmark/benchmark.h>
#include <map>
#include "../containers/map.hpp"
#include "../utils/utils.hpp"

namespace benchy {
    /**
     * Performance comparison suite between custom map and std::map implementations.
     * Tests insertion and lookup operations with both integer and string keys.
     * Each benchmark runs with varying input sizes (8 to 8192 elements).
     */

    static void BM_CustomMapInsertion(benchmark::State& state) {
        for (auto _ : state) {
            shared::map<int, int> m;
            for (int i = 0; i < state.range(0); ++i) {
                m[i] = i;
            }
        }
    }

    static void BM_StdMapInsertion(benchmark::State& state) {
        for (auto _ : state) {
            std::map<int, int> m;
            for (int i = 0; i < state.range(0); ++i) {
                m[i] = i;
            }
        }
    }

    static void BM_CustomMapLookup(benchmark::State& state) {
        shared::map<int, int> m;
        for (int i = 0; i < state.range(0); ++i) {
            m[i] = i;
        }

        for (auto _ : state) {
            for (int i = 0; i < state.range(0); ++i) {
                benchmark::DoNotOptimize(m.find(i));
            }
        }
    }

    static void BM_StdMapLookup(benchmark::State& state) {
        std::map<int, int> m;
        for (int i = 0; i < state.range(0); ++i) {
            m[i] = i;
        }

        for (auto _ : state) {
            for (int i = 0; i < state.range(0); ++i) {
                benchmark::DoNotOptimize(m.find(i));
            }
        }
    }

    static void BM_CustomMapStringInsertion(benchmark::State& state) {
        auto keys = benchy::utils::generate_random_data<std::string>(state.range(0));
        for (auto _ : state) {
            shared::map<std::string, int> m;
            for (int i = 0; i < state.range(0); ++i) {
                m[keys[i]] = i;
            }
        }
    }

    static void BM_StdMapStringInsertion(benchmark::State& state) {
        auto keys = benchy::utils::generate_random_data<std::string>(state.range(0));
        for (auto _ : state) {
            std::map<std::string, int> m;
            for (int i = 0; i < state.range(0); ++i) {
                m[keys[i]] = i;
            }
        }
    }
}

// Register benchmarks with increasing sizes (8 to 8K elements)
BENCHMARK(benchy::BM_CustomMapInsertion)->Range(8, 8 << 10);
BENCHMARK(benchy::BM_StdMapInsertion)->Range(8, 8 << 10);
BENCHMARK(benchy::BM_CustomMapLookup)->Range(8, 8 << 10);
BENCHMARK(benchy::BM_StdMapLookup)->Range(8, 8 << 10);
BENCHMARK(benchy::BM_CustomMapStringInsertion)->Range(8, 8 << 10);
BENCHMARK(benchy::BM_StdMapStringInsertion)->Range(8, 8 << 10); 