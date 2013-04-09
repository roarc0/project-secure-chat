#ifndef _XML_H_
#define _XML_H_

#include <tinyxml.h>
#include <sstream>

using namespace std;

void   WriteMessage(const char *filname);
string ReadMessage(TiXmlDocument &doc);

#endif
