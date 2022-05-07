#ifndef HTTPCONTENT_H_
#define HTTPCONTENT_H_

#include <Arduino.h>

typedef enum HTTPType
{
  HTTP_BODY = 1, HTTP_HEADER = 2
} HTTPType_t;

class HTTPContent {
  public:
    
    HTTPContent();
    HTTPContent(HTTPType_t);
    
    String toString(bool h = false);
    bool addLine(String);
    bool set(String);
    bool clear();
    
  private:
    uint8_t type=HTTP_HEADER;
    char* raw=NULL;
    int size=0;
    
};

#endif
