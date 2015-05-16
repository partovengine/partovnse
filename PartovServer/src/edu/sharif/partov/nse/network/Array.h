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

#ifndef ARRAY_H_
#define ARRAY_H_

#include "IOException.h"

#include <QDataStream>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              Array<T>
 * Parent:            None
 * Description:       Implement a generic array which may be shared. Set and Get values
 *                    need O(1) and the size of array is immutable.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Template Class;
 */
template < typename T >
class Array {

private:
  T *t;
  int size;

public:

  inline Array (int _size) :
      t (new T[_size]), size (_size) {
    for (int i = 0; i < size; ++i) {
      t[i] = 0;
    }
  }

  inline Array (T *_t, int _size) :
      t (_t), size (_size) {
  }

  virtual ~Array ();

  operator const T* () const;
  T &operator[] (int index);
  T &at (int index);

  int length () const;

  QDataStream &operator>> (QDataStream &stream) const
  throw (edu::sharif::partov::nse::network::IOException *);
  QDataStream &operator<< (QDataStream &stream)
  throw (edu::sharif::partov::nse::network::IOException *);

  virtual Array < T > *clone () const;
};

template < typename T >
Array < T >::~Array () {
  delete[] t;
}

template < typename T >
inline Array < T >::operator const T* () const {
  return t;
}

template < typename T >
inline T &Array < T >::operator[] (int index) {
  return t[index];
}

template < typename T >
inline T &Array < T >::at (int index) {
  return t[index];
}

template < typename T >
inline int Array < T >::length () const {
  return size;
}

template < typename T >
QDataStream &Array < T >::operator>> (QDataStream &stream) const
throw (edu::sharif::partov::nse::network::IOException *) {
  if (stream.writeRawData (reinterpret_cast<char *> (t), size * sizeof (T))
      != static_cast<int> (size * sizeof (T))) {
    // an error occurred
    QString mesg = "Can not write ";
    mesg += (size * sizeof (T)) + " byte(s) to stream";
    throw new edu::sharif::partov::nse::network::IOException (mesg);
  }
  return stream;
}

template < typename T >
QDataStream &Array < T >::operator<< (QDataStream &stream)
throw (edu::sharif::partov::nse::network::IOException *) {
  if (stream.readRawData (reinterpret_cast<char *> (t), size * sizeof (T))
      != static_cast<int> (size * sizeof (T))) {
    // an error occurred
    QString mesg = "Can not read ";
    mesg += (size * sizeof (T)) + " byte(s) from stream";
    throw new edu::sharif::partov::nse::network::IOException (mesg);
  }
  return stream;
}

template < typename T >
Array < T > *Array < T >::clone () const {
  T *newT = new T[size];
  for (int i = 0; i < size; ++i) {
    newT[i] = t[i];
  }
  return new Array < T > (newT, size);
}

}
}
}
}
}

#endif /* ARRAY_H_ */
