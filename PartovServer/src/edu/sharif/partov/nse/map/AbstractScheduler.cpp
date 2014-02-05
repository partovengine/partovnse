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

#include "AbstractScheduler.h"

#include <QMultiMap>
#include <QCoreApplication>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

AbstractScheduler::AbstractScheduler () :
Scheduler (),
events (new QMultiMap < quint64, Schedulee > ()) {
}

AbstractScheduler::~AbstractScheduler () {
  events->clear ();
  delete events;
}

void AbstractScheduler::registerEvent (quint64 eventAbsoluteTimeInMicroSeconds,
    QObject *receiver, const char *invokableMethodName, int eventIdentifier) {
  events->insert (eventAbsoluteTimeInMicroSeconds,
                  Schedulee (receiver, invokableMethodName, eventIdentifier));
}

void AbstractScheduler::processNextEvent () {
  Q_ASSERT (!events->isEmpty ());
  QMultiMap < quint64, Schedulee >::iterator it = events->begin ();
  aboutToProcessAnEventAtTime (it.key ());
  it->deliverEvent ();
  events->erase (it);
}

void AbstractScheduler::aboutToProcessAnEventAtTime (quint64 time) {
  /*
   * By this point, the scheduling time should be advanced till given time
   * As an abstract scheduler, this class should delegate (to its children) decision
   * about how the clock should be advanced.
   */
  Q_UNUSED (time);
}

AbstractScheduler::Schedulee::Schedulee (QObject *_receiver,
    const char *_invokableMethodName, int _eventIdentifier) :
receiver (_receiver), invokableMethodName (_invokableMethodName),
eventIdentifier (_eventIdentifier) {
}

void AbstractScheduler::Schedulee::deliverEvent () const {
  if (!QMetaObject::invokeMethod (receiver, invokableMethodName,
                                  Qt::DirectConnection, Q_ARG (int, eventIdentifier))) {
    qFatal ("--- Panic: Error while invoking method ``%s''", invokableMethodName);
    QCoreApplication::exit (-40);
  }
}

}
}
}
}
}
