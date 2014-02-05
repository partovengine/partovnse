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

#include "ScalarLogger.h"

#include "edu/sharif/partov/nse/map/MapThread.h"

#include <QMap>
#include <QFile>
#include <QTextStream>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace util {

ScalarLogger::ScalarLogger (const QString &_logFilePath,
    edu::sharif::partov::nse::map::MapThread *parent) :
QObject (parent), logFilePath (_logFilePath), intValues (new QMap < QString, int > ()) {
}

ScalarLogger::~ScalarLogger () {
  if (!intValues->isEmpty ()) {
    saveToLogFile ();
  }
  delete intValues;
}

void ScalarLogger::setIntegerValue (const QString &key, int value) {
  intValues->insert (key, value);
}

int ScalarLogger::getIntegerValue (const QString &key) const {
  return intValues->value (key);
}

void ScalarLogger::saveToLogFile () const {
  QFile logFile (logFilePath);
  if (!logFile.open (QFile::WriteOnly | QFile::Truncate)) {
    qCritical ("ScalarLogger: Error in opening file ``%s'' for writing.",
               qPrintable (logFilePath));
    return;
  }
  QTextStream out (&logFile);
  out << "[Integers]" << endl;
  for (QMap < QString, int >::iterator it = intValues->begin (); it != intValues->end ();
      ++it) {
    out << it.key () << "=" << it.value () << endl;
  }
  logFile.close ();
}

}
}
}
}
}
