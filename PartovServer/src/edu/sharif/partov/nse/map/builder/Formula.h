//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *  
 * Copyright © 2009-2014 Behnam Momeni.
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

#ifndef FORMULA_H_
#define FORMULA_H_

#include "edu/sharif/partov/exception/Exception.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

/**
 * Name:              ParsingException
 * Parent:            Exception
 * Description:       Indicate an error during parsing a formula
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Exception Class;
 */
class ParsingException : public edu::sharif::partov::exception::Exception {
Q_OBJECT
public:
  ParsingException (const QString & mesg) :
      Exception (mesg) {
  }
};

/**
 *  Each formula is like {a+bi} such that a and b are integers (i is a symbol as is)
 */
class Formula {
private:
  enum {
    Constant, Linear
  } mode;
  int a;
  int b;

  Formula () {
  }

public:
  ~Formula () {
  }

  /**
   * The returned array will be newed and caller must delete it.
   */
  static Formula *parseFormula (const char *str, char separator, int countOfParts,
      bool hexMode) throw (ParsingException *);

  int resolve (int i) const;

private:
  static Formula parseFormula (const char *&str) throw (ParsingException *);
};

}
}
}
}
}
}

#endif /* FORMULA_H_ */
