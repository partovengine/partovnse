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

#include "MapInstantiator.h"

#include "MapThread.h"

#include "edu/sharif/partov/nse/usermanagement/User.h"
#include "edu/sharif/partov/nse/usermanagement/Group.h"

#include <string.h>

#include <QHash>
#include <QStack>
#include <QDir>
#include <QDateTime>

#include <QHostAddress>
#include <QDomElement>

#include <cstddef>
#include <string>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

MapInstantiator::MapInstantiator (int _count) :
resumableBy (Instantiator), maxIpu (1), instantiators (0), allowedHosts (0), allocationTable (
new bool[_count]), count (_count), maps (
new QHash < QString, QStack < MapThread * > * > ()) {
  for (int i = 0; i < count; ++i) {
    allocationTable[i] = false;
  }
}

MapInstantiator::~MapInstantiator () {
  delete[] allocationTable;

  foreach (QStack <MapThread *> *stack, maps->values ()) {
    delete stack;
  }
  delete maps;
  if (instantiators) {
    delete instantiators;
  }
  if (allowedHosts) {
    delete allowedHosts;
  }
}

bool MapInstantiator::initializeAuthorizationConfiguration (
    const QDomElement &authorization) {
  if (!initializeAuthorizationParameters (authorization)) {
    return false;
  }

  QDomElement instantiator = authorization.firstChildElement ("instantiator");
  QDomElement connectionRestriction = authorization.firstChildElement
      ("connection-restriction");
  int size = authorization.childNodes ().size ();
  if (!instantiator.isNull ()) {
    if (!initializeMapAllowedInstantiatorGroups (instantiator)) {
      return false;
    }
    size--;
  }
  if (!connectionRestriction.isNull ()) {
    if (!initializeConnectionRestrictionHostsConfiguration (connectionRestriction)) {
      return false;
    }
    size--;
  }
  if (size == 0) {
    return true;
  } else {
    return false;
  }
}

bool MapInstantiator::initializeConnectionRestrictionHostsConfiguration (
    const QDomElement &connectionRestriction) {
  allowedHosts = new QList < QHostAddress > ();
  QDomNode hostNode = connectionRestriction.firstChild ();
  while (!hostNode.isNull ()) {
    QDomElement host = hostNode.toElement ();
    if (!host.isNull ()) {
      if (host.localName () != "host") {
        qWarning ("Authorization connection-restriction tag contains non-host tag.");
        return false;
      }
      QString hostValue = host.text ();
      if (hostValue.isNull ()) {
        qWarning ("Authorization > connection-restriction > host value is invalid.");
        return false;
      }
      QHostAddress hostIP (hostValue);
      if (hostIP.isNull ()) {
        qWarning ("Authorization > connection-restriction >"
                  " host has to contain an IP address.");
        return false;
      }
      allowedHosts->append (hostIP);
    }
    hostNode = hostNode.nextSibling ();
  }
  if (allowedHosts->isEmpty ()) {
    return false;
  } else {
    return true;
  }
}

bool MapInstantiator::initializeMapAllowedInstantiatorGroups (
    const QDomElement &instantiator) {
  instantiators = new QList < edu::sharif::partov::nse::usermanagement::Group > ();
  QDomNode groupNode = instantiator.firstChild ();
  while (!groupNode.isNull ()) {
    QDomElement group = groupNode.toElement ();
    if (!group.isNull ()) {
      if (group.localName () != "group") {
        qWarning ("Authorization instantiator tag contains non-group tag.");
        return false;
      }
      QString groupValue = group.text ();
      if (groupValue.isNull ()) {
        qWarning ("Authorization > instantiator > group tag value is invalid.");
        return false;
      }
      instantiators->append
          (edu::sharif::partov::nse::usermanagement::Group (groupValue));
    }
    groupNode = groupNode.nextSibling ();
  }
  if (instantiators->isEmpty ()) {
    return false;
  } else {
    return true;
  }
}

bool MapInstantiator::initializeAuthorizationParameters (
    const QDomElement &authorization) {
  int size = authorization.attributes ().size ();
  QString resumableByStr = authorization.attribute ("resumable-by");
  if (!resumableByStr.isNull ()) {
    if (resumableByStr == "instantiator") {
      resumableBy = Instantiator;
    } else if (resumableByStr == "anyone") {
      resumableBy = Anyone;
    } else {
      qWarning ("Unsupported value for ``resumable-by'' parameter in authorization tag"
                " (value: %s)", qPrintable (resumableByStr));
      return false;
    }
    size--;
  }
  QString maxIpuStr = authorization.attribute ("max-ipu");
  if (!maxIpuStr.isNull ()) {
    bool ok;
    maxIpu = maxIpuStr.toInt (&ok);
    if (!ok || maxIpu <= 0) {
      qWarning ("The max-ipu has to be a positive integer.");
      return false;
    }
    size--;
  }
  if (size != 0) {
    qWarning ("Authorization tag only supports"
              " ``resumable-by'' and ``max-ipu'' parameters.");
    return false;
  }
  return true;
}

int MapInstantiator::getCountOfAllowedSimultaneousMapInstances () const {
  return count;
}

bool MapInstantiator::hasAnyBusyResource () const {
  for (int i = 0; i < count; ++i) {
    if (allocationTable[i]) {
      return true;
    }
  }
  return false;
}

