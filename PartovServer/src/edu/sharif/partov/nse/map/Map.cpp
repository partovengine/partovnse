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

#include "Map.h"

#include "Node.h"
#include "MapFactory.h"
#include "ListManager.h"
#include "UnBoxable.h"
#include "VariableManager.h"
#include "RealtimeScheduler.h"
#include "CompacttimeScheduler.h"

#include "edu/sharif/partov/nse/server/Server.h"

#include "edu/sharif/partov/nse/map/builder/MapReader.h"
#include "edu/sharif/partov/nse/map/builder/ListVisitor.h"
#include "edu/sharif/partov/nse/map/builder/LinkVisitor.h"
#include "edu/sharif/partov/nse/map/builder/SimpleNodeVisitor.h"
#include "edu/sharif/partov/nse/map/builder/PluginNodeVisitor.h"
#include "edu/sharif/partov/nse/map/builder/FsmVisitor.h"

#include "edu/sharif/partov/nse/plugin/PluginScheduler.h"

#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QMutex>
#include <QWaitCondition>

#include <QStateMachine>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

const char *Map::MAP_VERSION = "3.3";

Map::Map (const QString &mapName, const QString &_logPathTemplate, int _index,
    QString _creatorId) :
logPathTemplate (_logPathTemplate), index (_index), listManager (
new ListManager (index)), variableManager (new VariableManager ()),
creatorId (_creatorId), pluginScheduler (NULL) {
  setObjectName (mapName);
}

Map::~Map () {
  delete listManager;
  delete variableManager;
  delete pluginScheduler;
}

Node *Map::retrieveNodeByName (const QString &name) const throw (NoSuchMapNodeException *) {
  Node *node = findChild < Node * > (name);
  if (node) {
    return node;
  }
  QString mesg = "There is no node with ``%1'' as its name in the ``%2'' map.";
  throw new NoSuchMapNodeException (mesg.arg (name).arg (objectName ()));
}

bool Map::setup () {
  try {
    if (!initializeMap ()) {
      return false;
    }
    pluginScheduler->start ();

  } catch (MapFileFormatException *e) {
    qWarning ("Wrong map file format: %s.map", qPrintable (objectName ()));
    e->printExceptionDescription ();
    delete e;
    return false;
  }
  return true;
}

bool Map::initializeMap () throw (MapFileFormatException *) {
  QDomDocument map ("map");
  if (!openMap (map)) {
    return false;
  }

  QDomElement root = map.documentElement ();

  QDomElement authorization = root.firstChildElement ("authorization");
  QDomElement fsm = root.firstChildElement ("fsm");
  QDomElement links = root.firstChildElement ("links");
  QDomElement nodes = root.firstChildElement ("nodes");
  QDomElement lists = root.firstChildElement ("lists");

  if (root.localName () != "map") {
    throw new MapFileFormatException
        (root, QString ("Root tag (%1) is wrong").arg (root.localName ()));
  }
  QString version = root.attribute ("version", "0");
  if (version != MAP_VERSION
      && version != "3.2" && version != "3.1" && version != "3.0") {
    throw new MapFileFormatException (root, "Version number");
  }
  if (root.attribute ("name") != objectName ()
      || root.attribute ("count").isNull () || nodes.isNull ()) {
    throw new MapFileFormatException
        (root, "Wrong map name, missing count parameter or missing nodes tag");
  }
  QString realtimeScheduling = root.attribute ("realtime");
  if (realtimeScheduling.isNull () || realtimeScheduling == "true") {
    MapFactory::getInstance ()->installMapEventScheduler (new RealtimeScheduler ());
  } else if (realtimeScheduling == "false") {
    MapFactory::getInstance ()->installMapEventScheduler (new CompacttimeScheduler ());
  } else {
    throw new MapFileFormatException
        (root, "Unknown scheduling algorithm is requested"
         " (realtime parameter of root tag can only take true/false values)");
  }
  pluginScheduler = new edu::sharif::partov::nse::plugin::PluginScheduler ();

  int size = root.childNodes ().size ();
  if (!authorization.isNull ()) {
    size--;
  }
  if (!fsm.isNull ()) {
    size--;
  }
  if (!links.isNull ()) {
    size--;
  }
  if (!lists.isNull ()) {
    size--;
    readLists (lists);
  }
  if (size != 1) {
    throw new MapFileFormatException (root, "Count of high level tags is wrong");
  }
  if (!links.isNull ()) {
    readLinks (links);
  }
  readNodes (nodes);
  if (!fsm.isNull ()) {
    readFSM (fsm);
  }
  if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
    qDebug ("Map ``%s'' created successfully.", qPrintable (objectName ()));
  }
  return true;
}

