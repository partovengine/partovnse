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

#ifndef ARRAYBASEDFRAME_H_
#define ARRAYBASEDFRAME_H_

#include "FirstLayerFrame.h"

#include "IOException.h"

class QDataStream;

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

template < typename T >
class Array;

/**
 * Name:              ArrayBasedFrame
 * Parent:            FirstLayerFrame
 * Description:       Array based implementation of the network first layer frame. All of
 *                    the frame data is stored in an Array<quint8>.
 *
 * Package Access:    Private
 * Package:           edu.sharif.partov.nse.network
 * Tags:              None
 */
class ArrayBasedFrame : public FirstLayerFrame {
private:
  Array < quint8 > *rawFrame;

public:
  ArrayBasedFrame (Array < quint8 > *_rawFrame, ReferenceCounter *_refCounter);
  ArrayBasedFrame (int _size, ReferenceCounter *_refCounter);
  virtual ~ArrayBasedFrame ();

  const quint8 *getSerializedData () const;
  quint32 getSerializedDataLength () const;

  virtual void dumpFrame () const;

  virtual void setFrameRawDataAsInt8 (int index, quint8 data);
  virtual void setFrameRawDataAsInt16 (int index, quint16 data);
  virtual void setFrameRawDataAsInt32 (int index, quint32 data);
  virtual void setFrameRawDataAsInt64 (int index, quint64 data);
  virtual void setFrameRawDataAsStream (int index, const char *istream, const int length);

  virtual quint8 getFrameRawDataAsInt8 (int index) const;
  virtual quint16 getFrameRawDataAsInt16 (int index) const;
  virtual quint32 getFrameRawDataAsInt32 (int index) const;
  virtual quint64 getFrameRawDataAsInt64 (int index) const;
  virtual void getFrameRawDataAsStream (int index, char *ostream, const int length) const;

  virtual void writeFrameOnTheStream (QDataStream &stream) const throw (IOException *);

  virtual ArrayBasedFrame *clone () const throw (NonCloneableException *);

  virtual int getBodyLength () const;

protected:
  friend QDataStream &operator>> (QDataStream &stream, ArrayBasedFrame &frame)
      throw (IOException *);
  friend QDataStream &operator<< (QDataStream &stream, const ArrayBasedFrame &frame)
      throw (IOException *);
};

QDataStream &operator>> (QDataStream &stream, ArrayBasedFrame &frame)
    throw (IOException *);
QDataStream &operator<< (QDataStream &stream, const ArrayBasedFrame &frame)
    throw (IOException *);

}
}
}
}
}

#endif /* ARRAYBASEDFRAME_H_ */
