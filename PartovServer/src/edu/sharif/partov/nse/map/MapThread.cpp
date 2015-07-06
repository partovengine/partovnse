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

#include "MapThread.h"

#include "Map.h"
#include "Node.h"
#include "MapFactory.h"

#include "edu/sharif/partov/nse/server/Server.h"

#include "edu/sharif/partov/nse/map/builder/ListVisitor.h"
#include "edu/sharif/partov/nse/map/builder/LinkVisitor.h"
#include "edu/sharif/partov/nse/map/builder/SimpleNodeVisitor.h"
#include "edu/sharif/partov/nse/map/builder/PluginNodeVisitor.h"
#include "edu/sharif/partov/nse/map/builder/FsmVisitor.h"
#include "edu/sharif/partov/nse/map/builder/MapReader.h"

#include "edu/sharif/partov/nse/plugin/PluginScheduler.h"

#include "edu/sharif/partov/nse/util/ScalarLogger.h"
#include "edu/sharif/partov/nse/util/NonBlockingLocker.h"

#include <QDomDocument>
#include <QFile>
#include <QMap>
#include <QMutex>
#include <QSemaphore>

#include <QStateMachine>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

MapThread::MapThread (const QString &mapName, const QString &logPathTemplate, int index,
    QString creatorId) :
QThread (), lock (new QMutex ()), cond (new QSemaphore (0)),
map (new Map (mapName, logPathTemplate, index, creatorId, lock)),
scalarLogFilePath (logPathTemplate.left (logPathTemplate.lastIndexOf ('/') + 1)
+ "scalar.log"), running (false),
finalizeMapBeforeDeletion (false) {
  map->moveToThread (this);
}

MapThread::~MapThread () {
  delete cond;
  delete lock;
}

bool MapThread::waitForInitialization () {
  edu::sharif::partov::nse::util::NonBlockingLocker waitCondition (cond);
  return running;
}

void MapThread::run () {
  running = map->setup ();
  cond->release ();

  if (running) {
    MapFactory::getInstance ()->installMapScalarLogger
        (new edu::sharif::partov::nse::util::ScalarLogger (scalarLogFilePath));
    exec ();
    if (finalizeMapBeforeDeletion) {
      map->finalize (this);
    }
  }
  delete map;
}

void MapThread::init () {
  connect (MapFactory::getInstance (), SIGNAL (shuttingDown ()), this,
           SLOT (privilegedFinalize ()), Qt::DirectConnection);
}

void MapThread::quit () {
  lock->lock ();
  emit aboutToFinish (); /* @@ signal emitted @@ */
  lock->unlock ();

  QThread::quit ();
}

void MapThread::finalize () {
  wait ();

  /*
   * We are in the simulator thread and a QObjectCleanupHandler object has reference to us
   * from the main thread. So we must be removed from its list before being deleted.
   * Otherwise a race condition on deleting this object (going through destructor) and
   * dispatching the deleted signal event to the main thread will be present.
   */
  MapFactory::getInstance ()->removeMapFromManagedMaps (this);
  delete this;
}

void MapThread::privilegedFinalize () {
  if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
    qDebug ("Map is being deleted (privileged case)...");
  }
  //  lock->lock ();
  /**
   * It's not required to synchronized here because only this (main thread) and
   * simulators threads are eligible to quit the MapThread. And of course if we
   * reach this line, no simulator thread exists anymore :)
   */
  finalizeMapBeforeDeletion = true;
  //  lock->unlock ();
  quit ();
  wait ();
}

Map *MapThread::getMap () const {
  return map;
}

}
}
}
}
}
