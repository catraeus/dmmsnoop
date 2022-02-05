/*
 * dmmsnoop - MIDI monitor and prober
 * Copyright (C) 2012 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

#ifndef __DMM_STR_HPP_
#define __DMM_STR_HPP_

#include "DmmTypes.hpp"
#include <QtWidgets/QWidget>


  template<typename T> inline T *getChild(const QObject *object, const QString &name=QString()) {
    T *child = object->findChild<T *>(name);
    return child;
  }

  QString  getMIDIControlString(quint8 control);
  QString  getMIDINoteString   (quint8 note);
  QWidget *loadForm            (const QString &path, QWidget *parent=0);

        void    TimeUsToStrSec (quint64 i_llTim, char *o_tStr);
        void    ByteToString   (uint   i_byte,  char *o_str); // Yes, IMPORTANT, the input is an int with an LSB Byte in it.
        ullong  GetTS();
#endif
