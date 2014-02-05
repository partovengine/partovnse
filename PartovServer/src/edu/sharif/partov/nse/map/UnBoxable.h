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

#ifndef UN_BOXABLE_H_
#define UN_BOXABLE_H_

#include <Qt>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace map {

/**
 * Name:              UnBoxable
 * Parent:            None
 * Description:       A representation of a template type which is also unboxable to wrapped type
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Unboxable;
 */
template < typename Type >
class UnBoxable {

private:
  Type b;

public:

  UnBoxable () :
      b () {
  }

  UnBoxable (Type _b) :
      b (_b) {
  }

  UnBoxable &operator= (Type _b) {
    b = _b;
    return *this;
  }

  operator Type () {
    return b;
  }
};

/**
 * Name:              Percentage
 * Parent:            UnBoxable
 * Description:       An unboxable specifically for persentage
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Unboxable;
 */
class Percentage : public UnBoxable < qreal > {

public:

  Percentage () :
      UnBoxable < qreal > () {
  }

  Percentage (qreal _b) :
      UnBoxable < qreal > (_b) {
  }
};

/**
 * Name:              Time
 * Parent:            UnBoxable
 * Description:       An unboxable specifically for time
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Unboxable;
 */
class Time : public UnBoxable < quint64 > {

public:

  Time () :
      UnBoxable < quint64 > () {
  }

  Time (quint64 _b) :
      UnBoxable < quint64 > (_b) {
  }
};

/**
 * Name:              Bandwidth
 * Parent:            UnBoxable
 * Description:       An unboxable specifically for bandwidth
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Unboxable;
 */
class Bandwidth : public UnBoxable < quint64 > {

public:

  Bandwidth () :
      UnBoxable < quint64 > () {
  }

  Bandwidth (quint64 _b) :
      UnBoxable < quint64 > (_b) {
  }
};

/**
 * Name:              Size
 * Parent:            UnBoxable
 * Description:       An unboxable specifically for size
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.map
 * Tags:              Unboxable;
 */
class Size : public UnBoxable < int > {

public:

  Size () :
      UnBoxable < int > () {
  }

  Size (int _b) :
      UnBoxable < int > (_b) {
  }
};

}
}
}
}
}

#endif /* UN_BOXABLE_H_ */
