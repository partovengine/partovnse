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

#include "PluginScheduler.h"

#include "PluginNode.h"

#include "edu/sharif/partov/nse/map/MapFactory.h"
#include "edu/sharif/partov/nse/map/Scheduler.h"

#include <QMultiMap>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

PluginScheduler::PluginScheduler () :
QObject (), schedule (new QMultiMap < int, PluginNode * > ()), timeCounter (0),
lcm (1) {
}

PluginScheduler::~PluginScheduler () {
  delete schedule;
}

/**
 * a must be >= b
 */
int calc_gcd_constrained (int a, int b) {
  if (b == 0) {
    return a;
  } else {
    return calc_gcd_constrained (b, a % b);
  }
}

int calc_gcd (int a, int b) {
  if (a > b) {
    return calc_gcd_constrained (a, b);
  } else {
    return calc_gcd_constrained (b, a);
  }
}

int calc_lcm (int a, int b) {
  int r = a / calc_gcd (a, b);
  return r * b;
}

template < typename T >
T left_fold (QList < T > list, T (*func) (T, T), T initialValue) {
  T res = initialValue;

  foreach (T t, list) {
    res = func (initialValue, t);
  }
  return res;
}

void PluginScheduler::start () {
  if (schedule->isEmpty ()) {
    return;
  }
  gcd = left_fold (schedule->keys (), calc_gcd, 0);

  QMultiMap < int, PluginNode * > *normalizedSchedule =
      new QMultiMap < int, PluginNode * > ();
  for (QMultiMap < int, PluginNode * >::iterator it = schedule->begin ();
      it != schedule->end (); ++it) {
    int tickInterval = it.key ();
    PluginNode *plugin = it.value ();
    int factor = tickInterval / gcd;
    normalizedSchedule->insert (factor, plugin);
  }
  delete schedule;
  schedule = normalizedSchedule;

  lcm = left_fold (schedule->keys (), calc_lcm, 1);
  edu::sharif::partov::nse::map::MapFactory::getInstance ()->getMapEventScheduler ()
      ->scheduleEvent (gcd, this, "dispatchPluginsTickEvents");
}

void PluginScheduler::dispatchPluginsTickEvents (int) {
  // schedule next recurring tick event
  edu::sharif::partov::nse::map::MapFactory::getInstance ()->getMapEventScheduler ()
      ->scheduleEvent (gcd, this, "dispatchPluginsTickEvents");
  timeCounter++;
  for (QMultiMap < int, PluginNode * >::iterator it = schedule->begin ();
      it != schedule->end (); ++it) {
    int tickInterval = it.key ();
    PluginNode *plugin = it.value ();
    if (timeCounter % tickInterval == 0) {
      plugin->tick ();
    }
  }
  timeCounter = timeCounter % lcm;
}

void PluginScheduler::registerSchedulee (PluginNode *plugin, int tickInterval) {
  Q_ASSERT (tickInterval > 0);
  schedule->insert (tickInterval, plugin);
}

}
}
}
}
}
