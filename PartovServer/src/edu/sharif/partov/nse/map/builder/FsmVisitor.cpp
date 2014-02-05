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

#include "FsmVisitor.h"

#include "MapReader.h"
#include "TransitionVisitor.h"

#include "edu/sharif/partov/nse/map/Map.h"
#include "edu/sharif/partov/nse/map/link/Link.h"

#include "edu/sharif/partov/nse/fsm/ExponentiallyTimedState.h"

#include <QDomElement>
#include <QStateMachine>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

FsmVisitor::FsmVisitor (Map *_map, QStateMachine *_fsm) :
NamedElementTwoPassVisitor (_map), fsm (_fsm) {
}

FsmVisitor::~FsmVisitor () {
}

void FsmVisitor::processNamedElementFirstPass (QString stateName,
    QDomElement stateElement) throw () {
  Q_UNUSED (stateElement);
  edu::sharif::partov::nse::fsm::ExponentiallyTimedState *state =
      new edu::sharif::partov::nse::fsm::ExponentiallyTimedState (fsm);
  state->setObjectName (stateName);
}

void FsmVisitor::processNamedElementSecondPass (QString stateName,
    QDomElement stateElement) throw (MapFileFormatException *) {
  edu::sharif::partov::nse::fsm::ExponentiallyTimedState *state = fsm->findChild <
      edu::sharif::partov::nse::fsm::ExponentiallyTimedState * > (stateName);
  readStateDetails (fsm, state, stateElement);
}

void FsmVisitor::readStateDetails (QStateMachine *fsm,
    edu::sharif::partov::nse::fsm::ExponentiallyTimedState *state,
    const QDomElement &stateElement) throw (MapFileFormatException *) {
  QDomElement transitions = stateElement.firstChildElement ("transitions");
  QDomElement activity = stateElement.firstChildElement ("activity");
  int size = stateElement.childNodes ().size ();
  switch (size) {
  case 0:
    break;

  case 1:
    if (!transitions.isNull ()) {
      readStateTransitionsDetails (fsm, state, transitions);
    } else if (!activity.isNull ()) {
      readStateActivityDetails (state, activity);
    } else {
      throw new MapFileFormatException (stateElement, "Malformed FSM children tags");
    }
    break;

  case 2:
    if (transitions.isNull () || activity.isNull ()) {
      throw new MapFileFormatException (stateElement, "Malformed FSM children tags");
    }
    readStateTransitionsDetails (fsm, state, transitions);
    readStateActivityDetails (state, activity);
    break;

  default:
    throw new MapFileFormatException
        (stateElement, "Unexpected number of FSM children tags");
  }
}

void FsmVisitor::readStateTransitionsDetails (QStateMachine *fsm,
    edu::sharif::partov::nse::fsm::ExponentiallyTimedState *state,
    const QDomElement &transitions) const throw (MapFileFormatException *) {
  if (transitions.childNodes ().size () <= 0) {
    qWarning ("You can remove transitions tag when there is no transition within it.");
    return;
  }

  TransitionVisitor visitor (map, fsm, state);
  MapReader ().readUnnamedElements (transitions, "transition", &visitor);
}

void FsmVisitor::readStateActivityDetails (QAbstractState *state,
    const QDomElement &activity) throw (MapFileFormatException *) {

  class ActivityVisitor : public ElementVisitor {

   private:
    QAbstractState *state;

   public:

    ActivityVisitor (Map *_map, QAbstractState *_state) :
        ElementVisitor (_map), state (_state) {
    }

    virtual void processUnnamedElement (QDomElement doElement)
    throw (MapFileFormatException *) {
      QString command = doElement.attribute ("command");
      QString arg0 = doElement.attribute ("arg0");
      if (command.isNull () || arg0.isNull ()) {
        throw new MapFileFormatException
            (doElement, "Missing command and/or arg0 parameters");
      }
      edu::sharif::partov::nse::map::link::Link *link = map->findChild <
          edu::sharif::partov::nse::map::link::Link * > (arg0);
      if (!link) {
        throw new MapFileFormatException (doElement, "Referenced link can not be found");
      }
      if (command == "link-up") {
        QObject::connect (state, SIGNAL (entered ()), link, SLOT (turnLinkUp ()));
      } else if (command == "link-down") {
        QObject::connect (state, SIGNAL (entered ()), link, SLOT (turnLinkDown ()));
      } else {
        throw new MapFileFormatException (doElement, "Malformed command parameter");
      }
    }
  } visitor (map, state);
  MapReader ().readUnnamedElements (activity, "do", &visitor);
}

void FsmVisitor::setFsmInitialStateAndStartIt (const QDomElement &fsmElement)
throw (MapFileFormatException *) {
  QString initialStateName = fsmElement.attribute ("initial-state");
  if (initialStateName.isNull ()) {
    throw new MapFileFormatException (fsmElement, "Missing initial-state parameter");
  }
  QAbstractState *initialState = fsm->findChild < QAbstractState * > (initialStateName);
  if (initialState == NULL) {
    throw new MapFileFormatException
        (fsmElement, "Specified initial state can not be found");
  }
  fsm->setInitialState (initialState);

  fsm->start ();
}

}
}
}
}
}
}
