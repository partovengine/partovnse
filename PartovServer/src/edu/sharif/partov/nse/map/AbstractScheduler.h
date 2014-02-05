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

#ifndef ABSTRACT_SCHEDULER_H
#define ABSTRACT_SCHEDULER_H

#include "Scheduler.h"

template <typename Key, typename T>
class QMultiMap;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

/**
 * Name:              AbstractScheduler
 * Parent:            Scheduler
 * Description:       An abstract implementation for scheduling (e.g. realtime, etc.)
 *                    mechanisms.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Abstract; SingletonPerMap
 */
class AbstractScheduler : public Scheduler {

  Q_OBJECT
private:
  class Schedulee {

  private:
    QObject *receiver;
    const char *invokableMethodName;
    int eventIdentifier;

  public:
    Schedulee (QObject *_receiver, const char *_invokableMethodName,
        int _eventIdentifier);

    void deliverEvent () const;
  };

  QMultiMap < quint64, Schedulee > *events;

protected:
  AbstractScheduler ();

public:
  virtual ~AbstractScheduler ();

protected:
  void registerEvent (quint64 eventAbsoluteTimeInMicroSeconds,
      QObject *receiver, const char *invokableMethodName, int eventIdentifier);

  virtual void aboutToProcessAnEventAtTime (quint64 time);

protected slots:
  void processNextEvent ();
};

}
}
}
}
}

#endif	/* AbstractScheduler.h */