bool MapInstantiator::canUserInstantiateANewMapInstance (QString user) const {
  QHash < QString, QStack < MapThread * > * >::iterator it = maps->find (user);
  return it == maps->end () || (*it)->size () < maxIpu;
}

int MapInstantiator::acquireFirstFreeResourceHandle () {
  for (int i = 0; i < count; ++i) {
    if (allocationTable[i] == false) {
      allocationTable[i] = true;
      return i;
    }
  }
  return -1;
}

bool MapInstantiator::isUserAuthorizedForInstantiation (
    edu::sharif::partov::nse::usermanagement::User user) const {
  if (!instantiators) {
    return true;
  }

  foreach (edu::sharif::partov::nse::usermanagement::Group group, *instantiators) {
    if (user.isInGroup (group)) {
      return true;
    }
  }
  return false;
}

bool MapInstantiator::isUserAuthorizedForResumption (
    edu::sharif::partov::nse::usermanagement::User user, QString creatorId) const {
  switch (resumableBy) {
  case Instantiator:
    return user.getUserName () == creatorId;
  case Anyone:
    return true;
  default:
    return false;
  }
}

bool MapInstantiator::isHostAuthorizedToBeConnected (const QHostAddress &ip) const {
  if (!allowedHosts) {
    return true;
  }
  return allowedHosts->contains (ip);
}

void MapInstantiator::releaseResource (MapThread *map, QString creatorId, int handle) {
  QHash < QString, QStack < MapThread * > * >::iterator it = maps->find (creatorId);
  if (it != maps->end ()) {
    QStack < MapThread * > *stack = *it;
    if (stack && stack->top () == map) {
      stack->pop ();
      allocationTable[handle] = false;
    }
  }
}

MapThread *MapInstantiator::instantiateMap (
    edu::sharif::partov::nse::usermanagement::User user, QString mapFileName,
    QString creatorId, QHostAddress host) throw (MapNotFoundException *,
    MaximumMapInstancesPerUserViolatedException *, OutOfResourceException *,
    AuthorizationException *) {
  if (!isUserAuthorizedForInstantiation (user)) {
    throw new AuthorizationException
        (QString ("The ``%1'' user is not authorized to instantiate ``%2'' map."
                  "This map could only be instantiated by limited groups.")
         .arg (creatorId).arg (mapFileName));
  }
  if (!isHostAuthorizedToBeConnected (host)) {
    throw new AuthorizationException
        (QString ("The ``%1'' host is not authorized to instantiate ``%2'' map."
                  "This map could only be instantiated from limited IP addresses.")
         .arg (host.toString ()).arg (mapFileName));
  }
  if (!canUserInstantiateANewMapInstance (creatorId)) {
    throw new MaximumMapInstancesPerUserViolatedException
        (QString ("Any user can only create %1 instance(s) of ``%2'' map."
                  " User ``%3'' already did that.")
         .arg (maxIpu).arg (mapFileName).arg (creatorId));
  }
  int i = acquireFirstFreeResourceHandle ();
  if (i == -1) {
    throw new OutOfResourceException
        (QString ("Can not instantiate another map from ``%1.map'' map file."
                  " Out of resources.").arg (mapFileName));
  }
  QString path = QString ("../logs/%1/%2/%3").arg (mapFileName)
      .arg (creatorId).arg (QDateTime::currentDateTime ().toString ("yyyyMMdd_hhmmss"));
  QDir ().mkpath (path);
  MapThread *map = new MapThread
      (mapFileName, QString ("%1/%2.pcap").arg (path), i, creatorId);
  map->init ();
  map->start ();
  if (!map->waitForInitialization ()) {
    map->wait ();
    delete map;
    allocationTable[i] = false;

    throw new MapNotFoundException
        (QString ("Error in format of the ``%1.map'' map file (in map instantiator).")
         .arg (mapFileName));
  }
  QHash < QString, QStack < MapThread * > * >::iterator it = maps->find (creatorId);
  QStack < MapThread * > *stack;
  if (it == maps->end ()) {
    stack = new QStack < MapThread * > ();
    maps->insert (creatorId, stack);
  } else {
    stack = *it;
  }
  stack->push (map);
  return map;
}

MapThread *MapInstantiator::retrieveMap (
    edu::sharif::partov::nse::usermanagement::User user, QString mapFileName,
    QString creatorId, QHostAddress host) throw (MapNotFoundException *,
    AuthorizationException *) {
  if (!isUserAuthorizedForResumption (user, creatorId)) {
    throw new AuthorizationException
        (QString ("The ``%1'' user is not authorized to resume to simulation"
                  " of ``%2'' map."
                  "This map's simulation only can be resumed by limited users.")
         .arg (creatorId).arg (mapFileName));
  }
  if (!isHostAuthorizedToBeConnected (host)) {
    throw new AuthorizationException
        (QString ("The ``%1'' host is not authorized to resume to simulation"
                  " of ``%2'' map."
                  "This map's simulation only can be resumed from limited IP addresses.")
         .arg (host.toString ()).arg (mapFileName));
  }
  QHash < QString, QStack < MapThread * > * >::iterator it = maps->find (creatorId);
  if (it == maps->end () || (*it)->isEmpty ()) {
    throw new MapNotFoundException
        (QString ("The ``%1'' user did not create any instance of ``%2'' map.")
         .arg (creatorId).arg (mapFileName));
  }
  return (*it)->top ();
}

}
}
}
}
}
