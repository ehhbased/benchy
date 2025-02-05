#pragma once

/**
 * @brief A custom vector implementation with unique features and comparable performance to std::vector
 * 
 * Performance characteristics:
 * - O(1) amortized push_back due to exponential growth strategy (growth factor of 2)
 * - O(1) random access via operator[] and at()
 * - O(n) for resizing and reserve operations
 * 
 * Key differences from std::vector:
 * - Custom deleter support for specialized cleanup of elements
 * - Move semantics prioritized over copying for better performance with movable types
 * - Manual memory management using placement new/delete for more control
 * 
 * Areas for improvement:
 * - Exception handling needs to be implemented (currently missing in at())
 * - Could add emplace_back() for in-place construction
 * - Shrink_to_fit() functionality could be added to reclaim unused capacity
 * - Iterator implementation could be expanded to include reverse iterators
 * - Missing some standard container typedefs (value_type, reference, etc.)
 */

namespace shared {
    /**
     * @brief Function pointer type for custom element deletion
     * @tparam T The type of elements stored in the vector
     */
    template<typename T>
    using deleter_fn = void(*)(T&&);

    template<class T>
    class vector {
    private:
        size_t _size;      // Current number of elements
        T* _elements;      // Pointer to elements array
        size_t _space;     // Total allocated capacity
        deleter_fn<T> _deleter;  // Optional custom cleanup function

        /**
         * @brief Cleans up all elements and deallocates memory
         * Uses custom deleter if provided, otherwise calls destructor
         */
        void clean_up() {
            if (_elements) {
                for (size_t i = 0; i < _size; i++) {
                    if (_deleter) _deleter(std::move(_elements[i]));
                    else _elements[i].~T();
                }
                ::operator delete(_elements);
                _elements = nullptr;
            }
            _size = _space = 0;
        }

    public:
        /**
         * @brief Default constructor
         * @param custom_deleter Optional function for custom element cleanup
         */
        vector(deleter_fn<T> custom_deleter = nullptr)
            : _size(0), _elements(nullptr), _space(0), _deleter(custom_deleter) {}

        /**
         * @brief Constructs vector with given size, default-initializing elements
         * @param s Initial size of the vector
         * @param custom_deleter Optional function for custom element cleanup
         */
        explicit vector(size_t s, deleter_fn<T> custom_deleter = nullptr)
            : _size(0), _elements(nullptr), _space(0), _deleter(custom_deleter)
        {
            if (s > 0) {
                _elements = static_cast<T*>(::operator new(s * sizeof(T)));
                _space = s;
                for (size_t i = 0; i < s; i++) {
                    new (_elements + i) T();
                }
                _size = s;
            }
        }

        /**
         * @brief Copy constructor
         * Creates deep copy of other vector's elements
         */
        vector(const vector& other)
            : _size(0), _elements(nullptr), _space(0), _deleter(other._deleter)
        {
            if (other._size > 0) {
                _elements = static_cast<T*>(::operator new(other._size * sizeof(T)));
                _space = _size = other._size;

                for (size_t i = 0; i < _size; i++) {
                    new (_elements + i) T(other._elements[i]);
                }
            }
        }

        /**
         * @brief Move constructor
         * Transfers ownership of other vector's resources
         */
        vector(vector&& other) noexcept
            : _size(other._size), _elements(other._elements), _space(other._space), _deleter(other._deleter)
        {
            other._elements = nullptr;
            other._size = other._space = 0;
        }

        /**
         * @brief Initializer list constructor
         * Constructs vector from list of elements
         */
        template<class U>
        vector(std::initializer_list<U> init, deleter_fn<T> custom_deleter = nullptr)
            : _size(0), _elements(nullptr), _space(0), _deleter(custom_deleter)
        {
            reserve(init.size());
            for (const auto& item : init) {
                push_back(item);
            }
        }

