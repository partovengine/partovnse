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

#include "edu/sharif/partov/nse/util/NonBlockingLocker.h"

#include <QTimer>
#include <QMutex>
#include <QSemaphore>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace server {

Simulator::Simulator () :
QObject (), communicationState (NotSignedInState), simulationState (IdleState),
shuttingDown (false), mutex (new QMutex ()), semaphore (new QSemaphore (0)),
blockSize (0), map (NULL), node (NULL) {
  // create simulator...
}

Simulator::~Simulator () {
  delete mutex;
  mutex = NULL;
  delete semaphore;
  semaphore = NULL;
}

void Simulator::finalize () {
  edu::sharif::partov::nse::util::NonBlockingLocker locker (mutex, false);
  if (!locker.isLocked ()) {
    QTimer::singleShot (0, this, SLOT (finalize ()));
    return;
  }
  shuttingDown = true;
  if (map) {
    QMutex *changesMutex = map->getMap ()->getMapChangesNotificationMutex ();
    locker.unlock ();
    edu::sharif::partov::nse::util::NonBlockingLocker changesMutexLocker (changesMutex);
    locker.relock (true);
    disconnect ();
    if (map == NULL) {
      semaphore->release ();
      changesMutexLocker.unlock ();
      locker.unlock ();
      edu::sharif::partov::nse::util::NonBlockingLocker waitCondition (semaphore, 2);
      locker.relock (true);
    }
  }
  deleteLater ();
}

