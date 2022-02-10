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

  closeButton = getChild<QPushButton>(rootWidget, "closeButton");
  connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

  message = getChild<QPlainTextEdit>(rootWidget, "message");

  sendButton = getChild<QPushButton>(rootWidget, "sendButton");
  connect(sendButton, SIGNAL(clicked()), SLOT(OnSend()));
}

     QVwMsg::~QVwMsg() {}
void QVwMsg::OnSend() { emit EmMsgSend(message->toPlainText());  }

