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

#include "Link.h"

#include "LinkCsmaCd.h"
#include "LinkPointToPoint.h"

#include "edu/sharif/partov/nse/map/Map.h"
#include "edu/sharif/partov/nse/map/interface/Interface.h"

#include "edu/sharif/partov/nse/util/PacketLogger.h"
#include "edu/sharif/partov/nse/util/PacketLoggerFactory.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace link {

Link::Link (Protocol _protocol, const QString &linkName, Map *parent) :
    QObject (parent), protocol (_protocol) {
  setObjectName (linkName);
  switch (protocol) {
  case P_CSMA_CD:
    csmacdImp = new LinkCsmaCd (linkName + "-csma-cd", this);
    break;
  case P_802o3:
    ppImpUp = new LinkPointToPoint (linkName + "-pp-up", this);
    ppImpDown = new LinkPointToPoint (linkName + "-pp-down", this);
    break;
  default:
    // FIXME: throw exception
    break;
  }
  countOfConnectedClients = 0;
}

Link::~Link () {
}

bool Link::setBandwidth (quint64 bandwidth) {
  switch (protocol) {
  case P_CSMA_CD:
    csmacdImp->setBandwidth (bandwidth);
    return true;
  case P_802o3:
    ppImpUp->setBandwidth (bandwidth);
    ppImpDown->setBandwidth (bandwidth);
    return true;
  default:
    // FIXME: throw exception
    break;
  }
  return false;
}

bool Link::setLatency (quint64 latency) {
  switch (protocol) {
  case P_CSMA_CD:
    csmacdImp->setLatency (latency);
    return true;
  case P_802o3:
    ppImpUp->setLatency (latency);
    ppImpDown->setLatency (latency);
    return true;
  default:
    // FIXME: throw exception
    break;
  }
  return false;
}

bool Link::setLossRate (qreal rate) {
  switch (protocol) {
  case P_CSMA_CD:
    csmacdImp->setLossRate (rate);
    return true;
  case P_802o3:
    ppImpUp->setLossRate (rate);
    ppImpDown->setLossRate (rate);
    return true;
  default:
    // FIXME: throw exception
    break;
  }
  return false;
}

void Link::turnLinkUp () {
  switch (protocol) {
  case P_CSMA_CD:
    csmacdImp->turnLinkUp ();
    break;
  case P_802o3:
    ppImpUp->turnLinkUp ();
    ppImpDown->turnLinkUp ();
    break;
  default:
    // FIXME: Indeed this class must be refactored to two classes and these state-dependent behaviours must be pushed down in an inheritance tree
    break;
  }
}

void Link::turnLinkDown () {
  switch (protocol) {
  case P_CSMA_CD:
    csmacdImp->turnLinkDown ();
    break;
  case P_802o3:
    ppImpUp->turnLinkDown ();
    ppImpDown->turnLinkDown ();
    break;
  default:
    // FIXME: Indeed this class must be refactored to two classes and these state-dependent behaviours must be pushed down in an inheritance tree
    break;
  }
}

bool Link::setupPacketLogger (QString logPathTemplate, QString linkName) {
  switch (protocol) {
  case P_CSMA_CD:
    csmacdImp->installPacketLogger (
        edu::sharif::partov::nse::util::PacketLoggerFactory::getInstance ()->createPacketLogger (
            logPathTemplate, linkName + "-csma-cd"));
    return true;
  case P_802o3:
    ppImpUp->installPacketLogger (
        edu::sharif::partov::nse::util::PacketLoggerFactory::getInstance ()->createPacketLogger (
            logPathTemplate, linkName + "-pp-up"));
    ppImpDown->installPacketLogger (
        edu::sharif::partov::nse::util::PacketLoggerFactory::getInstance ()->createPacketLogger (
            logPathTemplate, linkName + "-pp-down"));
    return true;
  default:
    // FIXME: throw exception
    break;
  }
  return false;
}

