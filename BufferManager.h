//
// Created by David Bahena Moctezuma
//

#ifndef STRING_BUFFERMANAGER_H
#define STRING_BUFFERMANAGER_H

#include <iostream>
#include <memory>
#include <cstring>

namespace ECE141 {

    template<typename T>
    class BufferManager {

    protected:
        std::unique_ptr<T[]> theBuffer;
        size_t bufferSize;
        size_t dataSize;
        const size_t scaleFactor = 2;
        const double resizeThreshold = 0.75;
        const double compactThreshold = 0.05;

    public:
        // avoids making implicit conversion
        explicit BufferManager(size_t aSize = 0) : dataSize{aSize} {
            bufferSize = dataSize * scaleFactor;
            theBuffer = (0 == aSize) ? nullptr : std::make_unique<T[]>(bufferSize);
        }

        BufferManager(const BufferManager &aCopy) : bufferSize{aCopy.bufferSize} {
            *this = aCopy;
        }

        BufferManager &operator=(const BufferManager &other) {
            if(this != &other){
                dataSize = other.dataSize;
                if(0 == dataSize){
                    theBuffer = nullptr;
                    bufferSize = 0;
                } else{
                    auto newBuffer = std::make_unique<T[]>(other.bufferSize);
                    std::memcpy(newBuffer.get(), other.theBuffer.get(), sizeof(T) * dataSize);
                    theBuffer = std::move(newBuffer);
                    bufferSize = other.bufferSize;
                }
            }
            return *this;
        }

        // move constructor
        BufferManager(BufferManager&& other) noexcept : bufferSize{0}, dataSize{0}, theBuffer{nullptr} {
            *this = std::move(other);
        }

        BufferManager &operator=(BufferManager &&other) noexcept{
            if(this != &other){
                theBuffer = std::move(other.theBuffer);
                bufferSize = other.bufferSize;
                dataSize = other.dataSize;

                other.bufferSize = 0;
                other.dataSize = 0;
            }
            return *this;
        }

        // overload subscript operator
        T &operator[](size_t anIndex){
            if(anIndex >= dataSize + 1){
                throw std::out_of_range("Index out of range");
            }
            return theBuffer[anIndex];
        }
        const T &operator[](size_t anIndex) const {
            if(anIndex >= dataSize + 1){
                throw std::out_of_range("Index out of range");
            }
            return theBuffer[anIndex];
        }

        // range-based for loops, std::copy
        T* begin(){
            return theBuffer.get();
        }

        T* end(){
            return theBuffer.get() + dataSize;
        }

        const T* begin() const{
            return theBuffer.get();
        }

        const T* end() const {
            return theBuffer.get() + dataSize;
        }

        ~BufferManager() = default;

        //----
        [[nodiscard]] size_t getCapacity() const {
            return bufferSize;
        }

        [[nodiscard]] size_t getDataSize() const {
            return dataSize;
        }

        T *getBuffer() const {
            return theBuffer.get();
        }

        size_t prepareInsert(size_t numValues, size_t anOffset) {
            // if theBuffer is initially empty
            if (nullptr == theBuffer) {
                bufferSize = numValues * scaleFactor;
                theBuffer = std::make_unique<T[]>(bufferSize);
            }
                // check if needed to expand
            else if(dataSize + numValues >= bufferSize * resizeThreshold){
                willExpand(bufferSize * scaleFactor, anOffset, numValues);
            }
            else{
                shiftRight(anOffset, numValues);
            }
            dataSize += numValues;
            return 0;
        }

        void willExpand(size_t aNewSize, size_t anOffset, size_t numValues) {
            // create new expanded buffer
            std::unique_ptr<T[]> newBuffer = std::make_unique<T[]>(aNewSize);

            // appending
            if(anOffset == dataSize){
                std::memcpy(newBuffer.get(), theBuffer.get(), sizeof(T) * dataSize);
            }

                // if offset is zero
            else if(anOffset == 0){
                std::memcpy(newBuffer.get(), theBuffer.get(), sizeof(T) * dataSize);
                theBuffer = std::move(newBuffer);
                shiftRight(0, numValues);

            }
                //
            else{
                // copy the data up the offset index
                std::memcpy(newBuffer.get(), theBuffer.get(), sizeof(T) * anOffset);

                // calculate the remaining data size : if we had datasize of 10 and off set is 4, we have 6 data values left
                auto remainingData = dataSize - anOffset;

                // start copy from initial index + numValues
                std::memcpy(newBuffer.get() + numValues, theBuffer.get() + anOffset,
                            sizeof(T) * remainingData);
                theBuffer = std::move(newBuffer);
            }
            bufferSize = aNewSize;

        }

        BufferManager &prepareErase(size_t numValues, size_t anOffset = 0) {
            dataSize -= numValues;
            // if theBuffer is initially empty
            if (nullptr == theBuffer) {
                bufferSize = dataSize * scaleFactor;
                theBuffer = std::make_unique<T[]>(bufferSize);
            }
                // if dataSize exceeds bufferSize * resizeThreshold, expand and shift data for offset
            else if (dataSize < bufferSize * compactThreshold) {
                willCompact(dataSize * scaleFactor, anOffset, numValues);
            }
                // shift data where offset indicates
            else if(anOffset != dataSize - numValues){
                shiftRight(anOffset, numValues);
            }
            return *this;
        }

        void willCompact(size_t aNewSize, size_t anOffset = 0) {
            std::unique_ptr<T[]> newBuffer = std::make_unique<T[]>(aNewSize);

            if (anOffset != 0) {
                std::memcpy(newBuffer.get(), theBuffer.get(), sizeof(T) * anOffset);

                auto remainingData = dataSize - anOffset;

                std::memcpy(newBuffer.get() + anOffset + aNewSize - dataSize, theBuffer.get() + anOffset,
                            sizeof(T) * remainingData);
            } else {
                std::memcpy(newBuffer.get(), theBuffer.get(), sizeof(T) * dataSize);
            }

            bufferSize = aNewSize;
            theBuffer = std::move(newBuffer);
        }

    private:
        void shiftRight(size_t startPos, size_t shiftAmount){
            /*
             * copy_backward copies elements in reverse order from back to front, useful for
             * copying elements that can potentially overlap.
             * The function works by starting at the end of the source range and the end of
             * the destination range and copying the elements one by one, moving towards the
             * start of the source range.
             */
            auto source = theBuffer.get() + startPos;
            auto sourceEnd = theBuffer.get() + dataSize;
            auto destinationEnd = sourceEnd + shiftAmount;
            std::copy_backward(source, sourceEnd, destinationEnd);
        }

        void shiftLeft(size_t startPos, size_t shiftAmount){
            auto source = theBuffer.get() + startPos + shiftAmount;
            auto sourceEnd = theBuffer.get() + dataSize;
            auto destination = theBuffer.get() + startPos;

            std::copy(source, sourceEnd, destination);
        }



    };

}

#endif //STRING_BUFFERMANAGER_H
