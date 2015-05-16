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

#ifndef PLUGINNODEVISITOR_H_
#define PLUGINNODEVISITOR_H_

#include "NodeVisitor.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

class PluginNode;
}
namespace map {
namespace builder {

/**
 * Name:              PluginNodeVisitor
 * Parent:            NodeVisitor
 * Description:       Visitor for plugin nodes
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor;
 */
class PluginNodeVisitor : public NodeVisitor {
public:
  PluginNodeVisitor (Map *_map, QString logPathTemplate);
  virtual ~PluginNodeVisitor ();

  virtual void processNamedElement (QString nodeName, QDomElement node)
      throw (MapFileFormatException *);

private:
  void readParameters (const QDomElement &parameters,
      edu::sharif::partov::nse::plugin::PluginNode *pluginNode)
          throw (MapFileFormatException *);

  /**
   * @param tick-interval in micro-seconds or -1 if parameter was not present (as it's optional)
   * @return count of unknown seen tags
   */
  int extractTickInterval (const QDomElement &node, int &tickInterval) const
      throw (MapFileFormatException *);
};

}
}
}
}
}
}

#endif /* PLUGINNODEVISITOR_H_ */