        /**
         * @brief Copy assignment operator
         * Creates deep copy of other vector's elements
         */
        vector& operator=(const vector& other) {
            if (this == &other) return *this;

            clean_up();

            if (other._size > 0) {
                _elements = static_cast<T*>(::operator new(other._size * sizeof(T)));
                _space = _size = other._size;

                for (size_t i = 0; i < _size; i++) {
                    new (_elements + i) T(other._elements[i]);
                }
            }
            _deleter = other._deleter;
            return *this;
        }

        /**
         * @brief Move assignment operator
         * Transfers ownership of other vector's resources
         */
        vector& operator=(vector&& other) noexcept {
            if (this != &other) {
                clean_up();
                _elements = other._elements;
                _size = other._size;
                _space = other._space;
                _deleter = other._deleter;
                other._elements = nullptr;
                other._size = other._space = 0;
            }
            return *this;
        }

        ~vector() {
            clean_up();
        }

        /**
         * @brief Basic iterator implementation for container traversal
         */
        class iterator {
            T* _curr;
        public:
            iterator(T* p) : _curr(p) {}
            iterator& operator++() { ++_curr; return *this; }
            iterator& operator--() { --_curr; return *this; }
            T& operator*() { return *_curr; }
            bool operator==(const iterator& other) const { return _curr == other._curr; }
            bool operator!=(const iterator& other) const { return _curr != other._curr; }
        };

        // Iterator access methods
        iterator begin() { return iterator(_elements); }
        iterator end() { return iterator(_elements + _size); }
        const iterator begin() const { return iterator(_elements); }
        const iterator end() const { return iterator(_elements + _size); }

        /**
         * @brief Reserves space for future growth
         * @param new_alloc New capacity to allocate
         */
        void reserve(size_t new_alloc) {
            if (new_alloc <= _space) return;

            T* new_elements = static_cast<T*>(::operator new(new_alloc * sizeof(T)));

            for (size_t i = 0; i < _size; i++) {
                new (new_elements + i) T(std::move(_elements[i]));
                _elements[i].~T();
            }

            ::operator delete(_elements);
            _elements = new_elements;
            _space = new_alloc;
        }

        /**
         * @brief Resizes vector to new size
         * @param new_size Desired size
         * @param val Value to initialize new elements with
         */
        void resize(size_t new_size, const T& val = T()) {
            if (new_size <= _size) {
                while (_size > new_size) {
                    _elements[--_size].~T();
                }
            }
            else {
                if (new_size > _space) {
                    reserve(new_size);
                }
                while (_size < new_size) {
                    new (_elements + _size++) T(val);
                }
            }
        }

        /**
         * @brief Adds element to end (copy version)
         * Automatically grows container if needed
         */
        void push_back(const T& val) {
            if (_space == 0) {
                reserve(8);
            }
            else if (_size == _space) {
                reserve(2 * _space);
            }
            new (_elements + _size) T(val);
            ++_size;
        }

        /**
         * @brief Adds element to end (move version)
         * Automatically grows container if needed
         */
        template<class U>
        void push_back(U&& value) {
            if (_space == 0) {
                reserve(8);
            }
            else if (_size == _space) {
                reserve(2 * _space);
            }
            new (_elements + _size) T(std::move(value));
            ++_size;
        }

        // Element access operators
        T& operator[](size_t i) { return _elements[i]; }
        const T& operator[](size_t i) const { return _elements[i]; }

        /**
         * @brief Bounds-checked element access
         * @todo Implement proper exception handling
         */
        T& at(size_t i) {
            if (i >= _size) {
                // TODO: throw std::out_of_range exception
                return _elements[0];
            }
            return _elements[i];
        }

        const T& at(size_t i) const {
            if (i >= _size) {
                // TODO: throw std::out_of_range exception
                return _elements[0];
            }
            return _elements[i];
        }

        // Container manipulation methods
        void clear() { clean_up(); }
        bool empty() const { return _size == 0; }
        size_t size() const { return _size; }
        size_t capacity() const { return _space; }
        T* data() { return _elements; }
        const T* data() const { return _elements; }

        // Element access methods
        T& front() { return _elements[0]; }
        const T& front() const { return _elements[0]; }
        T& back() { return _elements[_size - 1]; }
        const T& back() const { return _elements[_size - 1]; }
    };
}