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

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "edu/sharif/partov/nse/usermanagement/User.h"

#include <QTcpSocket>

class QMutex;
class QSemaphore;
class QByteArray;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

class SecondLayerFrame;
}
namespace map {

class MapThread;
class InvalidInterfaceIndexException;
}
namespace plugin {

class SimulatedNode;
}
namespace server {

class Server;

/**
 * Name:              Simulator
 * Parent:            QObject
 * Description:       A simulator is a proxy of simulated node which stands in
 *                    the main thread and communicates with exactly one
 *                    authenticated client framework.
 *                    So it can send forward/backward the sent/received frames
 *                    via the SimulatedNode plugin.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.server
 * Tags:              None
 */
class Simulator : public QObject {

  Q_OBJECT
public:
  enum StubCommunicationProtocolType {
    SigningInNegotiationType = 0,
    MapNodeSelectingNegotiationType = 1,
    InformationSynchronizationType = 2,
    SimulationStartedNotificationType = 3,
    RawFrameReceivedNotificationType = 4,
    InvalidInterfaceIndexType = 5,
    ChangeEventNotificationType = 6
  };

  enum ChangeEventNotificationSubTypes {
    IPAddressChangedNotificationSubType = 1, NetmaskChangedNotificationSubType = 2
  };

  enum InformationSynchronizationSubTypes {
    InterfacesInformationSynchronizationSubType = 1,
    CustomInformationSynchronizationSubType = 2
  };

  enum MapNodeSelectingNegotiationCommands {
    MapNotExistCommand = 0,
    MaxMapIpuViolatedCommand = 1,
    MapSelectedCommand = 2,
    NodeNotExistCommand = 3,
    NodeSelectedCommand = 4,
    OutOfResourceCommand = 5,
    MapInstanceResourcesAreReleased = 6
  };

  enum InformationRequestingCommand {
    InterfacesInformationRequestCommand = 0,
    CustomInformationRequestCommand = 1,
    StartingSimulationRequestCommand = 2
  };

  enum SimulationCommand {
    DisconnectCommand = 0,
    SendFrameCommand = 1,
    ChangeIPAddressCommand = 2,
    ChangeNetmaskCommand = 3,
    StateQueryCommand = 4,
    WalkOnFiniteStateMachineCommand = 5
  };

  enum CommunicationState {
    NotSignedInState,
    JustSignedInState,
    InstantiatingOrRetrievingMapState,
    MapSelectedState,
    ConnectedToAMapNodeState,
    SimulatingNodeState,
    DisconnectedState,
    ObservingChangeEventsState
  };

  enum SimulationState {
    IdleState, InProgressState, ChangingIPAddressState, ChangingNetmaskState
  };

private:
  QByteArray *pendingMessage;

  CommunicationState communicationState;
  SimulationState simulationState;

  bool shuttingDown;
  QMutex *mutex;
  QMutex *mayBlock;

  quint16 blockSize;
  QTcpSocket *socket;

  edu::sharif::partov::nse::map::MapThread *map;
  edu::sharif::partov::nse::plugin::SimulatedNode *node;
  edu::sharif::partov::nse::usermanagement::User user;

  QString mapFileName;
  QString creatorId;
  qint32 needNewMap;

protected slots:
  void displayError (QAbstractSocket::SocketError errorCode);
  void readData (void);

  void instantiateOrRetrieveMap ();

  void frameReceived (int interfaceIndex,
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  void notifyUserAboutInvalidInterfaceIndex (
      edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e);

  void mapSimulationThreadIsAboutToFinish ();

  void nodeIPAddressChanged (QString nodeName, int interfaceIndex, quint32 ip);
  void nodeNetmaskChanged (QString nodeName, int interfaceIndex, quint32 netmask);

signals:
  void walkOnFsm ();
  void sendFrame (int interfaceIndex,
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  void changeIPAddress (int interfaceIndex, quint32 ip);
  void changeNetmask (int interfaceIndex, quint32 netmask);
  void finished ();

public:
  Simulator (Server *server);
  ~Simulator ();

  void setSimulatorUserSocket (QTcpSocket *socket,
      edu::sharif::partov::nse::usermanagement::User _user);

public slots:
  void finalize ();
  void run ();

  void sendPendingMessage ();

private:
  void readMapRequestData (QDataStream &stream);
  void readNodeRequestData (QDataStream &stream);
  void readNodeInformationRequestData (QDataStream &stream);
  void readObservingData (QDataStream &stream);
  void readSimulationData (QDataStream &stream);
};

}
}
}
}
}

#endif /* SIMULATOR_H_ */