bool Map::openMap (QDomDocument &map) const {
  QFile mapFile (QString ("../maps/%1.map").arg (objectName ()));
  QString mapFileName = mapFile.fileName ();

  if (!mapFile.open (QIODevice::ReadOnly)) {
    qWarning ("Error in opening file ``%s.map'' for reading.", qPrintable (objectName ()));
    return false;
  }
  QString errorMsg;
  int errorLine;
  int errorColumn;
  if (!map.setContent (&mapFile, true, &errorMsg, &errorLine, &errorColumn)) {
    mapFile.close ();
    qWarning ("Error in initializing map from ``%s.map'' map file."
              " (message: %s; line: %d; column: %d)",
              qPrintable (objectName ()), qPrintable (errorMsg), errorLine, errorColumn);
    return false;
  }
  mapFile.close ();
  if (MapFactory::getInstance ()->isXmlSchemaValidationEnabled ()) {
    QUrl ptlSchemaUrl = QUrl::fromLocalFile ("../maps/map.xsd");
    QXmlSchema schema;
    if (!schema.load (ptlSchemaUrl)) {
      qCritical ("The map XML schema is invalid!");
      return false;
    }
    QXmlSchemaValidator validator (schema);
    if (!validator.validate (QUrl::fromLocalFile (mapFileName))) {
      qWarning ("The map file is invalid according to PTL XML schema!");
      return false;
    }
  }
  return true;
}

void Map::readLists (const QDomElement &lists) throw (MapFileFormatException *) {
  edu::sharif::partov::nse::map::builder::ListVisitor < QString > strVisitor
      (this, "Malformed string list's item value");
  edu::sharif::partov::nse::map::builder::ListVisitor <
      edu::sharif::partov::nse::network::address::EthernetMACAddress > macVisitor
      (this, "Malformed ethernet mac address list's item value");
  edu::sharif::partov::nse::map::builder::ListVisitor < QHostAddress > ipv4Visitor
      (this, "Malformed ipv4 address list's item value");
  edu::sharif::partov::nse::map::builder::ListVisitor < Percentage > percentageVisitor
      (this, "Malformed percentage list's item value");
  edu::sharif::partov::nse::map::builder::ListVisitor < Time > timeVisitor
      (this, "Malformed time list's item value");
  edu::sharif::partov::nse::map::builder::ListVisitor < Bandwidth > bandwidthVisitor
      (this, "Malformed bandwidth list's item value");
  edu::sharif::partov::nse::map::builder::ListVisitor < Size > sizeVisitor
      (this, "Malformed size list's item value");
  edu::sharif::partov::nse::map::builder::NamedElementVisitor * visitors[]
      = {
         &strVisitor, &macVisitor, &ipv4Visitor, &percentageVisitor,
         &timeVisitor, &bandwidthVisitor, &sizeVisitor
  };
  const char *tags[] = {"string", "mac", "ipv4", "percentage", "time", "bandwidth",
                        "size"};
  edu::sharif::partov::nse::map::builder::MapReader ().readNamedElements (lists, tags,
                                                                          visitors, 7);
}

void Map::readLinks (const QDomElement &links) throw (MapFileFormatException *) {
  edu::sharif::partov::nse::map::builder::LinkVisitor visitor (this, logPathTemplate);
  edu::sharif::partov::nse::map::builder::MapReader (variableManager).readNamedElements
      (links, "link", &visitor);
}

void Map::readNodes (const QDomElement &nodes) throw (MapFileFormatException *) {
  edu::sharif::partov::nse::map::builder::SimpleNodeVisitor svisitor
      (this, logPathTemplate);
  edu::sharif::partov::nse::map::builder::PluginNodeVisitor pvisitor
      (this, logPathTemplate);
  edu::sharif::partov::nse::map::builder::NamedElementVisitor * visitors[]
      = {&svisitor, &pvisitor};
  const char *tags[] = {"simple", "plugin"};
  edu::sharif::partov::nse::map::builder::MapReader (variableManager).readNamedElements
      (nodes, tags, visitors, 2);
}

void Map::readFSM (const QDomElement &fsmElement) throw (MapFileFormatException *) {
  edu::sharif::partov::nse::map::builder::FsmVisitor visitor
      (this, new QStateMachine (this));

  edu::sharif::partov::nse::map::builder::MapReader ().readNamedElements
      (fsmElement, "state", &visitor);
  visitor.setFsmInitialStateAndStartIt (fsmElement);
}

void Map::finalize (MapThread *mt) {
  MapFactory::getInstance ()->freeResources (mt, creatorId, objectName (), getMapIndex ());
}

int Map::getMapIndex () const {
  return index;
}

ListManager *Map::getListManager () const {
  return listManager;
}

VariableManager *Map::getVariableManager () const {
  return variableManager;
}

edu::sharif::partov::nse::plugin::PluginScheduler *Map::getPluginScheduler () const {
  return pluginScheduler;
}

}
}
}
}
}
