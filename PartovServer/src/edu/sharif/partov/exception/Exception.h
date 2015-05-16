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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <QObject>

#include <QPair>

class QString;

namespace edu {
namespace sharif {
namespace partov {
namespace exception {

/**
 * Name:              Exception
 * Parent:            QObject
 * Description:       Parent of all exceptions introduced from 'edu.sharif.partov.nse' package.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.exception
 * Tags:              Exception Class;
 */
class Exception : public QObject {
Q_OBJECT

private:
  const QString message;
  const QPair < char * const *, int > backtraceSymbols;

public:
  Exception (const QString & mesg);
  virtual ~Exception ();

  const QString & getExceptionDescription () const;
  virtual void printExceptionDescription () const;

  void printStackBacktrace () const;

private:
  static QPair < char * const *, int > fetchBacktraceSymbols ();
  bool findFunctionLineParts (char *line, char *&begin, char *&offset, char *&end) const;

protected:
  static const char * const RED_COLOR;
  static const char * const YELLOW_COLOR;
  static const char * const BLUE_COLOR;
  static const char * const MAGNET_COLOR;
  static const char * const RESET_COLOR;
};

}
}
}
}

#endif /* EXCEPTION_H_ */
