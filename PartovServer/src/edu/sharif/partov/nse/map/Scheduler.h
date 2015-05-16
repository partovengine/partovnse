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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QObject>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

/**
 * Name:              Scheduler
 * Parent:            QObject
 * Description:       Interface for all scheduling mechanisms.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Interface
 */
class Scheduler : public QObject {

  Q_OBJECT
protected:
  Scheduler () {
  }

public:

  virtual ~Scheduler () {
  }

  /**
   * Schedules a new event to be processed after eventTimeInMilliSeconds milli-seconds
   * later. The given time will be processed according to simulation clock which may
   * differ from wall clock or match with it based on the actual implementation of
   * this interface.
   * When the event occurred, the invokableMethodName method of receiver object, which
   * must be declared using Q_INVOKABLE macro, will be called and the eventIdentifier
   * number will be passed as its first and sole argument. The method's return type
   * must be void.
   * 
   * @param eventTimeInMilliSeconds The event's time in milli-seconds in simulation clock
   * @param receiver The object which event should be delivered to it
   * @param invokableMethodName The method which is invoked to process the event
   * @param eventIdentifier An identifier which may be used by receiver to recognize event
   */
  virtual void scheduleEvent (int eventTimeInMilliSeconds,
      QObject *receiver, const char *invokableMethodName, int eventIdentifier = 0) = 0;

  /**
   * Same as scheduleEvent, but with higher resolution.
   * 
   * @param eventTimeInMicroSeconds The event's time in micro-seconds in simulation clock
   * @param receiver The object which event should be delivered to it
   * @param invokableMethodName The method which is invoked to process the event
   * @param eventIdentifier An identifier which may be used by receiver to recognize event
   */
  virtual void scheduleEventInMicroSeconds (int eventTimeInMicroSeconds,
      QObject *receiver, const char *invokableMethodName, int eventIdentifier = 0) = 0;

  /**
   * Returns the current value of simulation clock in milli-seconds (which may be the same
   * as wall clock or differ from it based on actual implementation of this interface).
   * 
   * @return Current simulation clock's value in milli-seconds
   */
  virtual quint64 getCurrentClockInMilliSeconds () const = 0;

  /**
   * Same as getCurrentClockInMilliSeconds, but with higher resolution.
   * 
   * @return Current simulation clock's value in micro-seconds
   */
  virtual quint64 getCurrentClockInMicroSeconds () const = 0;
};

}
}
}
}
}

#endif	/* Scheduler.h */
