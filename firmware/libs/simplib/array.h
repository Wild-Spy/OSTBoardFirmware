//
// Created by mcochrane on 5/04/17.
//

#ifndef WS_OST_ARRAY_H
#define WS_OST_ARRAY_H

#include <stddef.h>
#include <exception/CException.h>

namespace  sl {

    template<typename T, const size_t MAX_SIZE_>
    class Array {
    public:
        static const size_t MAX_SIZE = MAX_SIZE_;

    public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef size_t      size_type;

        Array() {
            count_ = 0;
        }

        size_t getCount() {
            return count_;
        }

        size_t append(T item) {
            if (count_ >= MAX_SIZE) Throw(EX_BUFFER_OVERRUN);
            buffer_[count_] = item;
            return count_++;
        }

        void remove_last() {
            count_--;
        }

        void remove(size_t index) {
            if (!isIndexValid(index)) Throw(EX_OUT_OF_RANGE);
            //0 1 2 3 4 5 6 7 8 9
            //    X
            //0 1 3 4 5 6 7 8 9
            //3->2, 9->8
            //index = 2 (count = 10)
            //index+1 -> index
            //index+2 -> index+1
            if (index == count_-1) {
                remove_last();
                return;
            }
            for (size_t i = index+1; i < count_; i++) {
                buffer_[i-1] = buffer_[i];
            }
            count_--;
        }

        T get(size_t index) {
            return *getPtr(index);
        }

        T& getRef(size_t index) {
            return *getPtr(index);
        }

        T* getPtr(size_t index) {
            if (!isIndexValid(index)) Throw(EX_OUT_OF_RANGE);
            return &buffer_[index];
        }

        /**
         * Clears the entire list.
         */
        void clear() {count_ = 0;}

    private:
        bool isIndexValid(size_t index) {
            return (index >= 0 && index < count_ && index < MAX_SIZE);
        }

    private:
        T buffer_[MAX_SIZE];
        size_t count_;
    };

}

#endif //WS_OST_ARRAY_H
