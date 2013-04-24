#ifndef _XML_H_
#define _XML_H_

#include <tinyxml.h>
#include <sstream>
#include <string>

using namespace std;

string XMLBuildMessage(const char *, const char *);
void   XMLReadMessage(const char *, string&, string&);

#endif
