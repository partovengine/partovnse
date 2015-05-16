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

#ifndef PLUGINNODEFACTORY_H_
#define PLUGINNODEFACTORY_H_

#include <QList>

#include "PluginNotInstalledException.h"

class QString;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {

namespace map {
class Map;
}
namespace plugin {

class PluginNode;
class AbstractPluginNodeInstantiator;

/**
 * Name:              PluginNodeFactory
 * Parent:            Node
 * Description:       The factory of plugin nodes.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              Singleton; Factory;
 */
class PluginNodeFactory {
private:
  static PluginNodeFactory *me;

  QList < AbstractPluginNodeInstantiator * > installedPluginInstantiators;

  PluginNodeFactory ();
  ~PluginNodeFactory ();

public:
  bool isPluginInstalled (const QString &pluginName) const;
  PluginNode *instantiatePluginNode (const QString &pluginName, const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent) const
          throw (PluginNotInstalledException *);

  static void init ();
  static PluginNodeFactory *getInstance ();
  static void destroy ();
};

}
}
}
}
}

#endif /* PLUGINNODEFACTORY_H_ */
