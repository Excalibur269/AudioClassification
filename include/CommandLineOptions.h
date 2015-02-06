/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifndef MARSYAS_COMMANDLINEOPTIONS_H
#define MARSYAS_COMMANDLINEOPTIONS_H

#include "common.h" 

#include <vector> 
#include <string> 
#include <map>
#include <iostream> 
#include <stdlib.h>

/** 
\class CommandLineOptions
	\ingroup Notmar
\brief CommandLineOptions

Class for handling command-line options 
*/


class CommandLineOptions
{
protected:
  std::vector<std::string> arguments_;
  std::vector<std::string> remaining_;
  

  std::map<std::string, double> realOptions_;
  std::map<std::string, int> naturalOptions_;
  std::map<std::string, std::string> stringOptions_;
  std::map<std::string, bool> boolOptions_;
  std::map<std::string, std::string> longNames_;
  
  std::map<std::string, std::string>::iterator nameIter_;
  std::map<std::string, double>::iterator riter_;
  std::map<std::string, int>::iterator niter_;
  std::map<std::string, std::string>::iterator siter_;
  std::map<std::string, bool>::iterator biter_;

public:
  CommandLineOptions();
  void addRealOption(std::string lname, std::string sname, double value);
  void addNaturalOption(std::string lname, std::string sname, int value);
  void addStringOption(std::string lname, std::string sname, std::string value);
  void addBoolOption(std::string lname, std::string sname, bool value);

  void readOptions(int argc, const char** argv);
  bool getBoolOption(std::string lname);
  int getNaturalOption(std::string lname);
  double getRealOption(std::string lname);
  std::string getStringOption(std::string lname);
  std::vector<std::string> getRemaining();  
};


#endif


	
