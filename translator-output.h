// -*- C++ -*-
// Copyright (C) 2005, 2009, 2014 Red Hat Inc.
//
// This file is part of systemtap, and is free software.  You can
// redistribute it and/or modify it under the terms of the GNU General
// Public License (GPL); either version 2, or (at your option) any
// later version.

#ifndef TRANSLATOR_OUTPUT_H
#define TRANSLATOR_OUTPUT_H

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

// Output context for systemtap translation, intended to allow
// pretty-printing.
class translator_output
{
  char *buf;
  std::ofstream* o2;
  std::ostream& o;
  unsigned tablevel;

public:
  std::string filename;
  bool trailer_p; // is this file to be linked before or after main generated source file

  translator_output* hdr;  /* for stap_common.h file */

  translator_output (std::ostream& file);
  translator_output (const std::string& filename, size_t bufsize = 8192);
  ~translator_output ();

  void new_common_header (std::ostream& file);
  void new_common_header (const std::string& filename, size_t bufsize = 8192);

  void close ();
  
  std::ostream& newline (int indent = 0);
  void indent (int indent = 0);

  // NB: don't bother assert upon tablevel != 0.  Some pass-3 exceptions
  // can be thrown that bypass o->indent() cleanups, which can cause
  // these failures.
  // At the worst, the generated C code will be uncompilable.  But that's
  // OK, we will have printed an error message already, so -p4 won't be
  // attempted.
  void assert_0_indent () { o << std::flush; }
  
  std::ostream& line();

  std::ostream::pos_type tellp() { return o.tellp(); }
  std::ostream& seekp(std::ostream::pos_type p) { return o.seekp(p); }
};

#endif

/* vim: set sw=2 ts=8 cino=>4,n-2,{2,^-2,t0,(0,u0,w1,M1 : */
