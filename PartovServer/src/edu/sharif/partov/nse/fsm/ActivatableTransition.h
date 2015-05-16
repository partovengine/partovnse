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

#ifndef ACTIVATABLE_TRANSITION_H_
#define ACTIVATABLE_TRANSITION_H_

#include <QSignalTransition>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace fsm {

/**
 * Name:              ActivatableTransition
 * Parent:            QSignalTransition
 * Description:       A transition that can be (de)activated before firing.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.fsm
 * Tags:              None
 */
class ActivatableTransition : public QSignalTransition {

  Q_OBJECT
private:
  bool activated;

public:
  ActivatableTransition (QObject *sender, const char *signal, QState *sourceState = 0);
  virtual ~ActivatableTransition ();

  void activate ();
  void deactivate ();

protected:
  virtual bool eventTest (QEvent *event);
};

}
}
}
}
}

#endif /* ACTIVATABLE_TRANSITION_H_ */
