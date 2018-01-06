/*
 * Queue.h
 *
 *  Created on: Jan 5, 2018
 *      Author: fcos
 */

#ifndef UTILS_QUEUE_H_
#define UTILS_QUEUE_H_

#include <stddef.h>
#include <stdint.h>

namespace i2c_hat {
namespace utils {

template<size_t N>
class Queue {
public:
    Queue();
    uint32_t capacity();
    bool IsEmpty();
    bool IsFull();
    void Clear();
    bool Put(const uint32_t value);
    bool Get(uint32_t &value);
private:
    uint32_t queue_data_[N];
    uint32_t front_;
    uint32_t rear_;
};

/**
  * @brief  Builds a Queue object.
  * @param [in] size    Queue size, use only power of 2 like: 2, 4, 8, 16, 32, 64 ...
  * @retval None
  */
template <size_t N>
Queue<N>::Queue() :
    front_(0),
    rear_(0) {
    // We are assuming that for an empty Queue, both front and rear will be -1.
}

/**
  * @brief  Gets Queue size.
  * @param  None
  * @retval Queue capacity, it's (size - 1), only 63 elements int in a 64 size queue, because of the implementation.
  */
template <size_t N>
uint32_t Queue<N>::capacity() {
    return N - 1;
}

/**
  * @brief  Checks if Queue is empty.
  * @param  None
  * @retval TRUE   Queue is empty.
  * @retval FALSE  Queue is not empty.
  */
template <size_t N>
bool Queue<N>::IsEmpty() {
    return (front_ ==  rear_);
}

/**
  * @brief  Checks if Queue is full.
  * @param  None
  * @retval TRUE   Queue is full.
  * @retval FALSE  Queue is not full.
  */
template <size_t N>
bool Queue<N>::IsFull() {
    return ((rear_+ 1) & (N - 1)) == front_;
}

/**
  * @brief  Clears/empties the Queue.
  * @param  None
  * @retval None
  */
template <size_t N>
void Queue<N>::Clear() {
    front_ = rear_ = 0;
}

/**
  * @brief  Puts a value in the Queue.
  * @param [in] value   Value to be stored.
  * @retval TRUE   Value was stored successfully.
  * @retval FALSE  Queue is full, value was not stored.
  */
template <size_t N>
bool Queue<N>::Put(const uint32_t value) {
    bool success = false;

    if(not IsFull()) {
        // store value in the rear end.
        queue_data_[rear_] = value;
        rear_ = (rear_ + 1) & (N - 1);
        success = true;
    }
    return success;
}

/**
  * @brief  Gets a value from the Queue.
  * @param [out] value   Value returned from the queue.
  * @retval TRUE   Value was returned successfully.
  * @retval FALSE  Queue is empty, no value can be returned.
  */
template <size_t N>
bool Queue<N>::Get(uint32_t &value) {
    bool success = false;

    if(not IsEmpty()) {
        // return value from the front end
        value = queue_data_[front_];
        front_ = (front_ + 1) & (N - 1);
        success = true;
    }
    return success;
}

} /* namespace utils */
} /* namespace i2c_hat */

#endif /* UTILS_QUEUE_H_ */
