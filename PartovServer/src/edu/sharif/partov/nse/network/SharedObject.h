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

#ifndef SHAREDOBJECT_H_
#define SHAREDOBJECT_H_

#include <QObject>

#include "edu/sharif/partov/exception/Exception.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              NonCloneableException
 * Parent:            Exception
 * Description:       Indicate that the SharedObject is not cloneable.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Exception Class;
 */
class NonCloneableException : public edu::sharif::partov::exception::Exception {

  Q_OBJECT

public:
  inline NonCloneableException (const QString & mesg) :
      Exception (mesg) {
  }
};

/**
 * Name:              SharedObject
 * Parent:            None
 * Description:       The Interface for shared object which its references can be counted
 *                    by ReferenceCounter Class.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
class SharedObject {

public:
  SharedObject ();
  virtual ~SharedObject ();

  virtual void unrefStarted () = 0;
  virtual void unrefIsExiting () = 0;
  virtual void atomicDetachExtensionSlot () = 0;
  virtual void finalize ();
  virtual SharedObject *clone () const throw (NonCloneableException *) = 0;
};

}
}
}
}
}

#endif /* SHAREDOBJECT_H_ */
