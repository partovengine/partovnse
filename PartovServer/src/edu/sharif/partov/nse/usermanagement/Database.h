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

#ifndef DATABASE_H_
#define DATABASE_H_

#include <QObject>
#include <QSqlDatabase>

class QMutex;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace usermanagement {

class User;

/**
 * Name:              Database
 * Parent:            QObject
 * Description:       The abstraction of the database. This class mediate all interactions with
 *                    the database and OO world.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.usermanagement
 * Tags:              Singleton;
 */
class Database : public QObject {
Q_OBJECT

private:
  static const QString PartovDBConnectionName;

  static Database *me;

  QSqlDatabase db;
  QSqlQuery *authenticationQuery;
  bool valid;

  Database (QString host, int port, QString dbname, QString user, QString pass);
  virtual ~Database ();

public:
  /**
   * This slot must be run ONLY AND ONLY in the main thread.
   */
  User authenticateUser (QString userName, QString password) const;

  static bool init ();
  static Database *getInstance ();
  static void destroy ();
};

}
}
}
}
}

#endif /* DATABASE_H_ */
