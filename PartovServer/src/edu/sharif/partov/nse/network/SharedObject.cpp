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

#include "SharedObject.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

SharedObject::SharedObject () {
}

SharedObject::~SharedObject () {
}

/**
 * Every shared object must finalize its state and
 * release every resource which is required to be
 * freed. This method must delete the object too.
 * After return we think about this object as deleted.
 */
void SharedObject::finalize () {
  unrefIsExiting ();
  delete this;
}

}
}
}
}
}
