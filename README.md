# Benchy: C++ Container Performance Analysis (Proof of Concept)

A proof-of-concept C++ benchmarking project comparing custom container implementations against STL counterparts. This project serves as an educational demonstration using Google Benchmark to analyze performance characteristics of different container implementations.

## Overview

This experimental project implements and benchmarks custom container classes, focusing on:
- Basic performance optimization techniques
- Modern C++ implementation examples
- Comparative analysis against STL implementations
- Demonstration of benchmarking methodology

### Current Implementations (Experimental)
- **Custom Vector**
  - Basic O(1) amortized operations
  - Move semantics optimization examples
  - Manual memory management demonstration
  
- **Custom Hash Map**
  - Simple open addressing with quadratic probing
  - Basic cache locality optimizations
  - Move-only semantics implementation

*Note: These implementations are for demonstration purposes and lack production-ready features. Users should review the code comments for limitations and potential improvements.*

## Building the Project

### Prerequisites
- CMake (3.15 or higher)
- C++17 compatible compiler
- Git

### Linux
```bash
# Clone the repository with submodules
git clone https://github.com/ehhbased/benchy.git --recursive
cd benchy

# Create build directory and build
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Windows
```bash
# Clone the repository with submodules
git clone https://github.com/ehhbased/benchy.git --recursive
cd benchy

# Run the build script
build.bat
```

## Running Benchmarks
```bash
./benchy_main
```

The benchmark suite includes:
- Operation performance (insertion, access, deletion)
- Memory usage patterns
- Cache efficiency
- Tests with varying data sizes (8 to 8192 elements)
- Multiple data type comparisons

## Project Structure
```
benchy/
├── include/
│   ├── containers/     # Custom container implementations
│   ├── benchmarks/     # Benchmark definitions
│   └── utils/          # Utility functions
├── src/
│   └── main.cpp        # Benchmark runner
└── benchmark/          # Google Benchmark submodule
```

## References
- [Google Benchmark](https://github.com/google/benchmark)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Chandler Carruth's CppCon Talk on Benchmarking](https://www.youtube.com/watch?v=nXaxk27zwlk)
- [Open Addressing Hash Tables](https://en.wikipedia.org/wiki/Open_addressing)

## License
This project is for educational and demonstration purposes.
