
#ifndef Exception_h
#define Exception_h 1

#include <string>
using namespace std;

class Exception {
public:
    Exception(const string msg = 0) : message(msg) {}
    string GetMessage() { return message;}
    
private:
	    const string message;
};

//Input Output Exception
class IOException : public Exception {
public:
    IOException(const string msg = 0) : Exception(msg) {} 
};


class ParserException : public IOException {
public:
    ParserException(const string msg = 0) : IOException(msg) {} 
};    
    
class InvalidArgumentException : public Exception {
public:
    InvalidArgumentException(const string msg = 0) : Exception(msg) {} 

};
#endif
