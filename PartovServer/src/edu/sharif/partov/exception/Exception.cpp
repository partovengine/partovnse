//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *  
 * Copyright © 2009-2015 Behnam Momeni.
 * 
 * This file is part of the Partov.
 * 
 * Partov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Partov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Partov.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */

#include "Exception.h"

#include <QDebug>

#include <execinfo.h>
#include <cstddef>
#include <cxxabi.h>
#include <iostream>

using namespace std;

namespace edu {
namespace sharif {
namespace partov {
namespace exception {

const char * const Exception::RED_COLOR = "\033[1;31m";
const char * const Exception::YELLOW_COLOR = "\033[0;33m";
const char * const Exception::BLUE_COLOR = "\033[0;34m";
const char * const Exception::MAGNET_COLOR = "\033[0;35m";
const char * const Exception::RESET_COLOR = "\033[0m";

Exception::Exception (const QString & mesg) :
message (mesg), backtraceSymbols (fetchBacktraceSymbols ()) {
}

Exception::~Exception () {
  if (backtraceSymbols.first) {
    const void *symbols = reinterpret_cast<const void *> (backtraceSymbols.first);
    free (const_cast<void *> (symbols));
  }
}

const QString & Exception::getExceptionDescription () const {
  return message;
}

void Exception::printExceptionDescription () const {
  cerr << "Exception occurred ("
      << YELLOW_COLOR << this->metaObject ()->className ()
      << RESET_COLOR << "): " << qPrintable (message) << endl;

  printStackBacktrace ();
}

QPair < char * const *, int > Exception::fetchBacktraceSymbols () {
  static const int backtraceMaxSteps = 20;
  void *array[backtraceMaxSteps];
  int size = backtrace (array, backtraceMaxSteps);
  char **symbols = backtrace_symbols (array, size);
  return QPair < char * const *, int > (symbols, size);
}

void Exception::printStackBacktrace () const {
  if (!backtraceSymbols.first) {
    qCritical () << "No stack backtrace is available!";
    return;
  }
  cerr << RESET_COLOR << MAGNET_COLOR << "\x1b[31;1mStack backtrace:"
      << RESET_COLOR << endl;

  // This is required for holding the demangled function name.
  // It must be malloc()ed since used library wishes to realloc() it as required.
  std::size_t functionNameSize = 512;
  char *functionName = (char *) malloc (functionNameSize);

  // two first entries are for fetchBacktraceSymbols and the Exception constructor.
  for (int i = 2; i < backtraceSymbols.second; i++) {
    char * const line = backtraceSymbols.first[i];
    char *begin, *offset, *end;
    if (findFunctionLineParts (line, begin, offset, end)) {
      int status;
      char *name = abi::__cxa_demangle (begin, functionName, &functionNameSize, &status);
      if (status == 0) {
        functionName = name; // use the (possibly) realloc()-ed function name
        cerr << MAGNET_COLOR << line << ":" << RED_COLOR << functionName
            << BLUE_COLOR << "+" << offset << end << RESET_COLOR << endl;
      } else {
        // error while demangling
        cerr << MAGNET_COLOR << line << ":" << RED_COLOR << functionName
            << BLUE_COLOR << "+" << offset << end << RESET_COLOR << endl;
      }
    } else {
      // line can not be divided to more fine grained parts
      cerr << MAGNET_COLOR << line << RESET_COLOR << endl;
    }
  }
  free (functionName);
}

bool Exception::findFunctionLineParts (char *line, char *&begin, char *&offset,
    char *&end) const {
  begin = offset = end = 0;
  // one of the followings must be in line:
  // ./program(mangled_function+0x12345) [0x987654321]
  // ./program() [0x987654321]
  while (*line) {
    switch (*line) {
    case '(':
      *line = 0;
      begin = ++line; // "mangled_function"
      break;

    case '+':
      *line = 0;
      offset = ++line; // "0x12345"
      break;

    case ')':
      *line = 0;
      end = ++line; // " [0x987654321]"
      return begin && offset && end && begin < offset && offset < end;

    default:
      ++line;
      break;
    }
  }
  return false;
}

}
}
}
}
