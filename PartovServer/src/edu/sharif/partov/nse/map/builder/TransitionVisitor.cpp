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

#include "TransitionVisitor.h"

#include "edu/sharif/partov/nse/fsm/ExponentiallyTimedState.h"

#include "edu/sharif/partov/nse/map/Map.h"

#include <QStateMachine>
#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

TransitionVisitor::TransitionVisitor (Map *_map, QStateMachine *_fsm,
    edu::sharif::partov::nse::fsm::ExponentiallyTimedState *_state) :
    ElementVisitor (_map), fsm (_fsm), state (_state),
    visitAnyManualTransition (false) {
}

TransitionVisitor::~TransitionVisitor () {
}

void TransitionVisitor::processUnnamedElement (QDomElement transitionElement)
throw (MapFileFormatException *) {
  if (readSingleTransition (transitionElement) == Manual) {
    visitAnyManualTransition = true;
  }
}

TransitionVisitor::TransitionType TransitionVisitor::readSingleTransition (
    const QDomElement &transitionElement) const
throw (MapFileFormatException *) {
  QString rateStr = transitionElement.attribute ("rate");
  QString manualStr = transitionElement.attribute ("manual");
  if (rateStr.isNull () && manualStr.isNull ()) {
    throw new MapFileFormatException (transitionElement,
                                      "Missing -rate | manual- parameters");
  } else if (!rateStr.isNull () && !manualStr.isNull ()) {
    throw new MapFileFormatException
        (transitionElement, "Only one of -rate | manual- parameters can be provided");
  }
  QString targetName = transitionElement.attribute ("target");
  if (targetName.isNull ()) {
    throw new MapFileFormatException (transitionElement, "Missing target parameter");
  }
  QAbstractState *target = fsm->findChild < QAbstractState * > (targetName);
  if (!target) {
    throw new MapFileFormatException (transitionElement,
                                      "Referenced target state can not be found");
  }
  if (!manualStr.isNull ()) {
    readSingleManualTransition (transitionElement, target, manualStr);
    return Manual;
  } else {
    readSingleCtmcTransition (transitionElement, target, rateStr);
    return ExponentiallyTimed;
  }
}

void TransitionVisitor::readSingleManualTransition (const QDomElement &transitionElement,
    QAbstractState *target, QString manualStr) const
throw (MapFileFormatException *) {
  if (manualStr != "true") {
    throw new MapFileFormatException (transitionElement, "Malformed -manual- value");
  }
  if (visitAnyManualTransition) {
    throw new MapFileFormatException
        (transitionElement, "More than one manual transition per state");
  }
  QState *source = state;
  source->addTransition (map, SIGNAL (walk ()), target);
}

void TransitionVisitor::readSingleCtmcTransition (const QDomElement &transitionElement,
    QAbstractState *target, QString rateStr) const
throw (MapFileFormatException *) {
  bool ok = false;
  double rate = rateStr.toDouble (&ok);
  if (!ok) {
    throw new MapFileFormatException (transitionElement, "Malformed -rate- value");
  }
  state->addTransition (rate, target);
}

}
}
}
}
}
}
