#include <stddef.h>
#include <stdexcept>
#include <stdlib.h>
#include <cstring>
#include "logger.hpp"
#include <sstream>

const size_t k_default_capacity = 8192;

template <class T>
class BufferArray {

public:
    using iterator = T*;
    using const_iterator = const T*;

    BufferArray(size_t capacity = k_default_capacity);
    ~BufferArray ();

    const T* data() const;
    const T operator[](int idx);
    size_t size() const;
    void push_back (const T* data, size_t len);
    void erase_front (size_t len);

    iterator end();
    iterator begin();
    const_iterator end() const;
    const_iterator begin() const;
    
    std::string info() const;

private:

/*Data members*/
    T *_array_start;
    T *_array_end;
    T *_data_start;
    T *_data_end;
    size_t _capacity;
    size_t _data_len;

/*Private funcrtions*/

/**
 * 
 * @brief Resizing the array to be of `new_capacity` length.
 * @param data_start A pointer to the start of the data section in the array
 * @param data_len The length of the current data existing in the array
 * @param new_capacity The desired new length of the array after resizing
 * 
 */
    void resize(const T* data_start, size_t data_len, size_t new_capacity);
};


template <class T>
BufferArray<T>::BufferArray(size_t capacity) : 
    _capacity(capacity), _data_len(0){

    // allocate new memory
    _array_start = new T[_capacity];
    _array_end = _array_start + _capacity;
    _data_start = _array_start;
    _data_end = _array_start;

    LOG_INFO("New buffer has been initialized with capacity: " + std::to_string(_capacity));
}

template <class T>
BufferArray<T>::~BufferArray () {
    LOG_INFO("Inside BufferArray destructor!");
    delete[] _array_start;
}




template <class T>
const T* BufferArray<T>::data() const {
    return _data_start;
}

template <class T>
const T BufferArray<T>::operator[](int idx) {
    if (idx < 0 || idx >= this->_data_len) {
        throw std::runtime_error();
    }

    return this->_data_start[idx];
}

template <class T>
typename BufferArray<T>::iterator BufferArray<T>::end() {
    return _data_end;
}

template <class T>
typename BufferArray<T>::iterator BufferArray<T>::begin() {
    if (this->_data_len == 0) {
        return this->end();
    }

    return this->_data_start;
}

template <class T>
typename BufferArray<T>::const_iterator BufferArray<T>::end() const{
    return _data_end;
}

template <class T>
typename BufferArray<T>::const_iterator BufferArray<T>::begin() const{
    if (this->_data_len == 0) {
        return this->end();
    }
    return this->_data_start;
}

template <class T>
void BufferArray<T>::resize(const T* data_start, size_t data_len, size_t new_capacity) {
    LOG_INFO("Resizing array from " + std::to_string(_capacity) + " to " + std::to_string(new_capacity));
    // allocate new memory
    T* new_array = new T[new_capacity];

    // copy data into new array
    std::memcpy(new_array, data_start, sizeof(T)*data_len);
    delete[] _array_start;

    // update capacity & data len
    _capacity = new_capacity;
    _data_len = data_len;

    // update pointers
    _array_start = new_array;
    _data_start = new_array;
    _data_end = _data_start + _data_len;
    _array_end = _array_start + _capacity;

}

// template <class T>
// void BufferArray<T>::increase_capacity(size_t updated_data_len) {
//     resize();
// }

// template <class T>
// void BufferArray<T>::decrease_capacity(size_t updated_data_len) {
//     resize(updated_data_len << 1, updated_data_len);
// }

template <class T>
void BufferArray<T>::push_back (const T* data, size_t len) {
    // check if resize is needed
    if (len + _data_len >= _capacity) {
        // Double the capacity
        resize(_data_start, _data_len, (len + _data_len) << 1);
    }

    LOG_INFO("Pushing back data!");
    std::memcpy(_data_end, data, len * sizeof(T));
    _data_end = _data_end + len;
    // data len after the append
    _data_len += len;
}

template <class T>
void BufferArray<T>::erase_front (size_t len) {
    LOG_INFO("Erasing data!");
    // erase data
    _data_start += len;
    _data_len -= len;

    // check if resize is needed
    if (_data_len <= _capacity >> 2 && _capacity >= k_default_capacity) {
        // Shrink capacity when used size is <= capacity/4
        resize(_data_start, _data_len, _data_len << 1);
    }
}

template<class T>
size_t BufferArray<T>::size() const {
    return this->_data_len;
}

template <class T>
std::string BufferArray<T>::info() const {
    std::ostringstream oss;
    oss << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                << "data: " << (void *)_data_start << " -> " << (void *)_data_end << " | LEN: " << _data_len
                << std::endl
                << "Array: " << (void *)_array_start << " -> " << (void *)_array_end << " | CAP: " << _capacity
                << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
                << std::endl;
    return oss.str();
}