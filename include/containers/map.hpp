#pragma once

/**
 * @brief A custom hash map implementation optimized for performance and memory usage
 * 
 * Algorithm:
 * - Open addressing with quadratic probing for collision resolution
 * - Rolling hash function optimized for integer and pointer types
 * - Exponential growth strategy (factor of 2) with 0.75 load factor threshold
 * 
 * Performance characteristics vs std::map:
 * - O(1) average case for insertions and lookups vs O(log n) for std::map
 * - Better cache locality due to contiguous storage vs node-based storage
 * - More predictable performance due to simpler collision resolution
 * 
 * Pros:
 * - Fast lookups and insertions in the average case
 * - Memory efficient due to contiguous storage
 * - Move-only semantics prevent accidental copies
 * - Custom pair implementation reduces overhead
 * 
 * Cons:
 * - Worse worst-case performance (O(n) vs O(log n) for std::map)
 * - No ordering guarantees unlike std::map's sorted keys
 * - Limited to types that can be efficiently hashed
 * - Current hash function may have clustering issues
 * 
 * Potential improvements:
 * - Add proper exception handling
 * - Implement erase() functionality
 * - Add bucket interface for manual rehashing control
 * - Support custom hash functions and equality comparators
 * - Add initializer list and range constructors
 * - Consider robin hood hashing for better probe sequences
 */

namespace shared {
    /**
     * @brief Custom pair implementation optimized for move semantics
     * @tparam t1 Type of first element
     * @tparam t2 Type of second element
     */
    template <typename t1, typename t2>
    class pair {
    public:
        pair() : first(), second() {}
        
        template<typename T1, typename T2>
        pair(T1&& f, T2&& s) noexcept 
            : first(std::forward<T1>(f))
            , second(std::forward<T2>(s)) {}
        
        pair(pair&& other) noexcept 
            : first(std::move(other.first))
            , second(std::move(other.second)) {}
        
        pair& operator=(pair&& other) noexcept {
            if (this != &other) {
                first = std::move(other.first);
                second = std::move(other.second);
            }
            return *this;
        }

        // Prevent copying to enforce move semantics
        pair(const pair&) = delete;
        pair& operator=(const pair&) = delete;

        t1 first;
        t2 second;
    };

    /**
     * @brief Rolling hash function optimized for integer and pointer types
     * Uses FNV-1a inspired algorithm for good distribution
     */
    template <typename T>
    size_t hash_fn(const T& value) {
        const unsigned char* data = reinterpret_cast<const unsigned char*>(&value);
        size_t hash = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            hash = ((hash << 5) + hash) + data[i]; // hash * 33 + data[i]
        }
        return hash;
    }

    /**
     * @brief Hash map implementation using open addressing
     * @tparam k Key type
     * @tparam v Value type
     * @tparam InitialSize Initial capacity (must be power of 2)
     */
    template <typename k, typename v, size_t InitialSize = 8>
    class map {
    private:
        struct Entry {
            // Pack booleans into a single byte for space efficiency
            uint8_t state : 2;  // 0: empty, 1: occupied, 2: deleted
            pair<k, v> data;

            Entry() : state(0) {}

            template<typename K, typename V>
            void insert(K&& key, V&& value) {
                data = pair<k, v>(std::forward<K>(key), std::forward<V>(value));
                state = 1;
            }
        };

        Entry* entries;
        uint32_t capacity;  // Using uint32_t since we're unlikely to need maps larger than 4GB
        uint32_t m_size;    // Current number of occupied slots
        static constexpr float max_load_factor = 0.75f;

        /**
         * @brief Finds slot for key using quadratic probing
         * @return Index where key exists or should be inserted
         */
        size_t find_slot(const k& key) const noexcept {
            size_t hash = hash_fn(key);
            size_t index = hash & (capacity - 1);
            
            // Quadratic probing with power of 2 capacity ensures full table coverage
            for (size_t i = 0; ; i++) {
                if (entries[index].state != 1 || entries[index].data.first == key) {
                    return index;
                }
                index = (index + i) & (capacity - 1);
            }
        }

        /**
         * @brief Grows hash table and rehashes all elements
         * Doubles capacity and reinserts all existing elements
         */
        void grow() {
            uint32_t old_cap = capacity;
            Entry* old_entries = entries;

            capacity *= 2;
            entries = new Entry[capacity]();
            m_size = 0;

            for (uint32_t i = 0; i < old_cap; i++) {
                if (old_entries[i].state == 1) {
                    operator[](std::move(old_entries[i].data.first)) = 
                        std::move(old_entries[i].data.second);
                }
            }

            delete[] old_entries;
        }

    public:
        map() noexcept : capacity(InitialSize), m_size(0) {
            entries = new Entry[capacity]();
        }

        ~map() noexcept {
            delete[] entries;
        }

        map(map&& other) noexcept 
            : entries(other.entries)
            , capacity(other.capacity)
            , m_size(other.m_size) {
            other.entries = nullptr;
            other.capacity = 0;
            other.m_size = 0;
        }

        map& operator=(map&& other) noexcept {
            if (this != &other) {
                delete[] entries;
                entries = other.entries;
                capacity = other.capacity;
                m_size = other.m_size;
                other.entries = nullptr;
                other.capacity = 0;
                other.m_size = 0;
            }
            return *this;
        }

        /**
         * @brief Access or insert element
         * @param key The key to look up
         * @return Reference to value associated with key
         */
        v& operator[](const k& key) {
            if (static_cast<float>(m_size + 1) / capacity > max_load_factor) {
                grow();
            }

            size_t index = find_slot(key);
            if (entries[index].state != 1) {
                entries[index].insert(key, v());
                m_size++;
            }
            return entries[index].data.second;
        }

        /**
         * @brief Finds element with given key
         * @return Pointer to value if found, nullptr if not found
         */
        const v* find(const k& key) const noexcept {
            size_t index = find_slot(key);
            if (entries[index].state == 1 && entries[index].data.first == key) {
                return &entries[index].data.second;
            }
            return nullptr;
        }

        v* find(const k& key) noexcept {
            size_t index = find_slot(key);
            if (entries[index].state == 1 && entries[index].data.first == key) {
                return &entries[index].data.second;
            }
            return nullptr;
        }

        /**
         * @brief Removes all elements and resets to initial capacity
         */
        void clear() noexcept {
            delete[] entries;
            capacity = InitialSize;
            entries = new Entry[capacity]();
            m_size = 0;
        }

        size_t size() const noexcept { return m_size; }
        bool empty() const noexcept { return m_size == 0; }

        // Prevent copying to enforce move semantics
        map(const map&) = delete;
        map& operator=(const map&) = delete;

        /**
         * @brief Iterator for traversing occupied slots
         * Automatically skips empty and deleted slots
         */
        class iterator {
        private:
            Entry* entries;
            uint32_t capacity;
            uint32_t index;

            void advance() {
                while (index < capacity && entries[index].state != 1) {
                    ++index;
                }
            }

        public:
            iterator(Entry* e, uint32_t cap, uint32_t i) 
                : entries(e), capacity(cap), index(i) {
                advance();
            }

            pair<k, v>& operator*() noexcept { return entries[index].data; }
            const pair<k, v>& operator*() const noexcept { return entries[index].data; }

            iterator& operator++() noexcept {
                ++index;
                advance();
                return *this;
            }

            bool operator!=(const iterator& other) const noexcept {
                return index != other.index;
            }
        };

        iterator begin() noexcept {
            return iterator(entries, capacity, 0);
        }

        iterator end() noexcept {
            return iterator(entries, capacity, capacity);
        }
    };
}