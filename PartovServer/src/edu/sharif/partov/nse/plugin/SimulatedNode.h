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

#ifndef SIMULATEDNODE_H_
#define SIMULATEDNODE_H_

#include "PluginNode.h"

#include "edu/sharif/partov/nse/network/Frame.h"

class QMutex;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

/**
 * Name:              SimulatedNode
 * Parent:            PluginNode
 * Description:       This plugin can extend the system with a client framework outside of the
 *                    system's server. So the plugin node can be controlled from outside world.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              None
 */
class SimulatedNode : public PluginNode {

  Q_OBJECT

public:
  /**
   * Just accepted packets will be transfered to simulator.
   */
  enum AcceptablePacketsType {
    /* accepts all packets */
    All,
    /* only accepts ip version 4 packets */
    IPv4Only,
    /* only accepts unicast ip version 4 packets */
    UnicastIPv4Only
  };

private:
  QString customInformation;
  AcceptablePacketsType apt;

public:
  SimulatedNode (const QString &nodeName, edu::sharif::partov::nse::map::Map *parent,
      bool replyARP = false, bool replyICMPEcho = false);
  virtual ~SimulatedNode ();

  void setCustomInformation (const QString &info);
  const QString &getCustomInformation () const;

  void setAcceptablePacketsType (SimulatedNode::AcceptablePacketsType apt);
  SimulatedNode::AcceptablePacketsType getAcceptablePacketsType () const;

  virtual bool setParameter (const QString &paramName, const QStringList &paramValues);

protected:
  virtual void
  pluginEngineHandleFrame (edu::sharif::partov::nse::network::SecondLayerFrame *frame,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      bool &finalizeFrame);
  virtual void processReceivedPacket (
      edu::sharif::partov::nse::network::IPBasedThirdLayerPacket *ip,
      edu::sharif::partov::nse::map::interface::Interface *interface,
      QHostAddress &myDestinedAddress, bool &finalizeFrame);

public slots:
  void sendFrame (int interfaceIndex,
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  void changeIPAddress (int interfaceIndex, quint32 ip);
  void changeNetmask (int interfaceIndex, quint32 netmask);

signals:
  void
  frameReceived (int interfaceIndex,
      edu::sharif::partov::nse::network::SecondLayerFrame *frame);
  void notifyUserAboutInvalidInterfaceIndex (
      edu::sharif::partov::nse::map::InvalidInterfaceIndexException *e);
  void nodeIPAddressChanged (QString nodeName, int interfaceIndex, quint32 ip);
  void nodeNetmaskChanged (QString nodeName, int interfaceIndex, quint32 netmask);

public:
  static SimulatedNode *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);

private:
  bool available;
  QMutex *mutex;

public:
  bool /* Synchronized */isAvailable () const;
  bool /* Synchronized */acquireNode (const QObject *owner);

protected slots:
  /**
   * Pre-Condition: node must not be available.
   */
  void /* Synchronized */releaseNode ();
};

}
}
}
}
}

#endif /* SIMULATEDNODE_H_ */
