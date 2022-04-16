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

#ifndef __QVW_MSG_HPP__
#define __QVW_MSG_HPP__

#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>

#include "QVwDesgn.hpp"

class QVwMsg: public QVwDesgn {
  Q_OBJECT

public:
  explicit        QVwMsg(QObject *parent=0);
                 ~QVwMsg();

signals:
           void   EmMsgSend(const QString &data);

private slots:
           void   OnSend();

private:
    QPushButton    *QPb_Close;
    QPlainTextEdit *QPt_Messages;
    QPushButton    *QPb_Send;

};

#endif
