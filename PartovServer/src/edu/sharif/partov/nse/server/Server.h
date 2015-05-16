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

#ifndef SERVER_H_
#define SERVER_H_

#include <QObject>

#include "edu/sharif/partov/nse/usermanagement/User.h"

template < typename T >
class QList;

class QTcpServer;
class QTcpSocket;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace server {

class Simulator;

/**
 * Name:              Server
 * Parent:            QObject
 * Description:       The server object which communicate with client frameworks and
 *                    connect them to simulatable nodes
 *                    via UserAuthenticator and Simulator.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.server
 * Tags:              None;
 */
class Server : public QObject {

  Q_OBJECT
private:
  bool shuttingDown;

  QList < Simulator * > *simulators;

protected:
  QTcpServer *tcpServer;

public:
  Server ();
  ~Server ();

public slots:
  void listenForConnections (void);

protected slots:
  void newConnectionEstablished (void);
  void userAuthenticated (QTcpSocket *socket,
      edu::sharif::partov::nse::usermanagement::User user);
  void authenticationFailed (QTcpSocket *socket);

  void simulatorFinished (void);

private:
  static bool verbose;

public:
  static void loadGlobalSettings ();
  static bool isVerbose ();
};

}
}
}
}
}

#endif /* SERVER_H_ */