/*
 * Interface-to-csmacdLink connection protocol:
 *  trivial (connect both way)
 *
 * Interface-to-ppLink connection protocol: *1
 *  check count of connected interfaces
 *   first interface
 *    increment count of connected interfaces
 *    read from up => write on iface
 *    read from iface => write on down
 *   second interface
 *    increment count of connected interfaces
 *    read from down => write on iface
 *    read from iface => write on up
 *   otherwise
 *    error
 *
 * csmacdLink-to-csmacdLink connection protocol:
 *  trivial (read from one link and write on other for one-way connection; do both way for two-way case)
 *
 * ppLink-to-csmacdLink connection protocol:
 *  ask whether connection is one-way or two-way
 *   one-way (read-only)
 *    read from up => write on other
 *    read from down => write on other
 *   two-way
 *    assume csmacdLink as an Interface and run *1 procedure
 *
 * csmacdLink-to-ppLink connection protocol:
 *  ask whether connection is one-way or two-way
 *   one-way (write-only)
 *    assume csmacdLink as an Interface
 *    check count of connected interfaces
 *     first one
 *      increment count of connected interfaces
 *      read from csmacdLink => write on down
 *     second one
 *      increment count of connected interfaces
 *      read from csmacdLink => write on up
 *   two-way
 *    assume csmacdLink as an Interface and run *1 procedure
 *
 * ppLink-to-ppLink connection protocol:
 *  ask whether connection is one-way or two-way
 *   one-way (read-only from first to second)
 *    assume first link as an Interface
 *    check count of connected interfaces to second link
 *     first one
 *      increment count of connected interfaces
 *      read from first->up => write on down
 *      read from first->down => write on down
 *     second one
 *      increment count of connected interfaces
 *      read from first->up => write on up
 *      read from first->down => write on up
 *   two-way
 *    assume first link as an Interface
 *    check count of connected interfaces to first link and second link
 *     first one - first one
 *      increment count of connected interfaces (on both links)
 *      read from first->up => write on second->down
 *      read from second->up => write on first->down
 *     first one - second one
 *      increment count of connected interfaces (on both links)
 *      read from first->up => write on second->up
 *      read from second->down => write on first->down
 *     second one - first one
 *      increment count of connected interfaces (on both links)
 *      read from first->down => write on second->down
 *      read from second->up => write on first->up
 *     second one - second one
 *      increment count of connected interfaces (on both links)
 *      read from first->down => write on second->up
 *      read from second->down => write on first->up
 */

bool Link::connectToInterfaceWithoutOwnershipTransfer (
    edu::sharif::partov::nse::map::interface::Interface *iface) {
  switch (protocol) {
  case P_CSMA_CD:
    return connectToInterfaceFromCsmaCdLinkWithoutOwnershipTransfer (iface);
  case P_802o3:
    return connectToInterfaceFromPpLinkWithoutOwnershipTransfer (iface);
  default:
    // FIXME: throw exception
    break;
  }
  return false;
}

bool Link::connectToInterface (
    edu::sharif::partov::nse::map::interface::Interface *iface) {
  switch (protocol) {
  case P_CSMA_CD:
    return connectToInterfaceFromCsmaCdLink (iface);
  case P_802o3:
    return connectToInterfaceFromPpLink (iface);
  default:
    // FIXME: throw exception
    break;
  }
  return false;
}

#define CONNECT_LINK_X_TO_INTERFACE_Y(X, Y) \
    QObject::connect ((X), \
          SIGNAL (propagateFrame (edu::sharif::partov::nse::network::SecondLayerFrame *)), \
          (Y), SLOT (receiveFrame (edu::sharif::partov::nse::network::SecondLayerFrame *)), \
          Qt::DirectConnection)

#define CONNECT_INTERFACE_X_TO_LINK_Y(X, Y) \
    QObject::connect ((X), \
          SIGNAL (propagateFrame (edu::sharif::partov::nse::network::SecondLayerFrame *)), \
          (Y), SLOT (sendFrame (edu::sharif::partov::nse::network::SecondLayerFrame *)), \
          Qt::DirectConnection)

#define CONNECT_INTERFACE_X_TO_LINK_Y_WITHOUT_OWNERSHIP_TRANSFER(X, Y) \
    QObject::connect ( \
          (X), \
          SIGNAL (propagateFrame (edu::sharif::partov::nse::network::SecondLayerFrame *)), \
          (Y), \
          SLOT (processPropagatedFrameFromAnotherLink (edu::sharif::partov::nse::network::SecondLayerFrame *)), \
          Qt::DirectConnection)

/**
 * For full protocol see above notes.
 * This note is borrowed from there for easy reference:
 *
 * Interface-to-csmacdLink connection protocol:
 *  trivial (connect both way)
 */
