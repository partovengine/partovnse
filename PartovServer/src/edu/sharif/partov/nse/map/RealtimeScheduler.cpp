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

#include "RealtimeScheduler.h"

#include <QTimer>

#include <sys/time.h>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

RealtimeScheduler::RealtimeScheduler () :
AbstractScheduler () {
}

RealtimeScheduler::~RealtimeScheduler () {
}

void RealtimeScheduler::scheduleEvent (int eventTimeInMilliSeconds,
    QObject *receiver, const char *invokableMethodName, int eventIdentifier) {
  registerEvent (eventTimeInMilliSeconds * 1000 + getCurrentClockInMicroSeconds (),
                 receiver, invokableMethodName, eventIdentifier);
  QTimer::singleShot (eventTimeInMilliSeconds, this, SLOT (processNextEvent ()));
}

void RealtimeScheduler::scheduleEventInMicroSeconds (int eventTimeInMicroSeconds,
    QObject *receiver, const char *invokableMethodName, int eventIdentifier) {
  registerEvent (eventTimeInMicroSeconds + getCurrentClockInMicroSeconds (),
                 receiver, invokableMethodName, eventIdentifier);
  QTimer::singleShot (eventTimeInMicroSeconds / 1000, this, SLOT (processNextEvent ()));
}

quint64 RealtimeScheduler::getCurrentClockInMilliSeconds () const {
  struct timeval currentClock;
  gettimeofday (&currentClock, NULL);
  return currentClock.tv_sec * 1000 + currentClock.tv_usec / 1000;
}

quint64 RealtimeScheduler::getCurrentClockInMicroSeconds () const {
  struct timeval currentClock;
  gettimeofday (&currentClock, NULL);
  return currentClock.tv_sec * 1000 * 1000 + currentClock.tv_usec;
}

}
}
}
}
}
