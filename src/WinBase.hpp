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

#ifndef __QVW_BASE_HPP__
#define __QVW_BASE_HPP__

#include <QtWidgets/QWidget>

#include "QVwClose.hpp"

class QVwBase: public QObject {
  Q_OBJECT

public:
           bool           isCloseEnabled() const;
           bool           isVisible() const;

public slots:
           void           hide();
  virtual  void           setCloseEnabled(bool enabled);
  virtual  void           setVisible(bool visible);
           void           show();

signals:
           void           closeEnabledChanged(bool enabled);
           void           closeRequest();
           void           visibilityChanged(bool visible);

protected:
  explicit                QVwBase(QWidget *rootWidget, QObject *parent=0);
  virtual                ~QVwBase();
  const    QWidget       *getRootWidget() const;
           QWidget       *getRootWidget();

private:
    QVwClose      closeEventFilter;
    QWidget              *rootWidget;

};

#endif
