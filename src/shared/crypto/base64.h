#ifndef _BASE64_H_
#define _BASE64_H_

#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>

std::string EncodeBase64(const ::std::string &);
std::string DecodeBase64(const ::std::string &);

#endif
