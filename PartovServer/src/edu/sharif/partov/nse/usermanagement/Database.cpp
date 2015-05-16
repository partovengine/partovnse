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

#include "Database.h"

#include "User.h"

#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace usermanagement {

const QString Database::PartovDBConnectionName ("PartovDBConnection");

Database *Database::me;

Database::Database (QString host, int port, QString dbname, QString user, QString pass) :
authenticationQuery (0), valid (false) {
  db = QSqlDatabase::addDatabase ("QMYSQL", PartovDBConnectionName);
  if (!db.isValid ()) {
    return;
  }
  db.setHostName (host);
  db.setPort (port);
  db.setDatabaseName (dbname);
  db.setUserName (user);
  db.setPassword (pass);
  if (!db.open ()) {
    return;
  }
  authenticationQuery = new QSqlQuery (db);
  if (!authenticationQuery->prepare ("SELECT userName, groupName FROM User"
                                     " WHERE userName = ? AND password = md5(?)"
                                     " AND active = 1")) {
    return;
  }
  valid = true;
}

Database::~Database () {
  if (authenticationQuery) {
    delete authenticationQuery;
  }
}

User Database::authenticateUser (QString userName, QString password) const {
  QString groupName;
  bool ok = false;
  authenticationQuery->bindValue (0, userName);
  authenticationQuery->bindValue (1, password);

  if (authenticationQuery->exec ()) {
    if (authenticationQuery->size () == 1 && authenticationQuery->next ()) {
      userName = authenticationQuery->value (0).toString ();
      groupName = authenticationQuery->value (1).toString ();
      ok = true;
    }
  }
  authenticationQuery->finish ();
  if (ok) {
    return User (userName, groupName);
  } else {
    return User ();
  }
}

bool Database::init () {
  QSettings config ("../config/config.ini", QSettings::IniFormat);
  config.beginGroup ("database");
  QString host = config.value ("host", "localhost").toString ();
  bool ok = true;
  int port = config.value ("port", 3306).toInt (&ok);
  if (!ok) {
    return false;
  }
  QString dbname = config.value ("dbname").toString ();
  QString user = config.value ("user").toString ();
  QString pass = config.value ("pass").toString ();
  config.endGroup ();

  Database::me = new Database (host, port, dbname, user, pass);
  if (!Database::me->valid) {
    Database::destroy ();
    return false;
  }
  return true;
}

Database *Database::getInstance () {
  return Database::me;
}

void Database::destroy () {
  delete Database::me;
}

}
}
}
}
}
