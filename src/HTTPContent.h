/*
   HTTPContent.h - Library for HTTP Header/Body processing
   Copyright (c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the GPL 3.0 License and WITHOUT ANY WARRANTY
*/

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
    
    String toString(bool = false);
    bool addLine(String);
    bool set(String);
    bool clear();
    int getSize();
    
  private:
    uint8_t type=HTTP_HEADER;
    char* raw=NULL;
    int size=0;
    
};

#endif
