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

#include "NonBlockingLocker.h"

#include <QCoreApplication>
#include <QMutex>
#include <QSemaphore>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

NonBlockingLocker::NonBlockingLocker (QMutex *m, bool waitForLocking) :
mutex (m), semaphore (NULL), locked (false), step (1) {
  relock (waitForLocking);
}

NonBlockingLocker::NonBlockingLocker (QSemaphore *s, int _step,
    bool waitForLocking) :
mutex (NULL), semaphore (s), locked (false), step (_step) {
  relock (waitForLocking);
}

void NonBlockingLocker::unlock () {
  if (locked) {
    if (mutex) {
      mutex->unlock ();
    } else {
      semaphore->release (step);
    }
    locked = false;
  }
}

void NonBlockingLocker::relock (bool waitForLocking) {
  if (!locked) {
    if (mutex) {
      while (!mutex->tryLock ()) {
        if (waitForLocking) {
          QCoreApplication::processEvents ();
        } else {
          return;
        }
      }
    } else {
      while (!semaphore->tryAcquire (step)) {
        if (waitForLocking) {
          QCoreApplication::processEvents ();
        } else {
          return;
        }
      }
    }
    locked = true;
  }
}

}
}
}
}
}
