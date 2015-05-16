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

#ifndef MAPINSTANTIATOR_H_
#define MAPINSTANTIATOR_H_

#include "MapExceptions.h"

template < class Key, class T >
class QHash;

template < class T >
class QStack;

class QHostAddress;

class QDomElement;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {

namespace usermanagement {

class User;
class Group;
}

namespace map {

class MapThread;

/**
 * Name:              MapInstantiator
 * Parent:            None
 * Description:       Encapsulate an allocation table and manage map instance such that each
 *                    user can only create one instance of each map.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Builder; Singleton per Map description;
 */
class MapInstantiator {
private:
  enum MapResumabilityOptions {
    Instantiator, Anyone

  } resumableBy; // by default only the Instantiator could resume the map simulation
  int maxIpu; // maximum instances per user

  QList < edu::sharif::partov::nse::usermanagement::Group > *instantiators; // null list means that anyone could instantiate the map
  QList < QHostAddress > *allowedHosts; // null list means that any host could connect to the map

  bool *allocationTable;
  int count;

  QHash < QString, QStack < MapThread * > * > *maps;

public:
  MapInstantiator (int _count);
  virtual ~MapInstantiator ();

  bool initializeAuthorizationConfiguration (const QDomElement &authorization);

  int getCountOfAllowedSimultaneousMapInstances () const;

  void releaseResource (MapThread *map, QString creatorId, int handle);

  MapThread *instantiateMap (edu::sharif::partov::nse::usermanagement::User user,
      QString mapFileName, QString creatorId, QHostAddress host)
          throw (MapNotFoundException *, MaximumMapInstancesPerUserViolatedException *,
          OutOfResourceException *, AuthorizationException *);
  MapThread *retrieveMap (edu::sharif::partov::nse::usermanagement::User user,
      QString mapFileName, QString creatorId, QHostAddress host)
          throw (MapNotFoundException *, AuthorizationException *);

protected:
  bool canUserInstantiateANewMapInstance (QString user) const;

  /**
   * @return the handle (index) of the acquired resource if any, otherwise return -1.
   */
  int acquireFirstFreeResourceHandle ();

  bool isUserAuthorizedForInstantiation (
      edu::sharif::partov::nse::usermanagement::User user) const;
  bool isUserAuthorizedForResumption (edu::sharif::partov::nse::usermanagement::User user,
      QString creatorId) const;
  bool isHostAuthorizedToBeConnected (const QHostAddress &ip) const;

private:
  bool initializeAuthorizationParameters (const QDomElement &authorization);
  bool initializeMapAllowedInstantiatorGroups (const QDomElement &instantiator);
  bool initializeConnectionRestrictionHostsConfiguration (
      const QDomElement &connectionRestriction);

  bool hasAnyBusyResource () const; // XXX: non-used method
};

}
}
}
}
}

#endif /* MAPINSTANTIATOR_H_ */
