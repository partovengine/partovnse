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

#include "Server.h"

#include "Simulator.h"

#include "edu/sharif/partov/nse/usermanagement/UserAuthenticator.h"

#include <QCoreApplication>
#include <QTcpServer>
#include <QList>
#include <QDebug>
#include <QTimer>
#include <QSettings>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace server {

bool Server::verbose;

Server::Server () :
shuttingDown (false) {
  // create server in current (main) thread...
  simulators = new QList < Simulator * > ();
}

Server::~Server () {
  // FIXME: We should do finalization sooner (for example through the QCoreApplication::aboutToQuit () signal).
  QDebug log = qDebug ();
  log << "Shutting down server.......";
  shuttingDown = true;

  foreach (Simulator *sim, *simulators) {
    delete sim;
  }
  delete simulators;

  log << " [Done] " << endl << "Please wait a minute for last finalizing operations..."
      << endl;
}

void Server::listenForConnections (void) {
  QDebug log = qDebug ();
  log << "Starting up server.........";
  QSettings config ("../config/config.ini", QSettings::IniFormat);
  config.beginGroup ("server");
  bool ok;
  int port = config.value ("port", 7890).toInt (&ok);
  if (!ok) {
    log << " [Failed]\n"
        "Port is not configured correctly.\n";
    QCoreApplication::quit ();
  }
  int timeoutInSeconds = config.value ("timeout-seconds", 0).toInt (&ok);
  if (!ok) {
    log << " [Failed]\n"
        "Server timeout value is not configured correctly"
        " (an integer indicating timeout in seconds is expected).\n";
    QCoreApplication::quit ();
  }
  config.endGroup ();

  tcpServer = new QTcpServer (this);
  connect (tcpServer, SIGNAL (newConnection ()), this,
           SLOT (newConnectionEstablished ()));

  if (!tcpServer->listen (QHostAddress::Any, port)) {
    log << " [Failed]\n"
        "Unable to start the Partov server:" << tcpServer->errorString ();
    QCoreApplication::quit ();
  } else {
    log << " [Done]\n"
        "Listening for connections on port" << port << "...\n";
  }
  if (timeoutInSeconds > 0) {
    log << "  (Server will timeout in" << timeoutInSeconds << "seconds)\n";
    QTimer::singleShot (timeoutInSeconds * 1000, qApp, SLOT (quit ()));
  }
}

void Server::newConnectionEstablished (void) {
  try {
    QTcpSocket *socket = tcpServer->nextPendingConnection ();
    edu::sharif::partov::nse::usermanagement::UserAuthenticator *ua =
        new edu::sharif::partov::nse::usermanagement::UserAuthenticator (socket);
    connect (ua,
             SIGNAL (userAuthenticated (QTcpSocket *, edu::sharif::partov::nse::usermanagement::User)),
             this,
             SLOT (userAuthenticated (QTcpSocket *, edu::sharif::partov::nse::usermanagement::User)));
    connect (ua, SIGNAL (authenticationFailed (QTcpSocket *)), this,
             SLOT (authenticationFailed (QTcpSocket *)));
    connect (socket, SIGNAL (disconnected ()), socket, SLOT (deleteLater ()));
    ua->startAuthenticating ();

  } catch (const std::exception &e) {
    qCritical ("!!! Can not start simulator (%s). Degrading gracefully and continue.",
               e.what ());
  }
}

void Server::userAuthenticated (QTcpSocket *socket,
    edu::sharif::partov::nse::usermanagement::User user) {
  try {
    Simulator *simulator = new Simulator ();

    simulators->append (simulator);
    simulator->setSimulatorUserSocket (socket, user);
    connect (simulator, SIGNAL (finished ()),
             this, SLOT (simulatorFinished ()), Qt::QueuedConnection);
    QTimer::singleShot (0, simulator, SLOT (run ()));

  } catch (const std::exception &e) {
    qCritical ("!! Can not start simulator (%s). Degrading gracefully and continue.",
               e.what ());
  }
}

void Server::authenticationFailed (QTcpSocket *socket) {
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_5_4);

  stream << (quint32) Simulator::SigningInNegotiationType << (quint32) 0;
  socket->disconnectFromHost ();
}

void Server::simulatorFinished () {
  if (shuttingDown) {
    return;
  }
  Simulator *simulator = qobject_cast < Simulator * > (sender ());
  if (simulator) {
    int index = simulators->indexOf (simulator);
    if (index != -1) {
      simulators->removeAt (index);
      QTimer::singleShot (0, simulator, SLOT (finalize ()));
    }
  }
}

void Server::loadGlobalSettings () {
  QSettings config ("../config/config.ini", QSettings::IniFormat);
  config.beginGroup ("server");
  Server::verbose = config.value ("verbose", false).toBool ();
  config.endGroup ();
}

bool Server::isVerbose () {
  return Server::verbose;
}

}
}
}
}
}
