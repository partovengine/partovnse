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

#include "Simulator.h"

#include "Server.h"

#include "edu/sharif/partov/nse/map/interface/Interface.h"

#include "edu/sharif/partov/nse/map/MapExceptions.h"
#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/MapThread.h"
#include "edu/sharif/partov/nse/map/Map.h"

#include "edu/sharif/partov/nse/plugin/SimulatedNode.h"

#include "edu/sharif/partov/nse/network/FrameFactory.h"
#include "edu/sharif/partov/nse/network/IOException.h"
#include "edu/sharif/partov/nse/network/Frame.h"

#include <QTimer>
#include <QMutex>
#include <QWaitCondition>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace server {

Simulator::Simulator () :
    QThread (), communicationState (NotSignedInState), simulationState (IdleState),
    shuttingDown (false), mutex (NULL), cond (NULL),
    blockSize (0), map (NULL), node (NULL) {
  // create simulator...
  moveToThread (this);
}

Simulator::~Simulator () {
  delete mutex;
  mutex = NULL;
  delete cond;
  cond = NULL;
}

void Simulator::finalize () {
  { // PRE-CONDITION: This Simulator thread is already finished and wait()-ed upon.
    QMutexLocker locker (mutex);
    if (map) {
      QMutex *changesMutex = map->getMap ()->getMapChangesNotificationMutex ();
      shuttingDown = true;
      locker.unlock ();
      QMutexLocker changesMutexLocker (changesMutex);
      locker.relock ();
      disconnect ();
      if (map == NULL) {
        cond->wakeOne ();
        changesMutexLocker.unlock ();
        cond->wait (mutex);
      }
    }
  }
  delete this;
}

void Simulator::setSimulatorUserSocket (QTcpSocket *socket,
    edu::sharif::partov::nse::usermanagement::User _user) {
  socket->disconnect ();

  socket->setParent (0);
  socket->moveToThread (this);
  this->socket = socket;
  user = _user;

  socket->setParent (this);
}

