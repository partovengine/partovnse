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

#ifndef MAP_H_
#define MAP_H_

#include "MapExceptions.h"
#include "MapFileFormatException.h"

#include <QObject>

class QMutex;
class QWaitCondition;

class QDomDocument;
class QDomElement;
class QDomNode;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

class PluginScheduler;
}
namespace map {

class Node;
class MapThread;
class ListManager;
class VariableManager;

/**
 * Name:              Map
 * Parent:            QObject
 * Description:       Indicate a runtime representation of instances of maps (topologies). This class
 *                    is responsible to setup the map and its nodes, links, and so on.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              None
 */
class Map : public QObject {

  Q_OBJECT
public:
  static const char *MAP_VERSION;

private:
  QMutex *lock;
  const QString logPathTemplate;

  int index;
  ListManager *listManager;
  VariableManager *variableManager;
  QString creatorId;

  edu::sharif::partov::nse::plugin::PluginScheduler *pluginScheduler;

public:
  Map (const QString &mapName, const QString &_logPathTemplate, int _index,
      QString _creatorId, QMutex *_lock);
  virtual ~Map ();

  Node *retrieveNodeByName (const QString &name) const throw (NoSuchMapNodeException *);

  void finalize (MapThread *mt);

  int getMapIndex () const;

  /**
   * This method instantiates and forms map's contents. It's expected
   * to be called while the map thread is running.
   *
   * @return true iff everything goes right and map thread can continue
   * running. Otherwise false will be returned and the map thread must
   * be terminated.
   */
  bool setup ();

signals:
  void nodeIPAddressChanged (QString nodeName, int interfaceIndex, quint32 ip);
  void nodeNetmaskChanged (QString nodeName, int interfaceIndex, quint32 netmask);

protected:
  bool initializeMap () throw (MapFileFormatException *);

  void readLists (const QDomElement &lists) throw (MapFileFormatException *);
  void readLinks (const QDomElement &links) throw (MapFileFormatException *);
  void readNodes (const QDomElement &nodes) throw (MapFileFormatException *);
  void readFSM (const QDomElement &fsmElement) throw (MapFileFormatException *);

private:
  bool openMap (QDomDocument &map) const;

public:
  ListManager *getListManager () const;
  VariableManager *getVariableManager () const;

  edu::sharif::partov::nse::plugin::PluginScheduler *getPluginScheduler () const;

  QMutex *getMapChangesNotificationMutex () const;
};

}
}
}
}
}

#endif /* MAP_H_ */
