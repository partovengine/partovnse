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

#ifndef REALTIME_SCHEDULER_H
#define REALTIME_SCHEDULER_H

#include "AbstractScheduler.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

/**
 * Name:              RealtimeScheduler
 * Parent:            AbstractScheduler
 * Description:       Realtime scheduler which synchs simulation clock with wall clock.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.map
 * Tags:              None
 */
class RealtimeScheduler : public AbstractScheduler {

  Q_OBJECT
public:
  RealtimeScheduler ();
  virtual ~RealtimeScheduler ();

  virtual void scheduleEvent (int eventTimeInMilliSeconds,
      QObject *receiver, const char *invokableMethodName, int eventIdentifier = 0);
  virtual void scheduleEventInMicroSeconds (int eventTimeInMicroSeconds,
      QObject *receiver, const char *invokableMethodName, int eventIdentifier = 0);

  virtual quint64 getCurrentClockInMilliSeconds () const;
  virtual quint64 getCurrentClockInMicroSeconds () const;
};

}
}
}
}
}

#endif	/* RealtimeScheduler.h */
