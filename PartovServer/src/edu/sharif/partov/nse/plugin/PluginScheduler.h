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

#ifndef PLUGINSCHEDULER_H_
#define PLUGINSCHEDULER_H_

#include <QObject>

template < class Key, class T >
class QMultiMap;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

class PluginNode;

/**
 * Name:              PluginScheduler
 * Parent:            QObject
 * Description:       The sole responsible class for providing tick signals for
 *                    all other plugins.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.plugin
 * Tags:              Singleton
 */
class PluginScheduler : public QObject {

  Q_OBJECT
private:
  QMultiMap < int, PluginNode * > *schedule;
  int timeCounter;
  int gcd, lcm;

public:
  PluginScheduler ();
  virtual ~PluginScheduler ();

  // tickInterval is in milli-seconds
  void registerSchedulee (PluginNode *plugin, int tickInterval);
  void start ();

private:
  Q_INVOKABLE void dispatchPluginsTickEvents (int);
};

}
}
}
}
}

#endif /* PLUGINSCHEDULER_H_ */
