#ifndef _XML_H_
#define _XML_H_

#include <tinyxml.h>
#include <sstream>
#include <string>
#include "common.h"

using namespace std;

string XMLBuildMessage(const char *, const char *);
void   XMLReadMessage(const char *, string&, string&);
std::string XMLBuildUpdate(const char *, const char*);
void XMLReadUpdate(const char *str, string& name, string& status);

#endif
