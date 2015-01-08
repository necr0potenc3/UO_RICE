////////////////////////////////////////////////////////////////////////////////
//
// Mini_flex.h
//
// Copyright (C) 2001 Nikolaus "Lord Binary" Gerstmayr
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////


#ifndef __mini_flex
#define __mini_flex

#include <iostream>
#include <vector>
#include <string.h>
#include <iterator>

using namespace std;

// #define debug

enum PT { any_1, specific_1, any_n }; // pattern type

struct single_char
{
   unsigned char value;
   PT type;
   long int num_anys; // somewhat wastative because it's only required for type any_n
   bool neg;
};


struct single_pos
{
	vector <single_char> or_list; // all values a single position can have to match
                                 // more than one == logical or
};

struct single_pattern
{
   vector <single_pos> and_list; // all positions (== logical ands)
};

class patterns
{
  private: 

  single_pattern *pat;
  unsigned long int start;
  unsigned long int end; // 0 == EOF (==filelength)
  unsigned char *filebuffer;
  unsigned long int file_length;
  unsigned long int relative_offset;
  int errorcode;

  void free_mem();

  bool isSexyChar(char* candidate); // helper helper method for parsing / read_pattern
  int get_single(char* inp, char  &out  ); // helper method for parsing
  int get_any_ns(char* inp, int &out1, char &out2, bool &nv); // helper method for parsing
 
  public:

  void set_file(const char* filename);
  void read_pattern(char *patternstring);           
  inline int get_error()                           { return errorcode; }  
  unsigned long int find_it();
  int copy_filebuffer(unsigned long int &fl, unsigned char * buffer)
  {	  
	  fl = file_length;
	  memmove(buffer, filebuffer, file_length);
	  return 1;
  }

  patterns() :start(0), end(0), file_length(0), filebuffer(NULL), relative_offset(0), pat(NULL) {  }
  patterns(char* ps) :start(0), end(0), file_length(0), filebuffer(NULL), pat(NULL) { read_pattern(ps);  }  
  virtual ~patterns();

};

#endif