void Simulator::readMapRequestData (QDataStream & stream) {
  /* read
   *   1. map file name
   *   2. map creator id
   *   3. need new map (+2: no, let observe change events of the existing map;
   *                    +1: yes, create a map;
   *                     0: no,  connect to an existing map;
   *                    -1: No,  delete the existing map too)
   */
  if (Server::isVerbose ()) {
    qDebug ("Read map request data ...");
  }
  if (blockSize == 0) {
    if (socket->bytesAvailable () < (int) sizeof (quint16)) {
      return;
    }
    stream >> blockSize;
  }
  if (socket->bytesAvailable () < blockSize) {
    return;
  }
  quint16 size;
  stream >> size;
  if (size < 1) {
    // Wrong format...
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  char *mapFileNameStr = new char[size];
  if (stream.readRawData (mapFileNameStr, size) != size) {
    // Qt magical bug !!! Of course never occur :D
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  QString mapFileName = QString::fromAscii (mapFileNameStr, size - 1);
  delete[] mapFileNameStr;
  stream >> size;
  if (size < 1) {
    // Wrong format...
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  char *creatorIdStr = new char[size];
  if (stream.readRawData (creatorIdStr, size) != size) {
    // Qt magical bug !!! Of course never occur :D
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  QString creatorId = QString::fromAscii (creatorIdStr, size - 1);
  delete[] creatorIdStr;
  qint32 needNewMap;
  stream >> needNewMap;
  blockSize = 0;
  if (needNewMap != 0) {
    creatorId = user.getUserName ();
  }
  try {
    map = edu::sharif::partov::nse::map::MapFactory::getInstance ()
        ->createOrRetrieveMap (user, mapFileName, creatorId, needNewMap == 1,
                               socket->peerAddress (), needNewMap == -1);
    if (needNewMap == -1) {
      map->finalize ();
      map = NULL;
      stream << (quint32) MapNodeSelectingNegotiationType
          << (quint32) MapInstanceResourcesAreReleased;

    } else if (needNewMap == 2) { // let observe the change events...
      connect (map, SIGNAL (aboutToFinish ()), this,
               SLOT (mapSimulationThreadIsAboutToFinish ()), Qt::DirectConnection);
      connect (map->getMap (), SIGNAL (nodeIPAddressChanged (QString, int, quint32)),
               this, SLOT (nodeIPAddressChanged (QString, int, quint32)),
               Qt::DirectConnection);
      connect (map->getMap (), SIGNAL (nodeNetmaskChanged (QString, int, quint32)), this,
               SLOT (nodeNetmaskChanged (QString, int, quint32)), Qt::DirectConnection);

      communicationState = ObservingChangeEventsState;
      stream << (quint32) MapNodeSelectingNegotiationType << (quint32) MapSelectedCommand;
      return;

    } else { // needNewMap == 0 || needNewMap == 1
      connect (map, SIGNAL (aboutToFinish ()), this,
               SLOT (mapSimulationThreadIsAboutToFinish ()), Qt::DirectConnection);

      communicationState = MapSelectedState;
      stream << (quint32) MapNodeSelectingNegotiationType << (quint32) MapSelectedCommand;
      return;
    }

  } catch (edu::sharif::partov::nse::map::AuthorizationException *e) {
    e->printExceptionDescription ();
    delete e; // The unauthorized user should not know that the map exists :)
    stream << (quint32) MapNodeSelectingNegotiationType << (quint32) MapNotExistCommand;

  } catch (edu::sharif::partov::nse::map::MapNotFoundException *e) {
    e->printExceptionDescription ();
    delete e;
    stream << (quint32) MapNodeSelectingNegotiationType << (quint32) MapNotExistCommand;

  } catch (edu::sharif::partov::nse::map::MaximumMapInstancesPerUserViolatedException *e) {
    e->printExceptionDescription ();
    delete e;
    stream << (quint32) MapNodeSelectingNegotiationType
        << (quint32) MaxMapIpuViolatedCommand;

  } catch (edu::sharif::partov::nse::map::OutOfResourceException *e) {
    e->printExceptionDescription ();
    delete e;
    stream << (quint32) MapNodeSelectingNegotiationType << (quint32) OutOfResourceCommand;
  }
  // closing connection...
  socket->disconnectFromHost ();
  communicationState = DisconnectedState;
}

void Simulator::readNodeRequestData (QDataStream & stream) {
  // read node name
  if (blockSize == 0) {
    if (socket->bytesAvailable () < (int) sizeof (quint16)) {
      return;
    }
    stream >> blockSize;
  }
  if (socket->bytesAvailable () < blockSize) {
    return;
  }
  char *nodeNameStr = new char[blockSize];
  if (stream.readRawData (nodeNameStr, blockSize) != blockSize) {
    // Qt magical bug !!! Of course never occur :D
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  QString nodeName = QString::fromAscii (nodeNameStr, blockSize - 1);
  delete[] nodeNameStr;
  blockSize = 0;
  try {
    node = qobject_cast < edu::sharif::partov::nse::plugin::SimulatedNode * > (
        map->getMap ()->retrieveNodeByName (nodeName));
    if (!node || !node->acquireNode (this)) {
      node = NULL;
      QString mesg = "There is no available simulatable node with ``%1'' as its name"
          " in the ``%2'' map.";
      throw new edu::sharif::partov::nse::map::NoSuchMapNodeException
          (mesg.arg (nodeName).arg (map->getMap ()->objectName ()));
    }

    communicationState = ConnectedToAMapNodeState;
    stream << (quint32) MapNodeSelectingNegotiationType << (quint32) NodeSelectedCommand;
    return;

  } catch (edu::sharif::partov::nse::map::NoSuchMapNodeException *e) {
    e->printExceptionDescription ();
    delete e;
    stream << (quint32) MapNodeSelectingNegotiationType << (quint32) NodeNotExistCommand;
  }
  // closing connection...
  socket->disconnectFromHost ();
  communicationState = DisconnectedState;
}

/**
 * User may ask for information about
 *    1. interfaces (ports)
 *    2. custom information provided by map description
 * or request to enter simulation mode.
 */
void Simulator::readNodeInformationRequestData (QDataStream & stream) {
  if (socket->bytesAvailable () < (int) sizeof (quint32)) {
    return;
  }
  quint32 command;
  stream >> command;
  InformationRequestingCommand cmd =
      static_cast<InformationRequestingCommand> (command);
  switch (cmd) {
  case InterfacesInformationRequestCommand:
  {
    stream << (quint32) InformationSynchronizationType
        << (quint32) InterfacesInformationSynchronizationSubType;
    const QList < edu::sharif::partov::nse::map::interface::Interface * > &interfaces =
        node->getInterfaces ();
    stream << (quint32) interfaces.size ();

    foreach (edu::sharif::partov::nse::map::interface::Interface *iface, interfaces) {
      iface->writeInterfaceInformation (stream);
    }
  }
    break;

  case CustomInformationRequestCommand:
  {
    stream << (quint32) InformationSynchronizationType
        << (quint32) CustomInformationSynchronizationSubType;
    const QString &ci = node->getCustomInformation ();
    int size = ci.length () + 1;
    stream << static_cast<quint16> (size);
    stream.writeRawData (ci.toAscii ().constData (), size);
  }
    break;

  case StartingSimulationRequestCommand:
    communicationState = SimulatingNodeState;
    stream << (quint32) SimulationStartedNotificationType;
    break;

  default:
    // Invalid command. Disconnecting....
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    break;
  }
}

void Simulator::nodeIPAddressChanged (QString nodeName, int interfaceIndex, quint32 ip) {
  QMutexLocker locker (mutex);
  if (communicationState != ObservingChangeEventsState) {
    return;
  }
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_4_8);

  stream << static_cast<quint32> (ChangeEventNotificationType)
      << static_cast<quint32> (IPAddressChangedNotificationSubType);

  int size = nodeName.length () + 1; // +1 is for the last null character
  stream << static_cast<quint16> (size);
  stream.writeRawData (nodeName.toAscii ().constData (), size);

  stream << static_cast<quint32> (interfaceIndex) << static_cast<quint32> (ip);
}

void Simulator::nodeNetmaskChanged (QString nodeName, int interfaceIndex,
    quint32 netmask) {
  QMutexLocker locker (mutex);
  if (communicationState != ObservingChangeEventsState) {
    return;
  }
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_4_8);

  stream << static_cast<quint32> (ChangeEventNotificationType)
      << static_cast<quint32> (NetmaskChangedNotificationSubType);

  int size = nodeName.length () + 1; // +1 is for the last null character
  stream << static_cast<quint16> (size);
  stream.writeRawData (nodeName.toAscii ().constData (), size);

  stream << static_cast<quint32> (interfaceIndex) << static_cast<quint32> (netmask);
}

void Simulator::readObservingData (QDataStream &stream) {
  SimulationCommand cmd;

  if (simulationState == IdleState) {
    if (socket->bytesAvailable () < (int) sizeof (quint32)) {
      return;
    }
    quint32 command;
    stream >> command;
    cmd = static_cast<SimulationCommand> (command);
    switch (cmd) {
    default: // Invalid command.
    case DisconnectCommand:
      socket->disconnectFromHost ();
      communicationState = DisconnectedState;
      return;

    case StateQueryCommand:
      simulationState = InProgressState;
      break;
    }
  }
  switch (simulationState) {
  case InProgressState:
  {
    /**
     * we must read
     *  1. length of data
     *  2. type of the requested information (what state was queried).
     *  3. name of the node
     */
    if (blockSize == 0) {
      if (socket->bytesAvailable () < (int) sizeof (quint16)) {
        return;
      }
      stream >> blockSize;
    }
    if (socket->bytesAvailable () < blockSize) {
      return;
    }
    quint32 type;
    stream >> type;
    InformationRequestingCommand cmd = static_cast<InformationRequestingCommand> (type);

    int nodeNameSize = ((int) blockSize) - ((int) sizeof (quint32));
    char *nodeNameStr = new char[nodeNameSize];
    if (stream.readRawData (nodeNameStr, nodeNameSize) != nodeNameSize) {
      // Qt magical bug !!! Of course never occur :D
      socket->disconnectFromHost ();
      communicationState = DisconnectedState;
      return;
    }
    QString nodeName = QString::fromAscii (nodeNameStr, nodeNameSize - 1);
    delete[] nodeNameStr;
    blockSize = 0;
    try {
      node = qobject_cast < edu::sharif::partov::nse::plugin::SimulatedNode * > (
          map->getMap ()->retrieveNodeByName (nodeName));
      if (!node) {
        node = NULL;
        QString mesg = "There is no such requested node with ``%1'' as its name"
            " in the ``%2'' map.";
        throw new edu::sharif::partov::nse::map::NoSuchMapNodeException
            (mesg.arg (nodeName).arg (map->getMap ()->objectName ()));
      } else {
        switch (cmd) {
        case InterfacesInformationRequestCommand:
        {
          stream << (quint32) InformationSynchronizationType
              << (quint32) InterfacesInformationSynchronizationSubType;
          const QList < edu::sharif::partov::nse::map::interface::Interface * > &interfaces =
              node->getInterfaces ();
          stream << (quint32) interfaces.size ();

          foreach (edu::sharif::partov::nse::map::interface::Interface *iface, interfaces) {
            iface->writeInterfaceInformation (stream);
          }
        }
          break;

        case CustomInformationRequestCommand:
        {
          stream << (quint32) InformationSynchronizationType
              << (quint32) CustomInformationSynchronizationSubType;
          const QString &ci = node->getCustomInformation ();
          int size = ci.length () + 1;
          stream << static_cast<quint16> (size);
          stream.writeRawData (ci.toAscii ().constData (), size);
        }
          break;

        default:
          // Invalid command. Disconnecting....
          socket->disconnectFromHost ();
          communicationState = DisconnectedState;
          break;
        }
      }

    } catch (edu::sharif::partov::nse::map::NoSuchMapNodeException *e) {
      e->printExceptionDescription ();
      delete e;
      stream << (quint32) MapNodeSelectingNegotiationType
          << (quint32) NodeNotExistCommand;
    }
    node = NULL;
    simulationState = IdleState;
    // If there were pending data, we must save it from being forgotten forever...
    if (socket->bytesAvailable () > 0) {
      QTimer::singleShot (0, this, SLOT (readData ()));
    }
  }
    break;

  default:
    qCritical ("--- Panic: Simulation state is undefined: %d", simulationState);
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    break;
  }
}

void Simulator::readSimulationData (QDataStream &stream) {
  SimulationCommand cmd;

  if (simulationState == IdleState) {
    if (socket->bytesAvailable () < (int) sizeof (quint32)) {
      return;
    }
    quint32 command;
    stream >> command;
    cmd = static_cast<SimulationCommand> (command);
    switch (cmd) {
    default: // Invalid command.
    case DisconnectCommand:
      socket->disconnectFromHost ();
      communicationState = DisconnectedState;
      return;

    case SendFrameCommand:
      simulationState = InProgressState;
      break;

    case ChangeIPAddressCommand:
      simulationState = ChangingIPAddressState;
      break;

    case ChangeNetmaskCommand:
      simulationState = ChangingNetmaskState;
      break;
    }
  }
  switch (simulationState) {
  case InProgressState:
  {
    /**
     * we must read
     *  1. length of data
     *  2. interface index to send packet on it
     *  3. the packet itself
     */
    if (blockSize == 0) {
      if (socket->bytesAvailable () < (int) sizeof (quint16)) {
        return;
      }
      stream >> blockSize;
      if (blockSize <= sizeof (quint32)) {
        socket->disconnectFromHost ();
        communicationState = DisconnectedState;
        return;
      }
    }
    if (socket->bytesAvailable () < blockSize) {
      return;
    }
    quint32 interfaceIndex;
    stream >> interfaceIndex;

    int frameSize = ((int) blockSize) - ((int) sizeof (quint32));
    blockSize = 0;
    try {
      edu::sharif::partov::nse::network::SecondLayerFrame *frame =
          edu::sharif::partov::nse::network::FrameFactory::getInstance ()
          ->createSecondLayerFrameFromStream (stream, frameSize);
      simulationState = IdleState;
      // If there were pending data, we must save it from being forgotten forever...
      if (socket->bytesAvailable () > 0) {
        QTimer::singleShot (0, this, SLOT (readData ()));
      }
      emit sendFrame ((int) interfaceIndex, frame); /* @@ signal emitted @@ */

    } catch (edu::sharif::partov::nse::network::IOException *e) {
      e->printExceptionDescription ();
      delete e;

      socket->disconnectFromHost ();
      communicationState = DisconnectedState;
    }
  }
    break;

  case ChangingIPAddressState:
  {
    if (socket->bytesAvailable () < (int) (2 * sizeof (quint32))) {
      return;
    }
    quint32 ifaceIndex, newIP;
    stream >> ifaceIndex >> newIP;

    simulationState = IdleState;
    // If there were pending data, we must save it from being forgotten forever...
    if (socket->bytesAvailable () > 0) {
      QTimer::singleShot (0, this, SLOT (readData ()));
    }
    emit changeIPAddress ((int) ifaceIndex, newIP); /* @@ signal emitted @@ */
  }
    break;

  case ChangingNetmaskState:
  {
    if (socket->bytesAvailable () < (int) (2 * sizeof (quint32))) {
      return;
    }
    quint32 ifaceIndex, newNetmask;
    stream >> ifaceIndex >> newNetmask;

    simulationState = IdleState;
    // If there were pending data, we must save it from being forgotten forever...
    if (socket->bytesAvailable () > 0) {
      QTimer::singleShot (0, this, SLOT (readData ()));
    }
    emit changeNetmask ((int) ifaceIndex, newNetmask); /* @@ signal emitted @@ */
  }
    break;

  default:
    qCritical ("--- Panic: Simulation state is undefined: %d", simulationState);
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    break;
  }
}

void Simulator::notifyUserAboutInvalidInterfaceIndex (
    edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e) {
  if (communicationState != SimulatingNodeState) {
    return;
  }
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_4_8);

  stream << (quint32) InvalidInterfaceIndexType;
  const QString &desc = e->getExceptionDescription ();
  int size = desc.length () + 1;
  stream << static_cast<quint16> (size);
  stream.writeRawData (desc.toAscii ().constData (), size);
  delete e;
}

/**
 * This slot will be called whenever user has sent more data.
 * This slot is executed in simulator special thread and should
 * read from socket and respond accordingly.
 */
void Simulator::readData () {
  QMutexLocker locker (mutex);

  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_4_8);

  switch (communicationState) {
  case DisconnectedState:
    // Ignore data...
    break;

  case JustSignedInState:
    readMapRequestData (stream);
    break;

  case MapSelectedState:
    readNodeRequestData (stream);
    break;

  case ConnectedToAMapNodeState:
    readNodeInformationRequestData (stream);
    break;

  case SimulatingNodeState:
    readSimulationData (stream);
    break;

  case ObservingChangeEventsState:
    readObservingData (stream);
    break;

  default:
    qCritical ("--- Panic: Simulator undefined state: %d", communicationState);
    communicationState = DisconnectedState;
    break;
  }
}

void Simulator::frameReceived (int interfaceIndex,
    edu::sharif::partov::nse::network::SecondLayerFrame *frame) {
  if (communicationState != SimulatingNodeState) {
    frame->finalize ();
    return;
  }
  QByteArray block;
  QDataStream out (&block, QIODevice::WriteOnly);
  out.setVersion (QDataStream::Qt_4_8);

  out << (quint16) 0;
  out << (quint32) interfaceIndex;
  frame->getLowerLayerFrame ()->writeFrameOnTheStream (out);

  out.device ()->seek (0);
  out << (quint16) (block.size () - sizeof (quint16));

  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_4_8);

  stream << (quint32) RawFrameReceivedNotificationType;
  stream.writeRawData (block.constData (), block.size ());

  frame->finalize ();
}

