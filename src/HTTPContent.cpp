/*
   HTTPContent.cpp - Library for HTTP Header/Body processing
   Copyright(c) 2022 Felix Kröhnert. All rights reserved.

   This library is distributed under the terms of the MIT License and WITHOUT ANY WARRANTY
*/

#include "HTTPContent.h"

HTTPContent::HTTPContent() {
}
HTTPContent::HTTPContent(HTTPType_t type) {
  if(!(type & ~(HTTP_HEADER|HTTP_BODY)))
    this->type=type;
}

String HTTPContent::toString(bool h) {
  String res = "";
  if(h)
    switch(type) {
      case HTTP_BODY: res+="{\n"; break;
      case HTTP_HEADER: res+="[\n"; break;
      default: res+="unk//\n";
    }
  
  res += String(raw);

  if(h)
    switch(type) {
      case HTTP_BODY: res+="}"; break;
      case HTTP_HEADER: res+="]"; break;
      default: res+="\\unk";
    }

  return res;
}

bool HTTPContent::addLine(String line) {
  int len = line.length();
  size+=len;
  char* tmp = (char*) realloc(raw, size+1);
  if(tmp == NULL) {
    size-=len;
    Serial.println("ERROR ALLOCA");
    return false;
  }
  raw=tmp;
  line.toCharArray(raw+(size-len), len+1);
  return true;
}

bool HTTPContent::set(String info) {
  int len = info.length(), os = size;

  if(len==0) {
    info="\0";
    len=0;
  }
  
  size=len;
  char* tmp = (char*) malloc(size+1);
  if(tmp == NULL) {
    size=os;
    Serial.println("ERROR ALLOCS");
    return false;
  }
  free(raw);
  raw=tmp;
  info.toCharArray(raw, len+1);   //len +1 to force the String class to clone the whole String
  return true;
}

bool HTTPContent::clear() {
  return set("");
}

int HTTPContent::getSize() {
  return size;
}