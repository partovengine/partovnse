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

#include "MapFactory.h"

#include "MapThread.h"
#include "Map.h"
#include "MapInstantiator.h"
#include "Scheduler.h"

#include "edu/sharif/partov/nse/server/Server.h"

#include "edu/sharif/partov/nse/usermanagement/User.h"

#include "edu/sharif/partov/nse/plugin/Router.h"

#include "edu/sharif/partov/nse/util/ScalarLogger.h"
#include "edu/sharif/partov/nse/util/NonBlockingLocker.h"

#include <QMap>
#include <QMutex>
#include <QDateTime>
#include <QWaitCondition>
#include <QMutexLocker>
#include <QHostAddress>
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QObjectCleanupHandler>
#include <QSettings>
#include <QThreadStorage>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

MapFactory *MapFactory::me;

MapFactory::MapFactory (bool _xmlSchemaValidation) :
mapScalarLoggers (
new QThreadStorage < edu::sharif::partov::nse::util::ScalarLogger * > ()),
mapEventScheduler (new QThreadStorage < Scheduler * > ()), mutex (new QMutex ()),
mapsCleanupHandler (new QObjectCleanupHandler ()),
mapAllocationTable (new QMap < QString, MapInstantiator * > ()),
xmlSchemaValidation (_xmlSchemaValidation) {
}

MapFactory::~MapFactory () {

  foreach (MapInstantiator *mi, *mapAllocationTable) {
    delete mi;
  }
  delete mapAllocationTable;
  delete mutex;
  delete mapsCleanupHandler;
  delete mapScalarLoggers;
  delete mapEventScheduler;
}

MapThread *MapFactory::createOrRetrieveMap (
    edu::sharif::partov::nse::usermanagement::User user, QString mapFileName,
    QString creatorId, bool needNewMap, QHostAddress host, bool finalizeMap)
throw (MapNotFoundException *, MaximumMapInstancesPerUserViolatedException *,
    OutOfResourceException *, AuthorizationException *) {
  edu::sharif::partov::nse::util::NonBlockingLocker locker (mutex, false);
  if (!locker.isLocked ()) {
    return NULL;
  }
  if (needNewMap) {
    if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
      qDebug ("Creating map from the ``%s.map'' map file by ``%s'' user.",
              qPrintable (mapFileName), qPrintable (creatorId));
    }
    MapInstantiator *mi = createOrRetrieveMapInstantiator (mapFileName);
    MapThread *map = mi->instantiateMap (user, mapFileName, creatorId, host);
    mapsCleanupHandler->add (map);
    return map;

  } else {
    if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
      qDebug ("The ``%s'' user is connecting to an existing instance of the ``%s'' map,"
              " instantiated by ``%s'' user.",
              qPrintable (user.getUserName ()), qPrintable (mapFileName),
              qPrintable (creatorId));
    }
    MapInstantiator *mi = createOrRetrieveMapInstantiator (mapFileName);
    MapThread *map = mi->retrieveMap (user, mapFileName, creatorId, host);
    if (finalizeMap) {
      if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
        qDebug ("Deleting map (MapFactory)");
      }
      mi->releaseResource (map, creatorId, map->getMap ()->getMapIndex ());
      map->quit ();
    }
    return map;
  }
}

void MapFactory::removeMapFromManagedMaps (MapThread *map) {
  QMutexLocker locker (mutex);

  mapsCleanupHandler->remove (map);
}

MapInstantiator *MapFactory::createOrRetrieveMapInstantiator (QString mapFileName)
throw (MapNotFoundException *) {
  QMap < QString, MapInstantiator * >::iterator it =
      mapAllocationTable->find (mapFileName);
  if (it == mapAllocationTable->end ()) {
    QDomDocument map ("Partov");
    QFile mapFile (QString ("../maps/%1.map").arg (mapFileName));

    if (!mapFile.open (QIODevice::ReadOnly)) {
      throw new MapNotFoundException
          (QString ("Error in opening file ``%1.map'' for reading.").arg (mapFileName));
    }
    QString errorMsg;
    int errorLine, errorColumn;
    if (!map.setContent (&mapFile, true, &errorMsg, &errorLine, &errorColumn)) {
      mapFile.close ();
      throw new MapNotFoundException
          (QString ("Error in initializing map from ``%1.map'' map file."
                    " (message: %2; line: %3; column: %4)")
           .arg (mapFileName).arg (errorMsg).arg (errorLine).arg (errorColumn));
    }
    mapFile.close ();

    QDomElement root = map.documentElement ();
    int count = 1;
    QString version = root.attribute ("version", "0");
    // TODO: This must be done within Map class
    if (root.localName () != "map"
        || (version != Map::MAP_VERSION &&
        version != "3.3" &&
        version != "3.2" && version != "3.1" && version != "3.0")
        || root.attribute ("name") != mapFileName
        || (count = root.attribute ("count").toInt ()) <= 0) {
      throw new MapNotFoundException
          (QString ("Error in format of the ``%1.map'' map file (in map factory).")
           .arg (mapFileName));
    }

    QDomElement authorization = root.firstChildElement ("authorization");
    MapInstantiator *mi = new MapInstantiator (count);
    if (authorization.isNull ()) {
      // no authorization tag :) use default values...
    } else if (!mi->initializeAuthorizationConfiguration (authorization)) {
      delete mi;
      throw new MapNotFoundException
          (QString ("Error in format of authorization configuration of"
                    " the ``%1.map'' map file (in map factory).")
           .arg (mapFileName));
    }
    mapAllocationTable->insert (mapFileName, mi);
    return mi;

  } else {
    return it.value ();
  }
}