bool Link::connectToInterfaceFromCsmaCdLinkWithoutOwnershipTransfer (
    edu::sharif::partov::nse::map::interface::Interface *iface) {
  return CONNECT_LINK_X_TO_INTERFACE_Y (this->csmacdImp, iface)
      && CONNECT_INTERFACE_X_TO_LINK_Y_WITHOUT_OWNERSHIP_TRANSFER (iface, this->csmacdImp);
}

/**
 * For full protocol see above notes.
 * This note is borrowed from there for easy reference:
 *
 * Interface-to-ppLink connection protocol: *1
 *  check count of connected interfaces
 *   first interface
 *    increment count of connected interfaces
 *    read from up => write on iface
 *    read from iface => write on down
 *   second interface
 *    increment count of connected interfaces
 *    read from down => write on iface
 *    read from iface => write on up
 *   otherwise
 *    error
 */
bool Link::connectToInterfaceFromPpLinkWithoutOwnershipTransfer (
    edu::sharif::partov::nse::map::interface::Interface *iface) {
  if (countOfConnectedClients == 0) {
    countOfConnectedClients++;
    return CONNECT_LINK_X_TO_INTERFACE_Y (this->ppImpUp, iface)
        && CONNECT_INTERFACE_X_TO_LINK_Y_WITHOUT_OWNERSHIP_TRANSFER (iface, this->ppImpDown);
  } else if (countOfConnectedClients == 1) {
    countOfConnectedClients++;
    return CONNECT_LINK_X_TO_INTERFACE_Y (this->ppImpDown, iface)
        && CONNECT_INTERFACE_X_TO_LINK_Y_WITHOUT_OWNERSHIP_TRANSFER (iface, this->ppImpUp);
  } else {
    return false; // exactly two clients are allowed
  }
}

/**
 * For full protocol see above notes.
 * This note is borrowed from there for easy reference:
 *
 * Interface-to-csmacdLink connection protocol:
 *  trivial (connect both way)
 */
bool Link::connectToInterfaceFromCsmaCdLink (
    edu::sharif::partov::nse::map::interface::Interface *iface) {
  return CONNECT_LINK_X_TO_INTERFACE_Y (this->csmacdImp, iface)
      && CONNECT_INTERFACE_X_TO_LINK_Y (iface, this->csmacdImp);
}

/**
 * For full protocol see above notes.
 * This note is borrowed from there for easy reference:
 *
 * Interface-to-ppLink connection protocol: *1
 *  check count of connected interfaces
 *   first interface
 *    increment count of connected interfaces
 *    read from up => write on iface
 *    read from iface => write on down
 *   second interface
 *    increment count of connected interfaces
 *    read from down => write on iface
 *    read from iface => write on up
 *   otherwise
 *    error
 */
bool Link::connectToInterfaceFromPpLink (
    edu::sharif::partov::nse::map::interface::Interface *iface) {
  if (countOfConnectedClients == 0) {
    countOfConnectedClients++;
    return CONNECT_LINK_X_TO_INTERFACE_Y (this->ppImpUp, iface)
        && CONNECT_INTERFACE_X_TO_LINK_Y (iface, this->ppImpDown);
  } else if (countOfConnectedClients == 1) {
    countOfConnectedClients++;
    return CONNECT_LINK_X_TO_INTERFACE_Y (this->ppImpDown, iface)
        && CONNECT_INTERFACE_X_TO_LINK_Y (iface, this->ppImpUp);
  } else {
    return false; // exactly two clients are allowed
  }
}

bool Link::connectToLink (Link *link, bool twoway) {
  switch (protocol) {
  case P_CSMA_CD:
    switch (link->protocol) {
    case P_CSMA_CD:
      return connectToLinkCsmaCdToCsmaCd (link, twoway);
    case P_802o3:
      return connectToLinkCsmaCdToPp (link, twoway);
    default:
      // FIXME: throw exception
      break;
    }
    break;
  case P_802o3:
    switch (link->protocol) {
    case P_CSMA_CD:
      return connectToLinkPpToCsmaCd (link, twoway);
    case P_802o3:
      return connectToLinkPpToPp (link, twoway);
    default:
      // FIXME: throw exception
      break;
    }
    break;
  default:
    // FIXME: throw exception
    break;
  }
  return false;
}

