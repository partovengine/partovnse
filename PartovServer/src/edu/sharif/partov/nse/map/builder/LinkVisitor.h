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

#ifndef LINKVISITOR_H_
#define LINKVISITOR_H_

#include "NamedElementVisitor.h"

#include "edu/sharif/partov/nse/map/link/Link.h"

#include <QString>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {
namespace builder {

/**
 * Name:              LinkVisitor
 * Parent:            NamedElementVisitor
 * Description:       Visitor for link tags.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map.builder
 * Tags:              Visitor;
 */
class LinkVisitor : public NamedElementVisitor {
private:
  QString logPathTemplate;

public:
  LinkVisitor (Map *_map, QString logPathTemplate);
  virtual ~LinkVisitor ();

  virtual void processNamedElement (QString linkName, QDomElement link)
      throw (MapFileFormatException *);

private:
  edu::sharif::partov::nse::map::link::Link::Protocol extractLinkProtocol (
      const QDomElement &link) const throw (MapFileFormatException *);
  int connectLinkToOtherLinks (const QDomElement &link,
      edu::sharif::partov::nse::map::link::Link *lk) const
          throw (MapFileFormatException *);
  void parseLinkLoggingParameter (QString linkName, const QDomElement &link,
      edu::sharif::partov::nse::map::link::Link *lk) const
          throw (MapFileFormatException *);
  int extractLossRate (const QDomElement &link,
      edu::sharif::partov::nse::map::link::Link *lk) const
          throw (MapFileFormatException *);
  int extractLatency (const QDomElement &link,
      edu::sharif::partov::nse::map::link::Link *lk) const
          throw (MapFileFormatException *);
  int extractBandwidth (const QDomElement &link,
      edu::sharif::partov::nse::map::link::Link *lk) const
          throw (MapFileFormatException *);
  bool parseRateInBps (QString rateStr, quint64 &rate) const;
};

}
}
}
}
}
}

#endif /* LINKVISITOR_H_ */
