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

#include "ReferenceCounter.h"

#include "SharedObject.h"

#include <QMutex>
#include <QMutexLocker>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

ReferenceCounter::ReferenceCounter () :
    mutex (new QMutex (QMutex::Recursive)), countOfReferences (1) {
}

ReferenceCounter::~ReferenceCounter () {
  delete mutex;
}

void ReferenceCounter::acquireLock () {
  mutex->lock ();
}

void ReferenceCounter::releaseLock () {
  mutex->unlock ();
}

void ReferenceCounter::ref (int count) {
  QMutexLocker lock (mutex);

  countOfReferences += count;
}

void ReferenceCounter::unref (SharedObject *obj) {
  obj->unrefStarted ();
  if (realUnref (obj)) {
    delete this;
  }
}

/**
 * @return true if object should be deleted, false otherwise.
 */
bool ReferenceCounter::realUnref (SharedObject *obj) {
  QMutexLocker locker (mutex);

  if (--countOfReferences == 0) {
    // No other object has any reference to us. So we must be deleted!
    obj->finalize ();
    return true;
  } else {
    obj->unrefIsExiting ();
    return false;
  }
}

SharedObject *ReferenceCounter::detach (SharedObject *obj) {
  if (countOfReferences > 1) {
    QMutexLocker locker (mutex);

    if (countOfReferences > 1) {
      obj->atomicDetachExtensionSlot ();
      SharedObject *res = obj->clone ();
      --countOfReferences;
      return res;
    }
  }
  return 0;
}

int ReferenceCounter::getCountOfReferences () const {
  QMutexLocker locker (mutex);

  return countOfReferences;
}

}
}
}
}
}
