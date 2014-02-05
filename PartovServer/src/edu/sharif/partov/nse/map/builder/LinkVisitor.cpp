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

#include "LinkVisitor.h"

#include "edu/sharif/partov/nse/map/Map.h"
#include "edu/sharif/partov/nse/map/VariableManager.h"

#include "MapReader.h"
#include "ListVisitor.h"
#include "UnnamedFieldVisitor.h"
#include "UnnamedFieldRefVisitor.h"

#include <QDomElement>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

LinkVisitor::LinkVisitor (Map *_map, QString _logPathTemplate) :
NamedElementVisitor (_map), logPathTemplate (_logPathTemplate) {
}

LinkVisitor::~LinkVisitor () {
}

void LinkVisitor::processNamedElement (QString linkName, QDomElement link)
throw (MapFileFormatException *) {
  edu::sharif::partov::nse::map::link::Link::Protocol protocol =
      extractLinkProtocol (link);
  edu::sharif::partov::nse::map::link::Link *lk =
      new edu::sharif::partov::nse::map::link::Link (protocol, linkName, map);

  int allChildren = link.childNodes ().size ();
  int validChildren = allChildren - connectLinkToOtherLinks (link, lk);

  parseLinkLoggingParameter (linkName, link, lk);

  validChildren += allChildren - extractLossRate (link, lk);
  validChildren += allChildren - extractLatency (link, lk);
  validChildren += allChildren - extractBandwidth (link, lk);
  if (validChildren < allChildren) {
    throw new MapFileFormatException (link, "Unknown child tag name");
  }
}

edu::sharif::partov::nse::map::link::Link::Protocol LinkVisitor::extractLinkProtocol (
    const QDomElement &link) const throw (MapFileFormatException *) {
  QString protocolStr = link.attribute ("protocol");
  if (protocolStr.isNull ()) {
    throw new MapFileFormatException (link, "Missing protocol parameter");
  }
  if (protocolStr == "csma-cd") {
    return edu::sharif::partov::nse::map::link::Link::P_CSMA_CD;
  } else if (protocolStr == "802.3") {
    return edu::sharif::partov::nse::map::link::Link::P_802o3;
  } else {
    throw new MapFileFormatException (link, "Unknown protocol value");
  }
}

int LinkVisitor::connectLinkToOtherLinks (const QDomElement &link,
    edu::sharif::partov::nse::map::link::Link *lk) const
throw (MapFileFormatException *) {

  class ConnectedLinkVisitor : public NamedElementVisitor {

   private:
    edu::sharif::partov::nse::map::link::Link *lk;

   public:

    ConnectedLinkVisitor (Map *_map, edu::sharif::partov::nse::map::link::Link *_lk) :
        NamedElementVisitor (_map), lk (_lk) {
    }

    virtual void processNamedElement (QString anotherLinkName,
        QDomElement connectedToLinkTag) throw (MapFileFormatException *) {
      QString running = connectedToLinkTag.attribute ("running", "false");
      if (running == "true") {
        anotherLinkName = map->getVariableManager ()->resolveName (anotherLinkName);
      } else if (running == "false") {
        // default case; no change is required for name
      } else {
        throw new MapFileFormatException
            (connectedToLinkTag, "Malformed connected-to-link tag's running attribute");
      }
      edu::sharif::partov::nse::map::link::Link *anotherLk = map->findChild <
          edu::sharif::partov::nse::map::link::Link * > (anotherLinkName);
      if (!anotherLk) {
        throw new MapFileFormatException
            (connectedToLinkTag, "Connected link name can not be found");
      }
      QString direction = connectedToLinkTag.attribute ("direction");
      if (direction.isNull ()) {
        throw new MapFileFormatException
            (connectedToLinkTag, "Missing direction parameter");

      } else if (direction == "in") {
        if (!anotherLk->connectToLink (lk, false)) {
          throw new MapFileFormatException
              (connectedToLinkTag, "Can not make link-to-link connection");
        }

      } else if (direction == "out") {
        if (!lk->connectToLink (anotherLk, false)) {
          throw new MapFileFormatException
              (connectedToLinkTag, "Can not make link-to-link connection");
        }

      } else if (direction == "inout") {
        if (!lk->connectToLink (anotherLk, true)) {
          throw new MapFileFormatException
              (connectedToLinkTag, "Can not make link-to-link connection");
        }

      } else {
        throw new MapFileFormatException (connectedToLinkTag, "Unknown direction value");
      }
    }
  } visitor (map, lk);
  return MapReader ().readOneNamedElement (link, "connected-to-link", &visitor, false);
}

