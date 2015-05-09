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

#ifndef TRANSITIONVISITOR_H_
#define TRANSITIONVISITOR_H_

#include "ElementVisitor.h"

class QStateMachine;
class QState;
class QAbstractState;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace fsm {

class ExponentiallyTimedState;
}
namespace map {

class Map;

namespace builder {

/**
 * Name:              TransitionVisitor
 * Parent:            ElementVisitor
 * Description:       Visitor for the FSM states transitions subtags.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor;
 */
class TransitionVisitor : public ElementVisitor {

private:
  QStateMachine *fsm;
  edu::sharif::partov::nse::fsm::ExponentiallyTimedState *state;
  bool visitAnyManualTransition;

public:
  TransitionVisitor (Map *_map, QStateMachine *_fsm,
      edu::sharif::partov::nse::fsm::ExponentiallyTimedState *_state);
  virtual ~TransitionVisitor ();

  virtual void processUnnamedElement (QDomElement transitionElement)
  throw (MapFileFormatException *);

private:

  enum TransitionType {

    ExponentiallyTimed,
    Manual
  };

  TransitionType readSingleTransition (const QDomElement &transitionElement) const
  throw (MapFileFormatException *);
  void readSingleManualTransition (const QDomElement &transitionElement,
      QAbstractState *target, QString manualStr) const
  throw (MapFileFormatException *);
  void readSingleCtmcTransition (const QDomElement &transitionElement,
      QAbstractState *target, QString rateStr) const
  throw (MapFileFormatException *);
};

}
}
}
}
}
}

#endif /* TRANSITIONVISITOR_H_ */
