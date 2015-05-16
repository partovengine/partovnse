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

#include "Formula.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

Formula *Formula::parseFormula (const char *str, char separator, int countOfParts,
    bool hexMode) throw (ParsingException *) {
  Formula *fm = new Formula[countOfParts];
  int i = 0;
  fm[i].a = 0;
  bool waitingForSeparator = false, waitingForFormula = true, completed = false;
  const int multiplier = (hexMode ? 16 : 10);
  // waitingForFormula => ~waitingForDot
  while (char c = *str++) {
    if ('a' <= c && c <= 'f') {
      c += 'A' - 'a';
    }
    if (('0' <= c && c <= '9') || (hexMode && 'A' <= c && c <= 'F')) {
      fm[i].mode = Formula::Constant;
      if ('A' <= c && c <= 'F') {
        fm[i].a = fm[i].a * multiplier + (c - 'A' + 10);
      } else {
        fm[i].a = fm[i].a * multiplier + (c - '0');
      }
      if (fm[i].a > 255) {
        delete[] fm;
        throw new ParsingException ("Byte overflow occurred");
      }
      waitingForSeparator = (i < countOfParts - 1);
      waitingForFormula = false;
      completed = (i == (countOfParts - 1));

    } else if (c == separator && waitingForSeparator) {
      i++;
      fm[i].a = 0;
      waitingForSeparator = false;
      waitingForFormula = true;

    } else if (c == '{' && waitingForFormula) {
      try {
        fm[i] = parseFormula (str);
      } catch (ParsingException *e) {
        delete[] fm;
        throw e;
      }
      fm[i].mode = Formula::Linear;
      if (i < countOfParts - 1) {
        if (*str++ == separator) {
          fm[++i].a = 0;
          continue;
        }
      } else {
        completed = ((*str++) == '\0'); // string is finished
      }
      break;

    } else {
      break;
    }
  }
  if (!completed) {
    delete[] fm;
    throw new ParsingException ("Malformed formula");
  }
  return fm;
}

/**
 * It's guaranteed on return that str is pointing to one after
 * the '}' character (it may point to null or anything else).
 */
Formula Formula::parseFormula (const char *&str) throw (ParsingException *) {
  Formula fm;
  int num = 0;
  bool waitingForPlus = false, waitingForI = false, bDigitsAreSeen = false;
  while (char c = *str++) { // str is like a+bi} where b is an integer and b >= 1 (missing b means 1 too)
    if ('0' <= c && c <= '9') {
      num = num * 10 + (c - '0');
      if (num > 255) {
        break;
      }
      waitingForPlus = !waitingForI;
      if (waitingForI) {
        bDigitsAreSeen = true;
      }

    } else if (c == '+' && waitingForPlus) {
      fm.a = num;
      num = 0;
      waitingForI = true;
      waitingForPlus = false;

    } else if (c == 'i' && waitingForI) {
      if (bDigitsAreSeen) {
        if (num == 0) {
          break;
        }
      } else {
        num = 1;
      }
      fm.b = num;
      if (*str++ == '}') {
        return fm;
      } else {
        break;
      }

    } else {
      break;
    }
  }
  throw new ParsingException ("Malformed formula");
}

int Formula::resolve (int i) const {
  if (mode == Constant) {
    return a;
  } else { //if (mode == Linear) {
    return a + b * i;
  }
}

}
}
}
}
}
}