void LinkVisitor::parseLinkLoggingParameter (QString linkName, const QDomElement &link,
    edu::sharif::partov::nse::map::link::Link *lk) const
throw (MapFileFormatException *) {
  QString logEnabled = link.attribute ("log");
  if (!logEnabled.isNull ()) {
    if (logEnabled == "true") {
      if (!lk->setupPacketLogger (logPathTemplate, linkName)) {
        throw new MapFileFormatException (link, "Can not setup link's packet logger");
      }
    } else if (logEnabled == "false") {
      // log is not enabled by default...
    } else {
      throw new MapFileFormatException (link, "Unknown log value");
    }
  }
}

int LinkVisitor::extractLossRate (const QDomElement &link,
    edu::sharif::partov::nse::map::link::Link *lk) const
throw (MapFileFormatException *) {
  Percentage rate = 0;
  UnnamedFieldVisitor < Percentage > lVisitor
      (map->getVariableManager (), map, rate, "Can not set link's loss rate");
  UnnamedFieldRefVisitor < Percentage > lrVisitor
      (map->getVariableManager (), map, rate,
       &edu::sharif::partov::nse::map::ListManager::getListValue < Percentage >,
       "Can not set link's loss rate by list reference");

  edu::sharif::partov::nse::map::builder::ElementVisitor * visitors[]
      = {&lVisitor, &lrVisitor};
  const char *tags[] = {"loss", "loss-ref"};
  int unknownTagsCount = MapReader ().readOneUnnamedElement
      (link, tags, visitors, 2, false);

  if (!lk->setLossRate (rate)) {
    throw new MapFileFormatException (link, "Can not set link's loss rate");
  }
  return unknownTagsCount;
}

int LinkVisitor::extractLatency (const QDomElement &link,
    edu::sharif::partov::nse::map::link::Link *lk) const
throw (MapFileFormatException *) {
  Time latency = 10l;
  UnnamedFieldVisitor < Time > lVisitor
      (map->getVariableManager (), map, latency, "Can not set link's latency");
  UnnamedFieldRefVisitor < Time > lrVisitor
      (map->getVariableManager (), map, latency,
       &edu::sharif::partov::nse::map::ListManager::getListValue < Time >,
       "Can not set link's latency by list reference");

  edu::sharif::partov::nse::map::builder::ElementVisitor * visitors[]
      = {&lVisitor, &lrVisitor};
  const char *tags[] = {"latency", "latency-ref"};
  int unknownTagsCount = MapReader ().readOneUnnamedElement
      (link, tags, visitors, 2, false);

  if (!lk->setLatency (latency)) {
    throw new MapFileFormatException (link, "Can not set link's latency");
  }
  return unknownTagsCount;
}

int LinkVisitor::extractBandwidth (const QDomElement &link,
    edu::sharif::partov::nse::map::link::Link *lk) const
throw (MapFileFormatException *) {
  Bandwidth bandwidth = 10ull * 1024l * 1024l * 1024l;
  UnnamedFieldVisitor < Bandwidth > bVisitor
      (map->getVariableManager (), map, bandwidth, "Can not set link's bandwidth");
  UnnamedFieldRefVisitor < Bandwidth > brVisitor
      (map->getVariableManager (), map, bandwidth,
       &edu::sharif::partov::nse::map::ListManager::getListValue < Bandwidth >,
       "Can not set link's bandwidth by list reference");

  edu::sharif::partov::nse::map::builder::ElementVisitor * visitors[]
      = {&bVisitor, &brVisitor};
  const char *tags[] = {"bandwidth", "bandwidth-ref"};
  int unknownTagsCount = MapReader ().readOneUnnamedElement
      (link, tags, visitors, 2, false);
  if (!lk->setBandwidth (bandwidth)) {
    throw new MapFileFormatException (link, "Can not set link's bandwidth");
  }
  return unknownTagsCount;
}

}
}
}
}
}
}
