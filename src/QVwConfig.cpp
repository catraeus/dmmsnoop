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

#include "QVwConfig.hpp"
#include "util/util.hpp"

  QVwConfig::QVwConfig(QObject *parent) : QVwDesgn(":/dmmsnoop/QVwConfig.ui", parent) {
  QWidget *rootWidget = getRootWidget();

  QCoMidiDrv      = getChild<QComboBox>  (rootWidget, "QCoMidiDrv"     ); connect(QCoMidiDrv,      SIGNAL(activated(int )), SLOT(DoMidiDrvChg(int)));

  QCoPortInp      = getChild<QComboBox>  (rootWidget, "QCoPortInp"     ); connect(QCoPortInp,      SIGNAL(activated(int )), SLOT(DoPortInpChg(int)));
  QCoPortOut      = getChild<QComboBox>  (rootWidget, "QCoPortOut"     ); connect(QCoPortOut,      SIGNAL(activated(int )), SLOT(DoPortOutChg(int)));
  QPbDlgClose     = getChild<QPushButton>(rootWidget, "QPbDlgClose"    ); connect(QPbDlgClose,     SIGNAL(clicked  (    )), SIGNAL(closeRequest()));

  QChModeIgnActSn = getChild<QCheckBox>  (rootWidget, "QChModeIgnActSn"); connect(QChModeIgnActSn, SIGNAL(clicked  (bool)), SIGNAL(EmModeIgnActSnChg(bool)));
  QChModeIgnSysEx = getChild<QCheckBox>  (rootWidget, "QChModeIgnSysEx"); connect(QChModeIgnSysEx, SIGNAL(clicked  (bool)), SIGNAL(EmModeIgnSysExChg(bool)));
  QChModeIgnMiTim = getChild<QCheckBox>  (rootWidget, "QChModeIgnMiTim"); connect(QChModeIgnMiTim, SIGNAL(clicked  (bool)), SIGNAL(EmModeIgnMiTimChg(bool)));
}
     QVwConfig::~QVwConfig() {}

void QVwConfig::OnMidiDrvAdd      (int  i_dex, const QString &i_name) {  QCoMidiDrv->insertItem              (i_dex + 1, i_name); }
void QVwConfig::OnMidiDrvChg      (int  i_dex                       ) {  QCoMidiDrv->setCurrentIndex         (i_dex + 1      ); }
void QVwConfig::OnPortInpAdd      (int  i_dex, const QString &i_name) {  QCoPortInp->insertItem              (i_dex + 1, i_name); }
void QVwConfig::OnPortInpChg      (int  i_dex                       ) {  QCoPortInp->setCurrentIndex         (i_dex + 1      ); }
void QVwConfig::OnPortInpDel      (int  i_dex                       ) {  QCoPortInp->removeItem              (i_dex + 1      ); }
void QVwConfig::OnPortOutAdd      (int  i_dex, const QString &i_name) {  QCoPortOut->insertItem              (i_dex + 1, i_name); }
void QVwConfig::OnPortOutChg      (int  i_dex                       ) {  QCoPortOut->setCurrentIndex         (i_dex + 1      ); }
void QVwConfig::OnPortOutDel      (int  i_dex                       ) {  QCoPortOut->removeItem              (i_dex + 1      ); }

void QVwConfig::DoMidiDrvChg      (int  i_dex ) { emit EmMidiDrvChg (i_dex - 1 ); }
void QVwConfig::DoPortInpChg      (int  i_dex ) { emit EmPortInpChg (i_dex - 1 ); }
void QVwConfig::DoPortOutChg      (int  i_dex ) { emit EmPortOutChg (i_dex - 1 ); }
void QVwConfig::OnModeIgnActSnChg (bool i_ign ) {      QChModeIgnActSn->setChecked (i_ign); }
void QVwConfig::OnModeIgnSysExChg (bool i_ign ) {      QChModeIgnSysEx->setChecked (i_ign); }
void QVwConfig::OnModeIgnMiTimChg (bool i_ign ) {      QChModeIgnMiTim->setChecked (i_ign); }

