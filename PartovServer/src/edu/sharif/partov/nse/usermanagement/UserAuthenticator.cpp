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

#include "UserAuthenticator.h"

#include "edu/sharif/partov/nse/server/Server.h"

#include <QTcpSocket>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace usermanagement {

UserAuthenticator::UserAuthenticator (QTcpSocket *_socket) :
    socket (_socket), blockSize (0) {
}

UserAuthenticator::~UserAuthenticator () {
}

void UserAuthenticator::displayError (QAbstractSocket::SocketError errorCode) {
  if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
    qDebug ("An error occurred during TCP IO operation, code %d", errorCode);
  }
}

void UserAuthenticator::startAuthenticating () {
  connect (socket, SIGNAL (disconnected ()), this, SLOT (deleteLater ()));

  connect (socket, SIGNAL (readyRead ()), this, SLOT (authenticating ()),
           Qt::DirectConnection);
  connect (socket, SIGNAL (error (QAbstractSocket::SocketError)), this,
           SLOT (displayError (QAbstractSocket::SocketError)), Qt::DirectConnection);
}

void UserAuthenticator::authenticating () {
  int val = realAuthenticating ();

  switch (val) {
  case 1:
    emit userAuthenticated (socket, user);
    break;

  case 0:
    // we should continue to read more data from socket...
    return;

  case -1:
    if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
      qDebug ("Signing in failed.");
    }
    emit authenticationFailed (socket);
    break;

  default:
    qCritical ("--- Panic: Authenticating method in UserAuthenticator returned"
               " with value %d", val);
    socket->disconnectFromHost ();
    break;
  }
  deleteLater ();
}

int UserAuthenticator::realAuthenticating () {
  /* read
   *  1. user name
   *  2. password
   */
  if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
    qDebug () << "Signing in ...";
  }
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_4_8);
  if (blockSize == 0) {
    if (socket->bytesAvailable () < (int) sizeof (quint16)) {
      return 0;
    }
    stream >> blockSize;
  }
  if (socket->bytesAvailable () < blockSize) {
    return 0;
  }

  // reading username....
  quint16 size;
  stream >> size;
  if (size < 1 || size > blockSize - 2 * sizeof (quint16)) {
    // Wrong format...
    return -1;
  }
  char *userNameStr = new char[size];
  if (stream.readRawData (userNameStr, size) != size) {
    // Qt magical bug !!! Of course never occur :D
    return -1;
  }
  QString userName = QString::fromAscii (userNameStr, size - 1);
  delete[] userNameStr;
  blockSize -= sizeof (quint16) + size;

  // reading password....
  stream >> size;
  if (size < 1 || size != blockSize - sizeof (quint16)) {
    // Wrong format...
    return -1;
  }
  char *passwordStr = new char[size];
  if (stream.readRawData (passwordStr, size) != size) {
    // Qt magical bug !!! Of course never occur :D
    return -1;
  }
  QString password = QString::fromAscii (passwordStr, size - 1);
  delete[] passwordStr;
  blockSize = 0;

  // authenticating ...
  user = edu::sharif::partov::nse::usermanagement::User::authenticateUser (userName,
                                                                           password);
  if (user.isNull ()) { // un-authenticated user !
    if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
      qDebug ("``%s'' blocked due to the wrong username/password.", qPrintable (userName));
    }
    return -1;
  }
  if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
    qDebug ("``%s'' is signed in.", qPrintable (userName));
  }
  return 1;
}

}
}
}
}
}
