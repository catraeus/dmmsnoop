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

#include "util/DmmStr.hpp"
#include "QVwBase.hpp"

               QVwBase::QVwBase                (QWidget *rootWidget, QObject *parent) : QObject(parent) {
  connect(&closeEventFilter, SIGNAL(closeRequest()),       SIGNAL(closeRequest()));
    rootWidget->installEventFilter(&closeEventFilter);
    this->rootWidget = rootWidget;
}
               QVwBase::~QVwBase               (          )       { delete rootWidget;          }
const QWidget *QVwBase::getRootWidget       (          ) const { return rootWidget;          }
      QWidget *QVwBase::getRootWidget       (          )       { return rootWidget;          }
      void     QVwBase::hide                (          )       {        setVisible(false);   }
      bool     QVwBase::isCloseEnabled      (          ) const { return closeEventFilter.isCloseEnabled();   }
      bool     QVwBase::isVisible           (          ) const { return rootWidget->isVisible();             }
      void     QVwBase::setCloseEnabled     (bool i_en )       {
  if(closeEventFilter.isCloseEnabled  () != i_en) {
    closeEventFilter.setCloseEnabled (i_en);
    emit closeEnabledChanged(i_en);
  }
}
      void     QVwBase::setVisible          (bool i_vis)       {
  if(rootWidget->isVisible() != i_vis) {
    rootWidget->setVisible(i_vis);
    emit visibilityChanged(i_vis);
  }
}
      void     QVwBase::show                (          )       {        setVisible(true);    }
