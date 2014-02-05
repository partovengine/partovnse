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

#ifndef MAPFILEFORMATEXCEPTION_H_
#define MAPFILEFORMATEXCEPTION_H_

#include "edu/sharif/partov/exception/Exception.h"

class QDomNode;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

/**
 * Name:              MapFileFormatException
 * Parent:            Exception
 * Description:       Indicate an error in the format of the XML map file by its line/column.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Exception Class;
 */
class MapFileFormatException : public edu::sharif::partov::exception::Exception {
Q_OBJECT
private:
  edu::sharif::partov::exception::Exception *cause;

public:
  MapFileFormatException (const QDomNode &node, const QString & mesg);
  MapFileFormatException (const QDomNode &node, const QString & mesg,
      edu::sharif::partov::exception::Exception *_cause);

  virtual ~MapFileFormatException ();

  virtual void printExceptionDescription () const;
};

}
}
}
}
}

#endif /* MAPFILEFORMATEXCEPTION_H_ */
