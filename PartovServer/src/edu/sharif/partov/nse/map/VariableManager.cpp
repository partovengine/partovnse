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

#include "VariableManager.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

VariableManager::VariableManager () :
    repetitionIndex (-1) {
}

VariableManager::~VariableManager () {
}

bool VariableManager::readUnnamedElement (const QDomElement &element,
    const edu::sharif::partov::nse::map::builder::MapReader *reader,
    const char *tagName[],
    edu::sharif::partov::nse::map::builder::ElementVisitor *visitor[], int visitorsCount)
        throw (MapFileFormatException *) {
  if (repetitionIndex < 0) { // look for repeat
    if (element.localName () == "repeat") {
      QString countStr = element.attribute ("count");
      if (countStr.isNull ()) {
        throw new MapFileFormatException (element, "Missing repeat's count parameter");
      }
      bool ok;
      int count = countStr.toInt (&ok);
      if (!ok || count < 1) {
        throw new MapFileFormatException (element, "Malformed repeat's count parameter");
      }
      for (repetitionIndex = 0; repetitionIndex < count; ++repetitionIndex) {
        reader->readUnnamedElements (element, tagName, visitor, visitorsCount);
      }
      repetitionIndex = -1;
      return true;
    }
  } else { // look for variable
  }
  return false;
}

bool VariableManager::readNamedElement (const QDomElement &element,
    const edu::sharif::partov::nse::map::builder::MapReader *reader,
    const char *tagName[],
    edu::sharif::partov::nse::map::builder::NamedElementVisitor *visitor[],
    int visitorsCount, bool raiseExceptionOnUnkownTags, int &unknownTagsCount)
        throw (MapFileFormatException *) {
  if (repetitionIndex < 0) { // look for repeat
    if (element.localName () == "repeat") {
      QString countStr = element.attribute ("count");
      if (countStr.isNull ()) {
        throw new MapFileFormatException (element, "Missing repeat's count parameter");
      }
      bool ok;
      int count = countStr.toInt (&ok);
      if (!ok || count < 1) {
        throw new MapFileFormatException (element, "Malformed repeat's count parameter");
      }
      for (repetitionIndex = 0; repetitionIndex < count; ++repetitionIndex) {
        unknownTagsCount += reader->readNamedElements (element, tagName, visitor,
            visitorsCount, raiseExceptionOnUnkownTags);
      }
      repetitionIndex = -1;
      return true;
    }
  } else { // look for variable
  }
  return false;
}

QString VariableManager::resolveName (QString name) const throw () {
  if (repetitionIndex < 0) {
    return name;
  } else {
    return QString ("%1-%2").arg (name).arg (repetitionIndex);
  }
}

}
}
}
}
}
