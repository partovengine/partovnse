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

#ifndef PLUGINNODEINSTANTIATOR_H_
#define PLUGINNODEINSTANTIATOR_H_

#include <QString>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {

namespace map {
class Map;
}
namespace plugin {

/**
 * Name:              AbstractPluginNodeInstantiator
 * Parent:            None
 * Description:       The interface of a factory which know how to build an instance
 *                    of a plugin.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              Singleton per Installed Plugin; Factory;
 */
class AbstractPluginNodeInstantiator {

public:

  virtual ~AbstractPluginNodeInstantiator () {
  }

  virtual const QString &getName () const = 0;

  virtual PluginNode *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent) const = 0;
};

/**
 * Name:              PluginNodeInstantiator
 * Parent:            AbstractPluginNodeInstantiator
 * Description:       A factory which know how to build an instance of a plugin.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              Singleton per Installed Plugin; Factory; Template;
 */
template < typename Plugin >
class PluginNodeInstantiator : AbstractPluginNodeInstantiator {

private:
  typedef Plugin *(*NativeInstantiator) (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent);

  const QString name;
  NativeInstantiator instantiator;

  PluginNodeInstantiator (const QString &_name, NativeInstantiator nativeInstantiator) :
      name (_name), instantiator (nativeInstantiator) {
  }

  friend class PluginNodeFactory;

public:

  virtual const QString &getName () const {
    return name;
  }

  virtual Plugin *instantiatePluginNode (const QString &nodeName,
      edu::sharif::partov::nse::map::Map *parent) const {
    return instantiator (nodeName, parent);
  }
};

}
}
}
}
}

#endif /* PLUGINNODEINSTANTIATOR_H_ */
