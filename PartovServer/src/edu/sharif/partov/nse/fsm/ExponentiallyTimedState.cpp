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

#include "ExponentiallyTimedState.h"

#include "ActivatableTransition.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"

#include <QPair>

#include <QtCore/qmath.h>
#include <cstdlib>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace fsm {

ExponentiallyTimedState::ExponentiallyTimedState (QState *parent) :
QState (parent), lastId (0), enabledTransition (0) {
}

ExponentiallyTimedState::~ExponentiallyTimedState () {
}

QAbstractTransition *ExponentiallyTimedState::addTransition (double rate,
    QAbstractState *target) {
  if (rate <= 0) {
    qWarning ("The firing rate must be positive");
    return 0;
  }
  if (!target) {
    qWarning ("Transition's target state can not be null");
    return 0;
  }
  ActivatableTransition *at = new ActivatableTransition (this, SIGNAL (fired ()));
  at->setTargetState (target);
  QState::addTransition (at);

  transitions.append (TimedTransition (rate, at));

  return at;
}

void ExponentiallyTimedState::onEntry (QEvent *event) {
  if (!transitions.isEmpty ()) {
    double min;
    enabledTransition = 0;

    foreach (TimedTransition trans, transitions) {
      double val = generateAnExponentiallyDistributedRandomValue (trans.first);
      if (!enabledTransition || min > val) {
        min = val;
        enabledTransition = trans.second;
      }
    }
    enabledTransition->activate ();
    edu::sharif::partov::nse::map::MapFactory::getInstance ()->getMapEventScheduler ()
        ->scheduleEvent (min, this, "processDeliveredEvent", lastId);
  }
  QState::onEntry (event);
}

void ExponentiallyTimedState::processDeliveredEvent (int eventIdentifier) const {
  if (eventIdentifier < lastId) {
    return;
  }
  emit fired (); /* @@ signal emitted @@ */
}

void ExponentiallyTimedState::onExit (QEvent *event) {
  lastId++;
  if (enabledTransition) {
    enabledTransition->deactivate ();
    enabledTransition = 0;
  }
  QState::onExit (event);
}

double ExponentiallyTimedState::generateAnExponentiallyDistributedRandomValue (
    double rate) {
  double uniformRandomValue = qrand ();
  if (0 < uniformRandomValue && uniformRandomValue < RAND_MAX) {
    return -qLn (uniformRandomValue / RAND_MAX) / rate;
  } else {
    return generateAnExponentiallyDistributedRandomValue (rate);
  }
}

}
}
}
}
}
