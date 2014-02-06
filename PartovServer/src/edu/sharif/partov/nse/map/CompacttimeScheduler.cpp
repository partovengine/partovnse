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

#include "CompacttimeScheduler.h"

#include <QTimer>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

CompacttimeScheduler::CompacttimeScheduler () :
AbstractScheduler (), clock (0) {
}

CompacttimeScheduler::~CompacttimeScheduler () {
}

void CompacttimeScheduler::scheduleEvent (int eventTimeInMilliSeconds,
    QObject *receiver, const char *invokableMethodName, int eventIdentifier) {
  registerEvent (eventTimeInMilliSeconds * 1000 + getCurrentClockInMicroSeconds (),
                 receiver, invokableMethodName, eventIdentifier);
  QTimer::singleShot (0, this, SLOT (processNextEvent ()));
}

void CompacttimeScheduler::scheduleEventInMicroSeconds (int eventTimeInMicroSeconds,
    QObject *receiver, const char *invokableMethodName, int eventIdentifier) {
  registerEvent (eventTimeInMicroSeconds + getCurrentClockInMicroSeconds (),
                 receiver, invokableMethodName, eventIdentifier);
  QTimer::singleShot (0, this, SLOT (processNextEvent ()));
}

quint64 CompacttimeScheduler::getCurrentClockInMilliSeconds () const {
  return clock / 1000;
}

quint64 CompacttimeScheduler::getCurrentClockInMicroSeconds () const {
  return clock;
}

void CompacttimeScheduler::aboutToProcessAnEventAtTime (quint64 time) {
  clock = time;
}

}
}
}
}
}