void Simulator::run (void) {
  try {
    mutex = new QMutex ();
    cond = new QWaitCondition ();
    if (Server::isVerbose ()) {
      qDebug ("New connection established. Reading user request...");
    }
    connect (socket, SIGNAL (disconnected ()), this, SLOT (quit ()),
             Qt::DirectConnection);

    connect (socket, SIGNAL (readyRead ()), this, SLOT (readData ()),
             Qt::DirectConnection);
    connect (socket, SIGNAL (error (QAbstractSocket::SocketError)), this,
             SLOT (displayError (QAbstractSocket::SocketError)), Qt::DirectConnection);

    {
      QDataStream stream (socket);
      stream.setVersion (QDataStream::Qt_4_8);

      stream << (quint32) SigningInNegotiationType << (quint32) 1;
      communicationState = JustSignedInState;
    }

    exec (); // enter event loop

  } catch (const std::exception &e) {
    qCritical ("--- Panic: Simulator thread died unexpectedly (%s)."
               " Degrading gracefully and continue.", e.what ());
  }
}

void Simulator::displayError (QAbstractSocket::SocketError errorCode) {
  if (Server::isVerbose ()) {
    qDebug ("An error occurred during TCP IO operation, code %d", errorCode);
  }
}

void Simulator::mapSimulationThreadIsAboutToFinish () {
  QMutexLocker locker (mutex);
  if (shuttingDown) {
    QMutex *changesMutex = map->getMap ()->getMapChangesNotificationMutex ();
    map = NULL;
    changesMutex->unlock ();
    cond->wait (mutex);
    changesMutex->lock ();
    cond->wakeOne ();
  } else {
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    map = NULL; // map changes notification mutex is already locked
  }
}

}
}
}
}
}