#define CONNECT_LINK_X_TO_LINK_Y(X, Y) \
    QObject::connect ( \
          (X), \
          SIGNAL (propagateFrameOverConnectedLinks (edu::sharif::partov::nse::network::SecondLayerFrame *)), \
          (Y), \
          SLOT (processPropagatedFrameFromAnotherLink (edu::sharif::partov::nse::network::SecondLayerFrame *)), \
          Qt::DirectConnection)

/**
 * For full protocol see above notes.
 * This note is borrowed from there for easy reference:
 *
 * csmacdLink-to-csmacdLink connection protocol:
 *  trivial (read from one link and write on other for one-way connection; do both way for two-way case)
 */
bool Link::connectToLinkCsmaCdToCsmaCd (Link *link, bool twoway) {
  if (twoway) {
    return CONNECT_LINK_X_TO_LINK_Y (this->csmacdImp, link->csmacdImp)
        && CONNECT_LINK_X_TO_LINK_Y (link->csmacdImp, this->csmacdImp);
  } else {
    return CONNECT_LINK_X_TO_LINK_Y (this->csmacdImp, link->csmacdImp);
  }
}

/**
 * For full protocol see above notes.
 * This note is borrowed from there for easy reference:
 *
 * csmacdLink-to-ppLink connection protocol:
 *  ask whether connection is one-way or two-way
 *   one-way (write-only)
 *    assume csmacdLink as an Interface
 *    check count of connected interfaces
 *     first one
 *      increment count of connected interfaces
 *      read from csmacdLink => write on down
 *     second one
 *      increment count of connected interfaces
 *      read from csmacdLink => write on up
 *   two-way
 *    assume csmacdLink as an Interface and run *1 procedure
 *
 * The *1 procedure is as follows:
 * Interface-to-ppLink connection protocol: *1
 *  check count of connected interfaces
 *   first interface
 *    increment count of connected interfaces
 *    read from up => write on iface
 *    read from iface => write on down
 *   second interface
 *    increment count of connected interfaces
 *    read from down => write on iface
 *    read from iface => write on up
 *   otherwise
 *    error
 */
bool Link::connectToLinkCsmaCdToPp (Link *link, bool twoway) {
  if (twoway) {
    if (link->countOfConnectedClients == 0) {
      link->countOfConnectedClients++;
      return CONNECT_LINK_X_TO_LINK_Y (link->ppImpUp, this->csmacdImp)
          && CONNECT_LINK_X_TO_LINK_Y (this->csmacdImp, link->ppImpDown);
    } else if (link->countOfConnectedClients == 1) {
      link->countOfConnectedClients++;
      return CONNECT_LINK_X_TO_LINK_Y (link->ppImpDown, this->csmacdImp)
          && CONNECT_LINK_X_TO_LINK_Y (this->csmacdImp, link->ppImpUp);
    } else {
      return false; // exactly two clients are allowed
    }
  } else {
    if (link->countOfConnectedClients == 0) {
      link->countOfConnectedClients++;
      return CONNECT_LINK_X_TO_LINK_Y (this->csmacdImp, link->ppImpDown);
    } else if (link->countOfConnectedClients == 1) {
      link->countOfConnectedClients++;
      // this => link->up
      return CONNECT_LINK_X_TO_LINK_Y (this->csmacdImp, link->ppImpUp);
    } else {
      return false; // exactly two clients are allowed
    }
  }
}

/**
 * For full protocol see above notes.
 * This note is borrowed from there for easy reference:
 *
 * ppLink-to-csmacdLink connection protocol:
 *  ask whether connection is one-way or two-way
 *   one-way (read-only)
 *    read from up => write on other
 *    read from down => write on other
 *   two-way
 *    assume csmacdLink as an Interface and run *1 procedure
 *
 * The *1 procedure is as follows:
 * Interface-to-ppLink connection protocol: *1
 *  check count of connected interfaces
 *   first interface
 *    increment count of connected interfaces
 *    read from up => write on iface
 *    read from iface => write on down
 *   second interface
 *    increment count of connected interfaces
 *    read from down => write on iface
 *    read from iface => write on up
 *   otherwise
 *    error
 */
