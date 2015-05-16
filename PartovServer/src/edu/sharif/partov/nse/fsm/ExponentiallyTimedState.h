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

#ifndef EXPONENTIALLY_TIMED_STATE_H_
#define EXPONENTIALLY_TIMED_STATE_H_

#include <QState>

template < class T1, class T2 >
class QPair;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace fsm {

class ActivatableTransition;

/**
 * Name:              ExponentiallyTimedState
 * Parent:            QState
 * Description:       A state which will exit after an exponentially distributed time.
 *                    N outgoing activatable transitions must be given to this state.
 *                    Each transition must be associated with one rate, and after an
 *                    exponentially distributed time (having that rate) the transition
 *                    (with minimum time to be fired) will be enabled/activated and
 *                    then fired.
 *                    BTW, this state can inteoperate with all other states and
 *                    transitions and it does not expect that only exponentially timed
 *                    transitions exist (you can add any other transition to it).
 *                    NOTE: Using removeTransition method will cause undefined behavior.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.fsm
 * Tags:              None
 */
class ExponentiallyTimedState : public QState {

  Q_OBJECT
private:
  typedef QPair < double, ActivatableTransition * > TimedTransition;

  int lastId;
  QList < TimedTransition > transitions;
  ActivatableTransition *enabledTransition;

public:
  ExponentiallyTimedState (QState *parent = 0);
  virtual ~ExponentiallyTimedState ();

  QAbstractTransition *addTransition (double rate, QAbstractState *target);

signals:
  void fired () const;

protected:
  virtual void onEntry (QEvent *event);
  virtual void onExit (QEvent *event);

private:
  Q_INVOKABLE void processDeliveredEvent (int eventIdentifier) const;

  double generateAnExponentiallyDistributedRandomValue (double rate);
};

}
}
}
}
}

#endif /* EXPONENTIALLY_TIMED_STATE_H_ */
