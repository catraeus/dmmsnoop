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

#include "util.h"
#include "QoVw.hpp"

QoVw::QoVw(QWidget *i_rootWidget, QObject *parent)
: QObject(parent) {
  connect(&closeEventFilter, SIGNAL(closeRequest()), SIGNAL(closeRequest()));
  rootWidget->installEventFilter(&closeEventFilter);
  this->rootWidget = i_rootWidget;
}

                   QoVw::~QoVw           ()       { delete rootWidget;   }
const QWidget     *QoVw::getRootWidget   () const { return rootWidget;   }
      QWidget     *QoVw::getRootWidget   ()       { return rootWidget;   }
      void         QoVw::hide            ()       { setVisible(false);   }
      bool         QoVw::isCloseEnabled  () const { return closeEventFilter.isCloseEnabled();  }
      bool         QoVw::isVisible       () const { return rootWidget->isVisible();            }
      void         QoVw::setCloseEnabled (bool enabled) {
  if (closeEventFilter.isCloseEnabled() != enabled) {
       closeEventFilter.setCloseEnabled(enabled);
        emit closeEnabledChanged(enabled);
    }
}
      void         QoVw::setVisible(bool visible) {
  if (rootWidget->isVisible() != visible) {
    rootWidget->setVisible(visible);
    emit visibilityChanged(visible);
  }
}
      void         QoVw::show            ()       { setVisible(true);   }

