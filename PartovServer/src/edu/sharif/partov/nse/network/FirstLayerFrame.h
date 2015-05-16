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

#ifndef FIRSTLAYERFRAME_H_
#define FIRSTLAYERFRAME_H_

#include "Frame.h"

#include "IOException.h"

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

/**
 * Name:              FirstLayerFrame
 * Parent:            Frame
 * Description:       Interface of first layer frames according to OSI network model.
 *
 * Package Access:    Public
 * Package:           edu.sharif.partov.nse.network
 * Tags:              Abstract;
 */
class FirstLayerFrame : public Frame {
public:
  FirstLayerFrame (ReferenceCounter *_refCounter);
  virtual ~FirstLayerFrame ();

  virtual const quint8 *getSerializedData () const = 0;
  virtual quint32 getSerializedDataLength () const = 0;

  virtual void dumpFrame () const = 0;

  virtual void setFrameRawDataAsInt8 (int index, quint8 data) = 0;
  virtual void setFrameRawDataAsInt16 (int index, quint16 data) = 0;
  virtual void setFrameRawDataAsInt32 (int index, quint32 data) = 0;
  virtual void setFrameRawDataAsInt64 (int index, quint64 data) = 0;
  virtual void setFrameRawDataAsStream (int index, const char *istream,
      const int length) = 0;

  virtual quint8 getFrameRawDataAsInt8 (int index) const = 0;
  virtual quint16 getFrameRawDataAsInt16 (int index) const = 0;
  virtual quint32 getFrameRawDataAsInt32 (int index) const = 0;
  virtual quint64 getFrameRawDataAsInt64 (int index) const = 0;
  virtual void getFrameRawDataAsStream (int index, char *ostream,
      const int length) const = 0;

  /**
   * Calculate IP header checksum and return it in network byte-order
   */
  virtual quint16 calcIPHeaderChecksum (const int offset, const int length) const;
  virtual quint16 calcPseudoHeaderBasedChecksum (int offset, const int length,
      const quint16 *pseudoPacket, const int size) const;

  virtual FirstLayerFrame *clone () const throw (NonCloneableException *) = 0;

  virtual void writeFrameOnTheStream (QDataStream &stream) const throw (IOException *) = 0;
};

}
}
}
}
}

#endif /* FIRSTLAYERFRAME_H_ */
