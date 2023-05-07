#ifndef _EXCEPTION_H
#define _EXCEPTION_H
#include <string>
#include <iostream>
#include <exception>

using namespace std;

/*
The "Failure" class inheritate the std::exception class and overwrite
the what() method to take a string and return that string.
 */
class Failure: public std::exception
{
private:
    string mErrMsg;

public:
  Failure(string errMsg):mErrMsg(errMsg){}
    const char* what() const noexcept
    {
        return mErrMsg.c_str();
    }
};


/*
The "VersionNotMatch" class inheritate the std::exception class and overwrite
the what() method to take a string and return that string.
 */
class VersionNotMatch: public std::exception
{
private:
    string mErrMsg;

public:
  VersionNotMatch(string errMsg):mErrMsg(errMsg){}
    const char* what() const noexcept
    {
        return mErrMsg.c_str();
    }
};


/*
The "FailurePrint" class inheritate the std::exception class.
*/
// class FailurePrint: public std::exception
// {
//   //nothing
// };

#endif