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

#ifndef MAP_THREAD_H_
#define MAP_THREAD_H_

#include "MapExceptions.h"
#include "MapFileFormatException.h"

#include <QThread>

class QMutex;
class QWaitCondition;

class QDomDocument;
class QDomElement;
class QDomNode;

template < class Key, class T >
class QMap;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace plugin {

class PluginScheduler;
}
namespace map {

class Node;
class Map;

/**
 * Name:              MapThread
 * Parent:            QThread
 * Description:       Indicate the thread of a map.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Thread;
 */
class MapThread : public QThread {

  Q_OBJECT

private:
  Map *map;
  const QString scalarLogFilePath;

  QMutex *lock;
  QWaitCondition *cond;

  bool initialized;
  bool running;

  bool finalizeMapBeforeDeletion;

public:
  MapThread (const QString &mapName, const QString &_logPathTemplate, int _index,
      QString _creatorId);
  virtual ~MapThread ();

  /**
   * This method must be called just before start of map thread
   * and surely when the map object lives in this thread.
   */
  void init ();

protected slots:
  void privilegedFinalize ();

public:
  bool waitForInitialization ();

  void finalize ();

public slots:
  void quit ();

signals:
  void aboutToFinish ();

protected:
  void run ();

public:
  Map *getMap () const;
};

}
}
}
}
}

#endif /* MAP_THREAD_H_ */
