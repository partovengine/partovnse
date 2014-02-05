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

#include "MapReader.h"

#include "edu/sharif/partov/nse/map/VariableManager.h"

#include "ElementVisitor.h"
#include "NamedElementVisitor.h"
#include "NamedElementTwoPassVisitor.h"
#include "ListVisitor.h"
#include "UnnamedFieldVisitor.h"
#include "VariableManager.h"

#include <QString>
#include <QStringList>
#include <QDomNode>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

MapReader::MapReader (edu::sharif::partov::nse::map::VariableManager *_variableManager) :
variableManager (_variableManager) {
}

MapReader::~MapReader () {
}

void MapReader::checkElements (const QDomElement &elements, const char *tagName) const
throw (MapFileFormatException *) {
  QDomNode elementNode = elements.firstChild ();
  while (!elementNode.isNull ()) {
    QDomElement element = elementNode.toElement ();
    if (!element.isNull ()) {
      if (element.localName () != tagName) {
        throw new MapFileFormatException (element, "Wrong tag name");
      }
    }
    elementNode = elementNode.nextSibling ();
  }
}

void MapReader::readUnnamedElements (const QDomElement &elements, const char *tagName,
    ElementVisitor *visitor) const throw (MapFileFormatException *) {
  const char *tagNames[] = {tagName};
  ElementVisitor * visitors[] = {visitor};
  readUnnamedElements (elements, tagNames, visitors, 1);
}

void MapReader::readUnnamedElements (const QDomElement &elements, const char *tagName[],
    ElementVisitor *visitor[], int visitorsCount) const throw (MapFileFormatException *) {
  QDomNode elementNode = elements.firstChild ();
  while (!elementNode.isNull ()) {
    QDomElement element = elementNode.toElement ();
    if (!element.isNull ()) {
      if (variableManager) {
        if (variableManager->readUnnamedElement (element, this, tagName, visitor,
                                                 visitorsCount)) {
          elementNode = elementNode.nextSibling ();
          continue;
        }
      }
      int i;
      for (i = 0; i < visitorsCount; ++i) {
        if (element.localName () == tagName[i]) {
          break;
        }
      }
      if (i == visitorsCount) {
        throw new MapFileFormatException (element, "Wrong tag name");
      }
      visitor[i]->processUnnamedElement (element);
    }
    elementNode = elementNode.nextSibling ();
  }
}

int MapReader::readNamedElements (const QDomElement &elements, const char *tagName,
    NamedElementVisitor *visitor, bool raiseExceptionOnUnkownTags) const
throw (MapFileFormatException *) {
  const char *tagNames[] = {tagName};
  NamedElementVisitor * visitors[] = {visitor};
  return readNamedElements (elements, tagNames, visitors, 1, raiseExceptionOnUnkownTags);
}

int MapReader::readNamedElements (const QDomElement &elements, const char *tagName[],
    NamedElementVisitor *visitor[], int visitorsCount,
    bool raiseExceptionOnUnkownTags) const throw (MapFileFormatException *) {
  int unknownTags = 0;
  QDomNode elementNode = elements.firstChild ();
  while (!elementNode.isNull ()) {
    QDomElement element = elementNode.toElement ();
    if (!element.isNull ()) {
      if (variableManager) {
        if (variableManager->readNamedElement
            (element, this, tagName, visitor, visitorsCount,
             raiseExceptionOnUnkownTags, unknownTags)) {
          elementNode = elementNode.nextSibling ();
          continue;
        }
      }
      int i;
      for (i = 0; i < visitorsCount; ++i) {
        if (element.localName () == tagName[i]) {
          break;
        }
      }
      if (i == visitorsCount) {
        if (raiseExceptionOnUnkownTags) {
          throw new MapFileFormatException (element, "Wrong tag name");
        } else {
          unknownTags++;
        }
      } else {
        QString elementName = element.attribute ("name");
        if (elementName.isNull ()) {
          throw new MapFileFormatException (element, "Missing name parameter");
        }
        if (variableManager) {
          elementName = variableManager->resolveName (elementName);
        }
        visitor[i]->processNamedElement (elementName, element);
      }
    }
    elementNode = elementNode.nextSibling ();
  }
  return unknownTags;
}

