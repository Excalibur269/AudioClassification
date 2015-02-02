#ifndef ACTOOLS_H_
#define ACTOOLS_H_

#include <string>
#include "ConfigChecker.h"

string getName(const char *file);
string getSuffix(const char *file);
void setConfigChecker(ConfigChecker &cc);
#endif
