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

#include "QVwClose.hpp"

      QVwClose::QVwClose(QObject *parent): QObject(parent) { closeEnabled = true; }
      QVwClose::~QVwClose() {}

bool  QVwClose::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::Close) {
    event->ignore();
    if (closeEnabled) emit closeRequest();
    return true;
  }
  return QObject::eventFilter(obj, event); // Bubble it up the window stack
}
bool  QVwClose::isCloseEnabled() const {    return closeEnabled;   }
void  QVwClose::setCloseEnabled(bool enabled) {    closeEnabled = enabled;   }
