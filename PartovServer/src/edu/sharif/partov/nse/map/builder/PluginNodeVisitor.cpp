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

#include "PluginNodeVisitor.h"

#include "edu/sharif/partov/nse/map/Map.h"
#include "edu/sharif/partov/nse/map/ListManager.h"
#include "edu/sharif/partov/nse/map/VariableManager.h"

#include "edu/sharif/partov/nse/plugin/PluginScheduler.h"
#include "edu/sharif/partov/nse/plugin/PluginNode.h"
#include "edu/sharif/partov/nse/plugin/PluginNodeFactory.h"

#include "MapReader.h"
#include "ElementVisitor.h"
#include "ListVisitor.h"
#include "UnnamedFieldVisitor.h"
#include "UnnamedFieldRefVisitor.h"

#include <QDomDocument>
#include <QStringList>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

PluginNodeVisitor::PluginNodeVisitor (Map *_map, QString logPathTemplate) :
    NodeVisitor (_map, logPathTemplate) {
}

PluginNodeVisitor::~PluginNodeVisitor () {
}

void PluginNodeVisitor::processNamedElement (QString nodeName, QDomElement node)
    throw (MapFileFormatException *) {
  QString pluginIdentifier = node.attribute ("plugin-identifier");
  if (pluginIdentifier.isNull ()) {
    throw new MapFileFormatException (node, "Missing plugin-identifier parameter");
  }
  const int allChildrenCount = node.childNodes ().size ();
  int tickInterval;
  int validChildrenCount = allChildrenCount - extractTickInterval (node, tickInterval);
  edu::sharif::partov::nse::plugin::PluginNode *pluginNode;
  try {
    pluginNode =
        edu::sharif::partov::nse::plugin::PluginNodeFactory::getInstance ()->instantiatePluginNode (
            pluginIdentifier, nodeName, map);
  } catch (edu::sharif::partov::nse::plugin::PluginNotInstalledException *e) {
    throw new MapFileFormatException (node,
        QString ("Plugin identifier (%1) is unknown").arg (pluginIdentifier), e);
  }
  mapNode = pluginNode;
  if (tickInterval != -1) { // optional parameter
    map->getPluginScheduler ()->registerSchedulee (pluginNode, tickInterval);
  }
  QDomElement parameters = node.firstChildElement ("parameters");
  if (!parameters.isNull ()) {
    expectedChildrenCount = 2 + validChildrenCount;
    readParameters (parameters,
        static_cast < edu::sharif::partov::nse::plugin::PluginNode * > (mapNode));
  } else {
    expectedChildrenCount = 1 + validChildrenCount;
  }
  return NodeVisitor::processNamedElement (nodeName, node);
}

int PluginNodeVisitor::extractTickInterval (const QDomElement &node,
    int &tickIntervalRet) const throw (MapFileFormatException *) {
  tickIntervalRet = -1; // not set (optional)

  edu::sharif::partov::nse::map::Time tickInterval;
  UnnamedFieldVisitor < edu::sharif::partov::nse::map::Time > tiVisitor (
      map->getVariableManager (), map, tickInterval,
      "Can not set plugin's tick-interval");
  UnnamedFieldRefVisitor < edu::sharif::partov::nse::map::Time > tirVisitor (
      map->getVariableManager (), map, tickInterval,
      &edu::sharif::partov::nse::map::ListManager::getListValue < Time >,
      "Can not set plugin's tick-interval by list reference");
  edu::sharif::partov::nse::map::builder::ElementVisitor *visitors[] = { &tiVisitor,
      &tirVisitor };
  const char *tags[] = { "tick-interval", "tick-interval-ref" };
  int unknownTags = MapReader ().readOneUnnamedElement (node, tags, visitors, 2, false);
  if (node.childNodes ().size () - unknownTags == 1) {
    if (tickInterval > INT_MAX) {
      throw new MapFileFormatException (node, "Too large tick-interval value");
    }
    tickIntervalRet = tickInterval;
  }
  return unknownTags;
}

void PluginNodeVisitor::readParameters (const QDomElement &parameters,
    edu::sharif::partov::nse::plugin::PluginNode *pluginNode)
        throw (MapFileFormatException *) {
  class ParameterVisitor : public NamedElementVisitor {
  private:
    edu::sharif::partov::nse::plugin::PluginNode *pluginNode;

  public:
    ParameterVisitor (Map *_map,
        edu::sharif::partov::nse::plugin::PluginNode *_pluginNode) :
        NamedElementVisitor (_map), pluginNode (_pluginNode) {
    }

    virtual void processNamedElement (QString paramName, QDomElement parameter)
        throw (MapFileFormatException *) {
      QStringList values;

      UnnamedFieldVisitor < QList < QString > > vVisitor (map->getVariableManager (), map,
          values, "Can not set plugin's parameter value");
      UnnamedFieldRefVisitor < QList < QString > > vrVisitor (map->getVariableManager (),
          map, values, NULL, "Can not set plugin's parameter value by list reference");

      edu::sharif::partov::nse::map::builder::ElementVisitor *visitors[] = { &vVisitor,
          &vrVisitor };
      const char *tags[] = { "value", "value-ref" };
      MapReader ().readUnnamedElements (parameter, tags, visitors, 2);
      if (!pluginNode->setParameter (paramName, values)) {
        throw new MapFileFormatException (parameter, "Malformed parameter value");
      }
    }
  } visitor (map, pluginNode);
  MapReader ().readNamedElements (parameters, "param", &visitor);
}

}
}
}
}
}
}