void MapFactory::freeResources (MapThread *map, QString creatorId, const QString &mapName,
    int index) {
  QMutexLocker locker (mutex);

  QMap < QString, MapInstantiator * >::iterator it = mapAllocationTable->find (mapName);
  if (it == mapAllocationTable->end ()) {
    qFatal ("--- Panic: A non-existed map requested for releasing its resources!!"
            " Map name is ``%s''", qPrintable (mapName));
    return;
  }
  MapInstantiator *mi = it.value ();
  if (index < 0 || index >= mi->getCountOfAllowedSimultaneousMapInstances ()) {
    qFatal ("--- Panic: A map requested for releasing resources with out-of-range index!!"
            " Map name is ``%s'' and its index is %d", qPrintable (mapName), index);
    return;
  }
  mi->releaseResource (map, creatorId, index);
}

bool MapFactory::setupAdapterRoutersMap () {
  QString path = QString ("../logs/system-logs/routers/%1")
      .arg (QDateTime::currentDateTime ().toString ("yyyyMMdd_hhmmss"));
  bool ok = QDir ().mkpath (path);
  if (edu::sharif::partov::nse::server::Server::isVerbose ()) {
    qDebug ("Creating log folder: %s", (ok ? "Done." : "Failed."));
  }
  routersMap = new edu::sharif::partov::nse::map::MapThread
      ("../config/routers", QString ("%1/%2.pcap").arg (path), 0, "system");
  routersMap->start ();
  if (!routersMap->waitForInitialization ()) {
    routersMap->wait ();
    return false;
  }
  return true;
}

edu::sharif::partov::nse::plugin::Router *MapFactory::getAdapterRouterNode (
    QString deviceName) const {
  return routersMap->getMap ()
      ->findChild < edu::sharif::partov::nse::plugin::Router * > (deviceName);
}

void MapFactory::shutdownAdapterRoutersMap () {
  routersMap->quit ();
  routersMap->wait ();
  delete routersMap;
}

bool MapFactory::init () {
  QSettings config ("../config/config.ini", QSettings::IniFormat);
  config.beginGroup ("map");
  QString xsv = config.value ("xml-schema-validation", "enable").toString ();
  bool xsvb;
  if (xsv == "enable") {
    xsvb = true;
  } else if (xsv == "disable") {
    xsvb = false;
  } else {
    qCritical ("The xml-schema-validation setting in config file is malformed.");
    return false;
  }
  config.endGroup ();

  MapFactory::me = new MapFactory (xsvb);
  return true;
}

MapFactory *MapFactory::getInstance () {
  return MapFactory::me;
}

void MapFactory::destroy () {
  /*
   * We do not need to synch using
   * \code
   *      QMutexLocker locker (mutex);
   * \endcode
   * since all simulators and server objects are deleted and so no map can be within
   * a finalize method or being created and so no one may access the mapsCleanupHandler
   * object and so there is no race.
   */
  emit
  shuttingDown (); /* @@ signal emitted @@ */
  mapsCleanupHandler->clear ();

  shutdownAdapterRoutersMap ();

  delete this;
  MapFactory::me = NULL;
}

void MapFactory::installMapScalarLogger (
    edu::sharif::partov::nse::util::ScalarLogger *scalarLogger) {
  mapScalarLoggers->setLocalData (scalarLogger);
}

edu::sharif::partov::nse::util::ScalarLogger *MapFactory::getMapScalarLogger () const {
  return mapScalarLoggers->localData ();
}

void MapFactory::installMapEventScheduler (Scheduler *scheduler) {
  mapEventScheduler->setLocalData (scheduler);
}

Scheduler *MapFactory::getMapEventScheduler () const {
  return mapEventScheduler->localData ();
}

}
}
}
}
}
