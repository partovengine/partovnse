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

#ifndef NON_BLOCKING_LOCKER_H_
#define NON_BLOCKING_LOCKER_H_

#include <Qt>

class QMutex;
class QSemaphore;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

/**
 * Name:              NonBlockingLocker
 * Parent:            None
 * Description:       A mutex locker which locks/unlocks mutex in its
 *                    constructor/deconstructor and calls the processEvents
 *                    instead of blocking.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.util
 * Tags:              None
 */
class NonBlockingLocker {
private:

  Q_DISABLE_COPY (NonBlockingLocker);

  QMutex * const mutex;
  QSemaphore * const semaphore;
  bool locked;
  const int step;

public:

  explicit NonBlockingLocker (QMutex *m, bool waitForLocking = true);
  explicit NonBlockingLocker (QSemaphore *s, int _step = 1,
      bool waitForLocking = true);

  inline ~NonBlockingLocker () {
    unlock ();
  }

  void unlock ();

  /**
   * Locks the mutex or acquires step resources out of the semaphore.
   * If it cannot get the lock, it calls processEvents instead of blocking
   * the thread.
   *
   * WARNING: If non-blocking locker is working with a non-recursive mutex
   * (recursive mutex is not supported) and the caller has already acquired
   * some locks,
   * when the processEvents allows other slots from the same thread to run,
   * those slots may look after locks of the caller too.
   * This situation, i.e. a thread looking for non-recursive locks which are
   * acquired by itself, leads to a dead-lock.
   * Use the `waitForLocking = false` to avoid dead-lock.
   *
   * @param waitForLocking if false, returns without locking if immediate
   * locking is not possible
   */
  void relock (bool waitForLocking);

  bool isLocked () const {
    return locked;
  }
};

}
}
}
}
}

#endif /* NON_BLOCKING_LOCKER_H_ */
