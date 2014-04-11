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

#ifndef SCALARLOGGER_H_
#define SCALARLOGGER_H_

#include <QObject>

template < class Key, class T >
class QMap;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

class MapThread;
}
namespace util {

/**
 * Name:              ScalarLogger
 * Parent:            None
 * Description:       This class allows to log scalar values from any object of a map. The
 *                    results can be fetched by all objects living in map's thread while the
 *                    map thread is running. It also will be saved in a file in the map's folder
 *                    when the map get deleted.
 *                    NOTE: Only objects living in the related map's thread are allowed to
 *                    set and/or get values. Otherwise race condition will arise.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.util
 * Tags:              SingletonPerMap
 */
class ScalarLogger : QObject {

private:
  const QString logFilePath;
  QMap < QString, int > *intValues;

public:
  ScalarLogger (const QString &logFilePath,
      edu::sharif::partov::nse::map::MapThread *parent = 0);
  virtual ~ScalarLogger ();

  void setIntegerValue (const QString &key, int value);
  int getIntegerValue (const QString &key) const;

private:
  void saveToLogFile () const;
};

}
}
}
}
}

#endif /* SCALARLOGGER_H_ */
