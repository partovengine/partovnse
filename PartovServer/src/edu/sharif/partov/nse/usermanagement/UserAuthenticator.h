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

#ifndef USERAUTHENTICATOR_H_
#define USERAUTHENTICATOR_H_

#include <QObject>
#include <QAbstractSocket>

#include "User.h"

class QTcpSocket;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace usermanagement {

/**
 * Name:              UserAuthenticator
 * Parent:            QObject
 * Description:       This class has responsibility to execute the signing in process of the SCFCP
 *                    and indicate the authenticated user or the failure of authentication process
 *                    via signals.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.usermanagement
 * Tags:              None
 */
class UserAuthenticator : public QObject {
Q_OBJECT

private:
  QTcpSocket *socket;
  quint16 blockSize;
  User user;

public:
  UserAuthenticator (QTcpSocket *_socket);
  virtual ~UserAuthenticator ();

  void startAuthenticating ();

private slots:
  void authenticating ();
  void displayError (QAbstractSocket::SocketError errorCode);

private:
  int realAuthenticating ();

signals:
  void userAuthenticated (QTcpSocket *socket, edu::sharif::partov::nse::usermanagement::User user);
  void authenticationFailed (QTcpSocket *socket);
};

}
}
}
}
}

#endif /* USERAUTHENTICATOR_H_ */
