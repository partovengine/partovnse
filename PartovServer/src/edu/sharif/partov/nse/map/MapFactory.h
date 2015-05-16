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

#ifndef MAPFACTORY_H_
#define MAPFACTORY_H_

#include <QString>

#include "MapExceptions.h"

class QMutex;
class QWaitCondition;
class QObjectCleanupHandler;

template < class Key, class T >
class QMap;

class QHostAddress;

template < class T >
class QThreadStorage;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace usermanagement {

class User;
}
namespace plugin {

class Router;
}
namespace util {

class ScalarLogger;
}
namespace map {

class MapThread;
class MapInstantiator;
class Scheduler;

/**
 * Name:              MapFactory
 * Parent:            QObject
 * Description:       The factory of map instances.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Factory;
 */
class MapFactory : public QObject {
  Q_OBJECT
private:
  static MapFactory *me;

  QThreadStorage < edu::sharif::partov::nse::util::ScalarLogger * > *mapScalarLoggers;
  QThreadStorage < Scheduler * > *mapEventScheduler;

  QMutex *mutex;
  QObjectCleanupHandler *mapsCleanupHandler;

  QMap < QString, MapInstantiator * > *mapAllocationTable;

  MapThread *routersMap;

  bool xmlSchemaValidation;

  MapFactory (bool xmlSchemaValidation);
  virtual ~MapFactory ();

  void waitUntilAllResourcesBeReleased ();

  MapInstantiator
  *createOrRetrieveMapInstantiator (QString mapFileName) throw (MapNotFoundException *);

signals:
  void shuttingDown ();

public:
  MapThread *createOrRetrieveMap (
      edu::sharif::partov::nse::usermanagement::User user,
      QString mapFileName, QString creatorId, bool needNewMap, QHostAddress host,
      bool finalizeMap) throw (MapNotFoundException *,
      MaximumMapInstancesPerUserViolatedException *, OutOfResourceException *,
      AuthorizationException *);

  void freeResources (MapThread *map, QString creatorId, const QString &mapName,
      int index);
  void removeMapFromManagedMaps (MapThread *map);

  bool setupAdapterRoutersMap ();
  void shutdownAdapterRoutersMap ();
  edu::sharif::partov::nse::plugin::Router *getAdapterRouterNode (
      QString deviceName) const;

  static bool init ();
  static MapFactory *getInstance ();
  void destroy ();

  void installMapScalarLogger (
      edu::sharif::partov::nse::util::ScalarLogger *scalarLogger);
  /**
   * Only objects from the MapThread's thread can call this method. Also not in their
   * destructors because the scalar logger will be removed when the map thread is stoppped.
   */
  edu::sharif::partov::nse::util::ScalarLogger *getMapScalarLogger () const;

  void installMapEventScheduler (Scheduler *scheduler);
  Scheduler *getMapEventScheduler () const;

  bool isXmlSchemaValidationEnabled () const {
    return xmlSchemaValidation;
  }
};

}
}
}
}
}

#endif /* MAPFACTORY_H_ */
