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

#include "QVwAbout.hpp"
#include "util/util.hpp"

QVwAbout::QVwAbout(QObject *parent) : QVwDlg(":/dmmsnoop/QVwAbout.ui", parent) {
    closeButton = getChild<QPushButton>(dialog, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    majorVersion = 0;
    minorVersion = 0;
    revision = 0;

    theTrMsg = TrMsg::GetInstance(TrMsg::DEL_ENGLISH);

    QLbAppVer = getChild<QLabel>(dialog, "QLbAppVer");
    updateVersion();

    QLbAppUrl = getChild<QLabel>(dialog, "QLbAppUrl");
    QLbAppUrl->setText(theTrMsg->MsgAppGet(TrMsg::DAT_APP_URL_CODE));
}

QVwAbout::~QVwAbout() {}

void QVwAbout::setMajorVersion(int majorVersion) {
    this->majorVersion = majorVersion;
    updateVersion();
}

void QVwAbout::setMinorVersion(int minorVersion) {
    this->minorVersion = minorVersion;
    updateVersion();
}

void QVwAbout::setRevision(int revision) {
    this->revision = revision;
    updateVersion();
}

void QVwAbout::updateVersion() {
    QLocale locale = QLocale::system();
    QLbAppVer->setText(tr("%1.%2.%3", "versionFormat").
                     arg(locale.toString(majorVersion)).
                     arg(locale.toString(minorVersion)).
                     arg(locale.toString(revision)));
}