bool Link::connectToLinkPpToCsmaCd (Link *link, bool twoway) {
  if (twoway) {
    if (countOfConnectedClients == 0) {
      countOfConnectedClients++;
      return CONNECT_LINK_X_TO_LINK_Y (this->ppImpUp, link->csmacdImp)
          && CONNECT_LINK_X_TO_LINK_Y (link->csmacdImp, this->ppImpDown);
    } else if (countOfConnectedClients == 1) {
      countOfConnectedClients++;
      return CONNECT_LINK_X_TO_LINK_Y (this->ppImpDown, link->csmacdImp)
          && CONNECT_LINK_X_TO_LINK_Y (link->csmacdImp, this->ppImpUp);
    } else {
      return false; // exactly two clients are allowed
    }
  } else {
    return CONNECT_LINK_X_TO_LINK_Y (this->ppImpUp, link->csmacdImp)
        && CONNECT_LINK_X_TO_LINK_Y (this->ppImpDown, link->csmacdImp);
  }
}

/**
 * For full protocol see above notes.
 * This note is borrowed from there for easy reference:
 *
 * ppLink-to-ppLink connection protocol:
 *  ask whether connection is one-way or two-way
 *   one-way (read-only from first to second)
 *    assume first link as an Interface
 *    check count of connected interfaces to second link
 *     first one
 *      increment count of connected interfaces
 *      read from first->up => write on down
 *      read from first->down => write on down
 *     second one
 *      increment count of connected interfaces
 *      read from first->up => write on up
 *      read from first->down => write on up
 *   two-way
 *    assume first link as an Interface
 *    check count of connected interfaces to first link and second link
 *     first one - first one
 *      increment count of connected interfaces (on both links)
 *      read from first->up => write on second->down
 *      read from second->up => write on first->down
 *     first one - second one
 *      increment count of connected interfaces (on both links)
 *      read from first->up => write on second->up
 *      read from second->down => write on first->down
 *     second one - first one
 *      increment count of connected interfaces (on both links)
 *      read from first->down => write on second->down
 *      read from second->up => write on first->up
 *     second one - second one
 *      increment count of connected interfaces (on both links)
 *      read from first->down => write on second->up
 *      read from second->down => write on first->up
 */
bool Link::connectToLinkPpToPp (Link *link, bool twoway) {
  if (twoway) {
    if (countOfConnectedClients == 0) {
      countOfConnectedClients++;
      if (link->countOfConnectedClients == 0) {
        link->countOfConnectedClients++;
        return CONNECT_LINK_X_TO_LINK_Y (this->ppImpUp, link->ppImpDown)
            && CONNECT_LINK_X_TO_LINK_Y (link->ppImpUp, this->ppImpDown);

      } else if (link->countOfConnectedClients == 1) {
        link->countOfConnectedClients++;
        return CONNECT_LINK_X_TO_LINK_Y (this->ppImpUp, link->ppImpUp)
            && CONNECT_LINK_X_TO_LINK_Y (link->ppImpDown, this->ppImpDown);

      } else {
        return false; // exactly two clients are allowed
      }

    } else if (countOfConnectedClients == 1) {
      countOfConnectedClients++;
      if (link->countOfConnectedClients == 0) {
        link->countOfConnectedClients++;
        return CONNECT_LINK_X_TO_LINK_Y (this->ppImpDown, link->ppImpDown)
            && CONNECT_LINK_X_TO_LINK_Y (link->ppImpUp, this->ppImpUp);

      } else if (link->countOfConnectedClients == 1) {
        link->countOfConnectedClients++;
        return CONNECT_LINK_X_TO_LINK_Y (this->ppImpDown, link->ppImpUp)
            && CONNECT_LINK_X_TO_LINK_Y (link->ppImpDown, this->ppImpUp);

      } else {
        return false; // exactly two clients are allowed
      }

    } else {
      return false; // exactly two clients are allowed
    }
  } else { // one-way
    if (link->countOfConnectedClients == 0) {
      link->countOfConnectedClients++;
      return CONNECT_LINK_X_TO_LINK_Y (this->ppImpUp, link->ppImpDown)
          && CONNECT_LINK_X_TO_LINK_Y (this->ppImpDown, link->ppImpDown);
    } else if (link->countOfConnectedClients == 1) {
      link->countOfConnectedClients++;
      return CONNECT_LINK_X_TO_LINK_Y (this->ppImpUp, link->ppImpUp)
          && CONNECT_LINK_X_TO_LINK_Y (this->ppImpDown, link->ppImpUp);
    } else {
      return false; // exactly two clients are allowed
    }
  }
}

}
}
}
}
}
}
