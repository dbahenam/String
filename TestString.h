//
// Created by David Bahena Moctezuma on 6/23/23.
//

#ifndef ASSIGNMENT2_TESTSTRING_H
#define ASSIGNMENT2_TESTSTRING_H
#include "Testable.h"
#include "String.h"
#include <map>

class TestString : public ECE141::Testable, public ECE141::String<char>{
public:
    TestString() : Testable(){
        count = testList.size();
    }
    using Callable = bool (TestString::*)();
    std::map<std::string, Callable> testList{
            {"testConstructor",   &TestString::testConstructor},
            {"testCopyConstruct", &TestString::testCopyConstruct},
            {"testInsert", &TestString::testInsert},
            {"testSum", &TestString::testSum},
    };
    OptString getTestName(size_t anIndex) const override{
        size_t theCount{0};
        for(auto thePair : testList){
            if(anIndex == theCount){
                return thePair.first;
            }
            theCount++;
        }
        return std::nullopt;
    }
    bool operator()(const std::string &aName) override{
        return testList.count(aName) ? (this->*testList[aName])() : false;
    }

    /* test methods */

    bool testConstructor(){
        /* test const char* initialization */
        const char* theName = "hello world";
        String<char> theString{theName};
        auto theResult = compareValues(theString, theName);
        if(theResult){
            /* test String initialization */
            String<char> theString2 = theString;
            theResult = compareValues(theString2, theString);
        }
        return theResult;
    }

    bool testCopyConstruct(){
        String theString{"hello world"};
        String theString2{theString};
        size_t ind{0};
        return compareValues(theString, theString2);
    }

    bool testInsert(){
        const char* str = "Greattt ";
        auto strLen = strlen(str);
        String theString{"Hello World!"};
        theString.insert(0, str, 0, strLen);

        const char* str2 = "hello world! ";
        auto strLength = strlen(str2);
        String theString2{"HELLO WORLD!"};
        theString2.insert(0, str2, 0, strLength);
        if(theString != "Greattt Hello World!"){
            return false;
        }
        return true;
    }

    bool testSum(){
        String str1{"hello "};
        String str2{"world!"};
        auto theStr = str1 + str2;
        if(theStr != "hello world!"){
            return false;
        }
        theStr += " bye world!";
        if(theStr != "hello world! bye world!"){
            return false;
        }
        return true;
    }

private:
    bool compareValues(const String<char>& mainStr, const char* &secondaryStr){
        size_t ind{0};
        while(mainStr[ind] != '\0'){
            if(secondaryStr[ind] != mainStr[ind]){
                return false;
            }
            ind++;
        }
        return true;
    }
    bool compareValues(const String<char>& mainStr, const String<char>& secondaryStr){
        size_t ind{0};
        while(mainStr[ind] != '\0'){
            if(secondaryStr[ind] != mainStr[ind]){
                return false;
            }
            ind++;
        }
        return true;
    }
};

#endif //ASSIGNMENT2_TESTSTRING_H