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

#include "SignalHandler.h"

#include <iostream>
#include <signal.h>

#include <unistd.h>

#ifdef Q_WS_WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/types.h>
#endif

#include <QSocketNotifier>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

SignalHandler *SignalHandler::me;

int SignalHandler::sigintFd[2];

SignalHandler::SignalHandler () {
  sigintNotifier = new QSocketNotifier (sigintFd[1], QSocketNotifier::Read, this);
  connect (sigintNotifier, SIGNAL (activated (int)), this,
      SLOT (handleInterruptSignal ()));
}

SignalHandler::~SignalHandler () {
}

void SignalHandler::interruptSignalHandler (int signalNumber) {
  Q_UNUSED(signalNumber);
  char dummyValue = 1;
  if (write (sigintFd[0], &dummyValue, sizeof(dummyValue)) < 0) {
    std::cerr << "Can not write to sigint socketpair." << std::endl;
  }
}

void SignalHandler::handleInterruptSignal () {
  sigintNotifier->setEnabled (false);
  std::cout << "-- SIGINT received." << std::endl;

  char dummyValue;
  if (read (sigintFd[1], &dummyValue, sizeof(dummyValue)) < 0) {
    std::cerr << "Can not read from sigint socketpair." << std::endl;
  }

  emit
  interruptSignalTriggered (); /* @@ signal emitted @@ */

  sigintNotifier->setEnabled (true);
}

bool SignalHandler::init () {
  if (socketpair (AF_UNIX, SOCK_STREAM, 0, sigintFd)) {
    qFatal ("--- Panic: Couldn't create SIGINT socketpair; Check POSIX installation.");
    return false;
  }
  me = new SignalHandler ();

  struct sigaction sigintAction;

  sigintAction.sa_handler = SignalHandler::interruptSignalHandler;
  sigemptyset (&sigintAction.sa_mask);
  sigintAction.sa_flags = 0;
  sigintAction.sa_flags |= SA_RESTART;

  if (sigaction (SIGINT, &sigintAction, 0) > 0) {
    delete me;
    return false;
  }
  return true;
}

SignalHandler *SignalHandler::getInstance () {
  return me;
}

void SignalHandler::destroy () {
  delete me;
}

}
}
}
}
}
