#ifndef _XML_H_
#define _XML_H_

#include <tinyxml.h>
#include <sstream>
#include <string>

using namespace std;

class XmlMessage
{

  public:
    XmlMessage();
    ~XmlMessage();

    string BuildMessage(const char *, const char *);
    string ReadMessage(const char *);
};

#endif