void Simulator::setSimulatorUserSocket (QTcpSocket *socket,
    edu::sharif::partov::nse::usermanagement::User _user) {
  socket->disconnect ();

  this->socket = socket;
  user = _user;

  socket->setParent (this);

  connect (socket, SIGNAL (readyRead ()), this, SLOT (readData ()),
           Qt::DirectConnection);
  connect (socket, SIGNAL (error (QAbstractSocket::SocketError)),
           this, SLOT (displayError (QAbstractSocket::SocketError)),
           Qt::DirectConnection);
  connect (socket, SIGNAL (disconnected ()), this, SIGNAL (finished ()),
           Qt::DirectConnection);
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
  if (blockSize < sizeof (quint16)) {
    qWarning ("Malformed request");
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  blockSize -= sizeof (quint16);
  quint16 size;
  stream >> size;
  if (size < 1 || blockSize < size + sizeof (quint16)) {
    qWarning ("Malformed request");
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  blockSize -= size;
  char *mapFileNameStr = new char[size];
  if (stream.readRawData (mapFileNameStr, size) != size) {
    qCritical ("socket's read and bytesAvailable are not consistent");
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  mapFileName = QString::fromLocal8Bit (mapFileNameStr, size - 1);
  delete[] mapFileNameStr;
  blockSize -= sizeof (quint16);
  stream >> size;
  if (size < 1 || blockSize < size + sizeof (qint32)) {
    qWarning ("Malformed request");
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  blockSize -= size;
  char *creatorIdStr = new char[size];
  if (stream.readRawData (creatorIdStr, size) != size) {
    qCritical ("socket's read and bytesAvailable are not consistent");
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  creatorId = QString::fromLocal8Bit (creatorIdStr, size - 1);
  delete[] creatorIdStr;
  blockSize -= sizeof (qint32);
  stream >> needNewMap;
  if (blockSize != 0) {
    qWarning ("Malformed request");
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  if (needNewMap != 0) {
    creatorId = user.getUserName ();
  }
  communicationState = InstantiatingOrRetrievingMapState;
  QTimer::singleShot (0, this, SLOT (instantiateOrRetrieveMap ()));
}

void Simulator::instantiateOrRetrieveMap () {
  edu::sharif::partov::nse::util::NonBlockingLocker locker (mutex, false);
  if (!locker.isLocked ()) {
    QTimer::singleShot (0, this, SLOT (readData ()));
    return;
  }
  if (shuttingDown) {
    return;
  }
  if (communicationState != InstantiatingOrRetrievingMapState) {
    qWarning ("instantiateOrRetrieveMap () is called while the state of "
              "simulator is not InstantiatingOrRetrievingMapState. It is %d",
              communicationState);
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_5_4);
  try {
    map = edu::sharif::partov::nse::map::MapFactory::getInstance ()
        ->createOrRetrieveMap (user, mapFileName, creatorId, needNewMap == 1,
                               socket->peerAddress (), needNewMap == -1);
    if (!map) { // cannot acquire the lock
      QTimer::singleShot (0, this, SLOT (instantiateOrRetrieveMap ()));
      return;
    }
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
  if (blockSize < 1) {
    qWarning ("Malformed request");
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  char *nodeNameStr = new char[blockSize];
  if (stream.readRawData (nodeNameStr, blockSize) != blockSize) {
    qCritical ("socket's read and bytesAvailable are not consistent");
    socket->disconnectFromHost ();
    communicationState = DisconnectedState;
    return;
  }
  QString nodeName = QString::fromLocal8Bit (nodeNameStr, blockSize - 1);
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
    stream.writeRawData (qPrintable (ci), size);
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
  stream.setVersion (QDataStream::Qt_5_4);

  stream << static_cast<quint32> (ChangeEventNotificationType)
      << static_cast<quint32> (IPAddressChangedNotificationSubType);

  int size = nodeName.length () + 1; // +1 is for the last null character
  stream << static_cast<quint16> (size);
  stream.writeRawData (qPrintable (nodeName), size);

  stream << static_cast<quint32> (interfaceIndex) << static_cast<quint32> (ip);
}

void Simulator::nodeNetmaskChanged (QString nodeName, int interfaceIndex,
    quint32 netmask) {
  QMutexLocker locker (mutex);
  if (communicationState != ObservingChangeEventsState) {
    return;
  }
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_5_4);

  stream << static_cast<quint32> (ChangeEventNotificationType)
      << static_cast<quint32> (NetmaskChangedNotificationSubType);

  int size = nodeName.length () + 1; // +1 is for the last null character
  stream << static_cast<quint16> (size);
  stream.writeRawData (qPrintable (nodeName), size);

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
      if (blockSize < sizeof (quint32) + 1) {
        qWarning ("Malformed request");
        socket->disconnectFromHost ();
        communicationState = DisconnectedState;
        return;
      }
    }
    if (socket->bytesAvailable () < blockSize) {
      return;
    }
    blockSize -= sizeof (quint32);
    quint32 type;
    stream >> type;
    InformationRequestingCommand cmd = static_cast<InformationRequestingCommand> (type);

    char *nodeNameStr = new char[blockSize];
    if (stream.readRawData (nodeNameStr, blockSize) != blockSize) {
      qCritical ("socket's read and bytesAvailable are not consistent");
      socket->disconnectFromHost ();
      communicationState = DisconnectedState;
      return;
    }
    QString nodeName = QString::fromLocal8Bit (nodeNameStr, blockSize - 1);
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
          stream.writeRawData (qPrintable (ci), size);
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

    case WalkOnFiniteStateMachineCommand:
      emit walkOnFsm (); /* @@ signal emitted @@ */
      return;
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
      if (blockSize < sizeof (quint32) + 1) {
        qWarning ("Malformed request");
        socket->disconnectFromHost ();
        communicationState = DisconnectedState;
        return;
      }
    }
    if (socket->bytesAvailable () < blockSize) {
      return;
    }
    blockSize -= sizeof (quint32);
    quint32 interfaceIndex;
    stream >> interfaceIndex;

    int frameSize = blockSize;
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
  stream.setVersion (QDataStream::Qt_5_4);

  stream << (quint32) InvalidInterfaceIndexType;
  const QString &desc = e->getExceptionDescription ();
  int size = desc.length () + 1;
  stream << static_cast<quint16> (size);
  stream.writeRawData (qPrintable (desc), size);
  delete e;
}

/**
 * This slot will be called whenever user has sent more data.
 * This slot is executed in simulator special thread and should
 * read from socket and respond accordingly.
 */
void Simulator::readData () {
  edu::sharif::partov::nse::util::NonBlockingLocker locker (mutex, false);
  if (!locker.isLocked ()) {
    QTimer::singleShot (0, this, SLOT (readData ()));
    return;
  }
  if (shuttingDown) {
    return;
  }
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_5_4);

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
    qWarning ("Unexpected Simulator state: %d", communicationState);
    socket->disconnectFromHost ();
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
  out.setVersion (QDataStream::Qt_5_4);

  out << (quint16) 0;
  out << (quint32) interfaceIndex;
  frame->getLowerLayerFrame ()->writeFrameOnTheStream (out);

  out.device ()->seek (0);
  out << (quint16) (block.size () - sizeof (quint16));

  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_5_4);

  stream << (quint32) RawFrameReceivedNotificationType;
  stream.writeRawData (block.constData (), block.size ());

  frame->finalize ();
}

void Simulator::run () {
  if (Server::isVerbose ()) {
    qDebug ("New connection established. Reading user request...");
  }
  QDataStream stream (socket);
  stream.setVersion (QDataStream::Qt_5_4);

  stream << (quint32) SigningInNegotiationType << (quint32) 1;
  communicationState = JustSignedInState;
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
    locker.unlock ();
    semaphore->acquire ();
    changesMutex->lock ();
    locker.relock ();
    semaphore->release (2);
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