void MapReader::readNamedElements (const QDomElement &elements, const char *tagName,
    NamedElementTwoPassVisitor *visitor) const throw (MapFileFormatException *) {
  QDomNode elementNode = elements.firstChild ();
  while (!elementNode.isNull ()) {
    QDomElement element = elementNode.toElement ();
    if (!element.isNull ()) {
      if (element.localName () != tagName) {
        throw new MapFileFormatException (element, "Wrong tag name");
      }
      QString elementName = element.attribute ("name");
      if (elementName.isNull ()) {
        throw new MapFileFormatException (element, "Missing name parameter");
      }
      visitor->processNamedElementFirstPass (elementName, element);
    }
    elementNode = elementNode.nextSibling ();
  }
  elementNode = elements.firstChild ();
  while (!elementNode.isNull ()) {
    QDomElement element = elementNode.toElement ();
    if (!element.isNull ()) {
      QString elementName = element.attribute ("name");
      visitor->processNamedElementSecondPass (elementName, element);
    }
    elementNode = elementNode.nextSibling ();
  }
}

int MapReader::readOneNamedElement (const QDomElement &elements, const char *tagName,
    NamedElementVisitor *visitor, bool raiseExceptionOnUnkownTags) const
throw (MapFileFormatException *) {
  int unknownTags = 0;
  bool anyElementIsVisited = false;
  QDomNode elementNode = elements.firstChild ();
  while (!elementNode.isNull ()) {
    QDomElement element = elementNode.toElement ();
    if (!element.isNull ()) {
      if (element.localName () != tagName) {
        if (raiseExceptionOnUnkownTags) {
          throw new MapFileFormatException (element, "Wrong tag name");
        } else {
          unknownTags++;
        }
      } else {
        QString elementName = element.attribute ("name");
        if (elementName.isNull ()) {
          throw new MapFileFormatException (element, "Missing name parameter");
        }
        if (anyElementIsVisited) {
          throw new MapFileFormatException
              (element, "Extra element (just one element was expected)");
        }
        visitor->processNamedElement (elementName, element);
        anyElementIsVisited = true;
      }
    }
    elementNode = elementNode.nextSibling ();
  }
  return unknownTags;
}

int MapReader::readOneUnnamedElement (const QDomElement &elements, const char *tagName,
    ElementVisitor *visitor, bool raiseExceptionOnUnkownTags) const
throw (MapFileFormatException *) {
  const char *tagNames[] = {tagName};
  ElementVisitor * visitors[] = {visitor};
  return readOneUnnamedElement (elements, tagNames, visitors, 1,
                                raiseExceptionOnUnkownTags);
}

int MapReader::readOneUnnamedElement (const QDomElement &elements, const char *tagName[],
    ElementVisitor *visitor[], int visitorsCount, bool raiseExceptionOnUnkownTags) const
throw (MapFileFormatException *) {
  int unknownTags = 0;
  bool anyElementIsVisited = false;
  QDomNode elementNode = elements.firstChild ();
  while (!elementNode.isNull ()) {
    QDomElement element = elementNode.toElement ();
    if (!element.isNull ()) {
      int i;
      for (i = 0; i < visitorsCount; ++i) {
        if (element.localName () == tagName[i]) {
          break;
        }
      }
      if (i == visitorsCount) {
        if (raiseExceptionOnUnkownTags) {
          throw new MapFileFormatException (element, "Wrong tag name");
        } else {
          unknownTags++;
        }
      } else {
        if (anyElementIsVisited) {
          throw new MapFileFormatException
              (element, "Extra element (just one element was expected)");
        }
        visitor[i]->processUnnamedElement (element);
        anyElementIsVisited = true;
      }
    }
    elementNode = elementNode.nextSibling ();
  }
  return unknownTags;
}

}
}
}
}
}
}
