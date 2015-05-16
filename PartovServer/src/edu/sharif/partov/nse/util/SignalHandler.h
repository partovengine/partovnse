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

#ifndef SIGNALHANDLER_H_
#define SIGNALHANDLER_H_

#include <QObject>

class QSocketNotifier;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

/**
 * Name:              SignalHandler
 * Parent:            QObject
 * Description:       SignalHandler is an adapter for POSIX standard library signal handling mechanism.
 *                    This is a singleton class which observes interrupt POSIX signals (OS Independent)
 *                    and emit a Qt signal for each one.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.util
 * Tags:              Adapter; Singleton;
 */
class SignalHandler : public QObject {
Q_OBJECT

private:
  static SignalHandler *me;

  SignalHandler ();
  virtual ~SignalHandler ();

public:
  static void interruptSignalHandler (int signalNumber);

private slots:
  void handleInterruptSignal ();

signals:
  void interruptSignalTriggered ();

private:
  static int sigintFd[2];

  QSocketNotifier *sigintNotifier;

public:
  static bool init ();
  static SignalHandler *getInstance ();
  static void destroy ();
};

}
}
}
}
}

#endif /* SIGNALHANDLER_H_ */
