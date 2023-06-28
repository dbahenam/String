//
// Created by David Bahena Moctezuma
//

#ifndef STRING_STRING_H
#define STRING_STRING_H
#include <iostream>
#include <algorithm>
#include "BufferManager.h"


//It's up to you to decide the how the string and buffermanager
//will work together -- IS_A vs HAS-A.

namespace ECE141 {


    template<typename T=char, size_t aPresize = 32>
    class String {
    protected:

    public:
        BufferManager<char> stringBuffer;
        // OCF

        String() : stringBuffer(0) {}

        String(const String &aString) : stringBuffer(aString.stringBuffer){
            *this = aString;
        }

        String(const char *aString){
            *this = aString;
        }

        String &operator=(const String &aCopy) {
            return *this;
        }

        String &operator=(const char *aString){
            auto len = std::strlen(aString);
            stringBuffer.prepareInsert(len,0); //
            std::copy(aString, aString + len, stringBuffer.getBuffer());
            setTerminator();
            return *this;
        }

        // Move constructor
        String(String&& other) noexcept : stringBuffer(std::move(other.stringBuffer)){}

        // Move assignment operator
        String& operator=(String&& other) noexcept {
            if(this != &other){
                stringBuffer = std::move(other.stringBuffer);
            }
            return *this;
        }

        ~String() = default;

        [[nodiscard]] size_t size() const { return stringBuffer.getDataSize(); }

        // Mutation Methods
        T &operator[](size_t pos) {
            return stringBuffer[pos];
        }
        const T &operator[](size_t pos) const {
            return stringBuffer[pos];
        }

        String operator+(const String &aString){
            String result(*this); // copy constructor
            result += aString; // append aString
            return result; // RETURN VALUE OPTIMIZATION
        }

        String operator+(const char *aBuffer){
            String result(*this);
            result += aBuffer;
            return result;
        }
        //add method to support "hello"+theStringObject -- BONUS

        String &operator+=(const String &aString){
            auto strLen = aString.stringBuffer.getDataSize();
            stringBuffer.prepareInsert(strLen, stringBuffer.getDataSize());
            std::copy(aString.stringBuffer.begin(), aString.stringBuffer.end(), stringBuffer.end());
            setTerminator();
            return *this;
        }

        String &operator+=(const char *aBuffer){
            auto strLen = strlen(aBuffer);
            stringBuffer.prepareInsert(strLen, stringBuffer.getDataSize());
            std::copy(aBuffer, aBuffer+strLen, stringBuffer.end());
            setTerminator();
            return *this;
        }

        String &insert(size_t anIndex, const String &aStr, size_t aStrIndex,
                       size_t aStrCount) {
            stringBuffer.prepareInsert(aStrCount, anIndex);
            for(size_t ind = 0; ind < aStrCount; ind++, aStrIndex++, anIndex++){
                stringBuffer[anIndex] = aStr.stringBuffer[aStrIndex];
            }
            setTerminator();
            return *this;
        }

        //add method to insert const char*
        String &insert(size_t anIndex, const char *aStr, size_t aStrIndex,
                       size_t aStrCount){
            stringBuffer.prepareInsert(aStrCount, anIndex);
            for(size_t ind = 0; ind < aStrCount; ind++, aStrIndex++, anIndex++){
                stringBuffer[anIndex] = aStr[aStrIndex];
            }
            setTerminator();
            return *this;
        }

        String &insert(size_t anIndex, T aChar){
            stringBuffer.prepareInsert(1, anIndex);
            stringBuffer[anIndex] = aChar;
            setTerminator();
            return *this;
        }

        String &replace(size_t anIndex, size_t aMaxCopyLen, const String &aString);
        //add method to replace const char*

        String &erase(size_t anIndex, size_t aCount);

        //Comparision methods...
        int compare(const String &aString) const;
        //add method to add compare const char*

        bool operator==(const String &aString) const {
            if(stringBuffer.getDataSize() != aString.stringBuffer.getDataSize()){
                return false;
            }
            for(size_t ind = 0; ind < aString.stringBuffer.getDataSize(); ind++){
                if(stringBuffer[ind] != aString[ind]){
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const String &aString) const { return !(*this == aString); }

        bool operator<(const String &aString) const {
            for(size_t ind = 0; ind < aString.stringBuffer.getDataSize(); ind++){
                if(stringBuffer[ind] < aString[ind]){
                    return true;
                }
            }
            return stringBuffer.getDataSize() < aString.stringBuffer.getDataSize();
        }

        bool operator<=(const String &aString) const {
            if(*this < aString || *this == aString){
                return true;
            }
            return false;
        }

        bool operator>(const String &aString) const { return !(*this < aString);}

        bool operator>=(const String &aString) const {
            if(*this > aString || *this == aString){
                return true;
            }
            return false;
        }

        //Add version(s) to support const char*...
        bool operator==(const char* aBuffer) const {
            if(aBuffer == nullptr){return false;}
            auto dataSize = strlen(aBuffer);
            if(stringBuffer.getDataSize() != dataSize){
                return false;
            }
            size_t ind = 0;
            while(stringBuffer[ind] != '\0'){
                if(stringBuffer[ind] != aBuffer[ind]){
                    return false;
                }
                ind++;
            }
            return true;
        }

        bool operator!=(const char *aBuffer) const { return !(*this == aBuffer); }

        bool operator<(const char *aBuffer) const {
            if(aBuffer == nullptr){return false;}
            auto bufferSize = strlen(aBuffer);
            for(size_t ind = 0; ind < bufferSize; ind++){
                if(stringBuffer[ind] < aBuffer[ind]){
                    return true;
                }
            }
            return stringBuffer.getDataSize() < bufferSize;
        }

        bool operator<=(const char *aBuffer) const {
            if(*this < aBuffer || *this == aBuffer){
                return true;
            }
            return false;
        }

        bool operator>(const char *aBuffer) const { return !(*this < aBuffer);}

        bool operator>=(const char *aBuffer) const {
            if(*this > aBuffer || *this == aBuffer){
                return true;
            }
            return false;
        }

        //Search...
        int find(const T &aTarget, size_t anOffset=0) const {
            auto it = std::find(stringBuffer.begin() + anOffset, stringBuffer.end(), aTarget);
            if(it != stringBuffer.end()){
                return std::distance(stringBuffer.begin(), it);
            }
            return -1;
        }

        int find(char aChar, size_t anOffset=0) const {
            return find(static_cast<T>(aChar), anOffset);
        }

        int find(const char *aBuffer, size_t anOffset=0) const {
            auto it = std::search(stringBuffer.begin(), stringBuffer.end(),
                                  aBuffer, aBuffer + strlen(aBuffer));
            if(it != stringBuffer.end()){
                return std::distance(stringBuffer.begin(), it);
            }
            return -1;
        }

        int find(const String &aString, size_t anIndex = 0){
            auto it = std::search(stringBuffer.begin() + anIndex, stringBuffer.end(),
                                  aString.stringBuffer.begin(), aString.stringBuffer.end());
            if(it != stringBuffer.end()){
                return std::distance(stringBuffer.begin(), it);
            }
            return -1;
        }

        friend std::ostream &operator<<(std::ostream &anOut, const String &aStr) {
            for(const auto aChar : aStr.stringBuffer ){
                anOut << aChar;
            }
            return anOut;
        }

        friend std::istream &operator>>(std::istream &anIn, String &aString){
            return anIn;
        }

    private:
        void setTerminator(){
            auto dataSize = stringBuffer.getDataSize();
            stringBuffer[dataSize] = '\0';
        }
    };

}
#endif //STRING_STRING_H

