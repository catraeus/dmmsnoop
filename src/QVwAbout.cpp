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

#include <stdio.h>

#include <QtCore/QLocale>

#include "AppVersion.hpp"
#include "BuildNo.hpp"

#include "QVwAbout.hpp"
#include "util/DmmStr.hpp"

QVwAbout::QVwAbout(QObject *parent) : QVwDlg(":/dmmsnoop/QVwAbout.ui", parent) {
    theTrMsg  = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);
    QPbClose  = getChild<QPushButton> (dialog, "QPbClose"   );

    QLbAppName = getChild<QLabel>     (dialog, "QLbAppName" );
    QLbAppVer  = getChild<QLabel>     (dialog, "QLbAppVer"  );
    QLbHome    = getChild<QLabel>     (dialog, "QLbHome"    );
    QLbContact = getChild<QLabel>     (dialog, "QLbContact" );
    QlbLic     = getChild<QLabel>     (dialog, "QlbLic" );

    connect(QPbClose, SIGNAL(clicked()), SLOT(hide()));

    QLbAppName->setText(APP_NAME);

    sprintf(verStr, "%d.%d.%d b%d", VER_MAJ, VER_MIN, VER_REV, BUILD_NO);
    QLbAppVer->setText(verStr);
    QLbHome->setText(theTrMsg->MsgAppGet(TrMsg::DAT_APP_URL_CODE));

}

QVwAbout::~QVwAbout() {}
