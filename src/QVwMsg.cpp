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

#include "QVwMsg.hpp"
#include "util/DmmStr.hpp"

     QVwMsg::QVwMsg(QObject *parent) : QVwDesgn(":/dmmsnoop/QVwMsg.ui", parent) {
  QWidget *rootWidget = getRootWidget();

  QPb_Close = getChild<QPushButton>(rootWidget, "QPb_Close");
  connect(QPb_Close, SIGNAL(clicked()), SIGNAL(closeRequest()));

  QPt_Messages = getChild<QPlainTextEdit>(rootWidget, "QPt_Messages");

  QPb_Send = getChild<QPushButton>(rootWidget, "QPb_Send");
  connect(QPb_Send, SIGNAL(clicked()), SLOT(OnSend()));
}

     QVwMsg::~QVwMsg() {}
void QVwMsg::OnSend() { emit EmMsgSend(QPt_Messages->toPlainText());  }

