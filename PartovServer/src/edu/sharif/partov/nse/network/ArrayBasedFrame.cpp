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

#include "ArrayBasedFrame.h"

#include "Array.h"
#include "ReferenceCounter.h"

#include <QDebug>

namespace edu {
namespace sharif {
namespace partov {
namespace nse {
namespace network {

ArrayBasedFrame::ArrayBasedFrame (Array < quint8 > *_rawFrame,
    ReferenceCounter *_refCounter) :
FirstLayerFrame (_refCounter), rawFrame (_rawFrame) {
}

ArrayBasedFrame::ArrayBasedFrame (int _size, ReferenceCounter *_refCounter) :
FirstLayerFrame (_refCounter), rawFrame (new Array < quint8 > (_size)) {
}

ArrayBasedFrame::~ArrayBasedFrame () {
  delete rawFrame;
}

const quint8 *ArrayBasedFrame::getSerializedData () const {
  return static_cast<const quint8 *> (*rawFrame);
}

quint32 ArrayBasedFrame::getSerializedDataLength () const {
  return rawFrame->length ();
}

void ArrayBasedFrame::setFrameRawDataAsInt8 (int index, quint8 data) {
  rawFrame->at (index) = data;
}

void ArrayBasedFrame::setFrameRawDataAsInt16 (int index, quint16 data) {
  (*const_cast<quint16 *> (reinterpret_cast<const quint16 *> (getSerializedData ()
      + index))) = data;
}

void ArrayBasedFrame::setFrameRawDataAsInt32 (int index, quint32 data) {
  (*const_cast<quint32 *> (reinterpret_cast<const quint32 *> (getSerializedData ()
      + index))) = data;
}

void ArrayBasedFrame::setFrameRawDataAsInt64 (int index, quint64 data) {
  (*const_cast<quint64 *> (reinterpret_cast<const quint64 *> (getSerializedData ()
      + index))) = data;
}

void ArrayBasedFrame::setFrameRawDataAsStream (int index, const char *istream,
    const int length) {
  memcpy (const_cast<quint8 *> (getSerializedData () + index), istream, length);
}

quint8 ArrayBasedFrame::getFrameRawDataAsInt8 (int index) const {
  return rawFrame->at (index);
}

quint16 ArrayBasedFrame::getFrameRawDataAsInt16 (int index) const {
  return *reinterpret_cast<const quint16 *> (getSerializedData () + index);
}

quint32 ArrayBasedFrame::getFrameRawDataAsInt32 (int index) const {
  return *reinterpret_cast<const quint32 *> (getSerializedData () + index);
}

quint64 ArrayBasedFrame::getFrameRawDataAsInt64 (int index) const {
  return *reinterpret_cast<const quint64 *> (getSerializedData () + index);
}

void ArrayBasedFrame::getFrameRawDataAsStream (int index, char *ostream,
    const int length) const {
  memcpy (ostream, getSerializedData () + index, length);
}

void ArrayBasedFrame::writeFrameOnTheStream (QDataStream &stream) const
throw (IOException *) {
  stream << (*this);
}

ArrayBasedFrame *ArrayBasedFrame::clone () const throw (NonCloneableException *) {
  return new ArrayBasedFrame (rawFrame->clone (), new ReferenceCounter ());
}

int ArrayBasedFrame::getBodyLength () const {
  return rawFrame->length ();
}

#define TO_HEX(DEC_NUM) (((DEC_NUM) < 10) ? ((DEC_NUM) + '0') : ((DEC_NUM) - 10 + 'A'))

void ArrayBasedFrame::dumpFrame () const {
  QDebug log = qDebug ();
  log << "Dumping frame:";
  for (int i = 0; i < rawFrame->length (); ++i) {
    int b1 = (rawFrame->at (i) >> 4) & 0x0F;
    int b2 = (rawFrame->at (i)) & 0x0F;
    char s[3];
    s[0] = TO_HEX (b1);
    s[1] = TO_HEX (b2);
    s[2] = '\0';
    if (i % 16 == 0) {
      log << endl;
    }
    log << s;
  }
}

#undef TO_HEX

/**
 * This operator must be called only and only when enough
 * data is available in stream to fill the frame.
 * Otherwise or if any other error occurred, throw an IO exception.
 */
QDataStream &operator>> (QDataStream &stream, ArrayBasedFrame &frame)
throw (IOException *) {
  return ((*frame.rawFrame) << stream);
}

QDataStream &operator<< (QDataStream &stream, const ArrayBasedFrame &frame)
throw (IOException *) {
  return ((*frame.rawFrame) >> stream);
}

}
}
}
}
}
