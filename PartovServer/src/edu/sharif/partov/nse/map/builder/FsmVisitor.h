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

#ifndef FSMVISITOR_H_
#define FSMVISITOR_H_

#include "NamedElementTwoPassVisitor.h"

class QStateMachine;
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
 * Name:              FsmVisitor
 * Parent:            NamedElementTwoPassVisitor
 * Description:       Visitor for the FSM tag.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor;
 */
class FsmVisitor : public NamedElementTwoPassVisitor {
private:
  QStateMachine *fsm;

public:
  FsmVisitor (Map *_map, QStateMachine *_fsm);
  virtual ~FsmVisitor ();

  virtual void processNamedElementFirstPass (QString stateName,
      QDomElement stateElement) throw ();
  virtual void processNamedElementSecondPass (QString stateName, QDomElement stateElement)
      throw (MapFileFormatException *);

  virtual void setFsmInitialStateAndStartIt (const QDomElement &fsmElement)
      throw (MapFileFormatException *);

private:
  void readStateDetails (QStateMachine *fsm,
      edu::sharif::partov::nse::fsm::ExponentiallyTimedState *state,
      const QDomElement &stateElement) throw (MapFileFormatException *);
  void readStateTransitionsDetails (QStateMachine *fsm,
      edu::sharif::partov::nse::fsm::ExponentiallyTimedState *state,
      const QDomElement &transitions) const throw (MapFileFormatException *);
  void readStateActivityDetails (QAbstractState *state, const QDomElement &activity)
      throw (MapFileFormatException *);
};

}
}
}
}
}
}

#endif /* FSMVISITOR_H_ */
