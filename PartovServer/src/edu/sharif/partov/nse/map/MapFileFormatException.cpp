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

#include "MapFileFormatException.h"

#include <QDomNode>
#include <QDebug>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

MapFileFormatException::MapFileFormatException (const QDomNode &node,
    const QString & mesg) :
Exception (
QString ("Error at line %4 %1 %5, column %4 %2 %5: %3").arg (node.lineNumber ()).arg (
node.columnNumber ()).arg (mesg).arg (BLUE_COLOR).arg (RESET_COLOR)), cause (
0) {
}

MapFileFormatException::MapFileFormatException (const QDomNode &node,
    const QString & mesg, edu::sharif::partov::exception::Exception *_cause) :
Exception (
QString ("Error at line %4 %1 %5, column %4 %2 %5: %3").arg (node.lineNumber ()).arg (
node.columnNumber ()).arg (mesg).arg (BLUE_COLOR).arg (RESET_COLOR)), cause (
_cause) {
}

MapFileFormatException::~MapFileFormatException () {
  if (cause) {
    delete cause;
  }
}

void MapFileFormatException::printExceptionDescription () const {
  Exception::printExceptionDescription ();
  if (cause) {
    qCritical () << "Caused by another exception as follows:";
    cause->printExceptionDescription ();
  }
}

}
}
}
}
}
