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

#ifndef MAP_EXCEPTIONS_H_
#define MAP_EXCEPTIONS_H_

#include "edu/sharif/partov/exception/Exception.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

/**
 * Name:              MapNotFoundException
 * Parent:            Exception
 * Description:       Indicate that the map is not exist or it's description is wrong.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Exception Class;
 */
class MapNotFoundException : public edu::sharif::partov::exception::Exception {

  Q_OBJECT

public:
  inline MapNotFoundException (const QString & mesg) :
      Exception (mesg) {
  }
};

/**
 * Name:              MaximumMapInstancesPerUserViolatedException
 * Parent:            Exception
 * Description:       Indicate that another instance of the map is created by current user.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Exception Class;
 */
class MaximumMapInstancesPerUserViolatedException : public edu::sharif::partov::exception::Exception {

  Q_OBJECT

public:
  inline MaximumMapInstancesPerUserViolatedException (const QString & mesg) :
      Exception (mesg) {
  }
};

/**
 * Name:              AuthorizationException
 * Parent:            Exception
 * Description:       Indicate that an unauthorized user or host tried to instantiate
 *                    or resume to simulation of a restricted map.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Exception Class;
 */
class AuthorizationException : public edu::sharif::partov::exception::Exception {

  Q_OBJECT

public:
  inline AuthorizationException (const QString & mesg) :
      Exception (mesg) {
  }
};

/**
 * Name:              NoSuchMapNodeException
 * Parent:            Exception
 * Description:       Indicate that there is no simulated node with given name or
 *                    the node is being simulated by another user.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Exception Class;
 */
class NoSuchMapNodeException : public edu::sharif::partov::exception::Exception {

  Q_OBJECT

public:
  inline NoSuchMapNodeException (const QString & mesg) :
      Exception (mesg) {
  }
};

/**
 * Name:              InvalidInterfaceIndexException
 * Parent:            Exception
 * Description:       Indicate that the interface index is out of range.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Exception Class;
 */
class InvalidInterfaceIndexException : public edu::sharif::partov::exception::Exception {

  Q_OBJECT

public:
  inline InvalidInterfaceIndexException (const QString & mesg) :
      Exception (mesg) {
  }
};

/**
 * Name:              OutOfResourceException
 * Parent:            Exception
 * Description:       Indicate that there is no more resource to allocate a new map instance.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Exception Class;
 */
class OutOfResourceException : public edu::sharif::partov::exception::Exception {

  Q_OBJECT

public:
  inline OutOfResourceException (const QString & mesg) :
      Exception (mesg) {
  }
};

}
}
}
}
}

#endif /* MAP_EXCEPTIONS_H_ */
