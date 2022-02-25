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
#include "util/DmmStr.hpp"

QVwConfig::QVwConfig(DrvIf *i_theDrvIf, QObject *parent) : QVwDesgn(":/dmmsnoop/QVwConfig.ui", parent), theDrvIf(i_theDrvIf) {
  Build();
  Connect();
  MSU_WinMainCheckTxEn = NULL;

}
     QVwConfig::~QVwConfig() {}
void QVwConfig::Build(void) {
  QWidget *rootWidget = getRootWidget();
  QCoMidiDrv      = getChild<QComboBox>  (rootWidget, "QCoMidiDrv"     );
  QCoPortInp      = getChild<QComboBox>  (rootWidget, "QCoPortInp"     );
  QCoPortOut      = getChild<QComboBox>  (rootWidget, "QCoPortOut"     );
  QPbDlgClose     = getChild<QPushButton>(rootWidget, "QPbDlgClose"    );
  QChModeIgnActSn = getChild<QCheckBox>  (rootWidget, "QChModeIgnActSn");
  QChModeIgnSysEx = getChild<QCheckBox>  (rootWidget, "QChModeIgnSysEx");
  QChModeIgnMiTim = getChild<QCheckBox>  (rootWidget, "QChModeIgnMiTim");
  return;
}
void QVwConfig::Connect (void) {
  connect(QPbDlgClose,     SIGNAL(clicked           (    )),                SLOT  (hide              (    )));
  connect(QCoMidiDrv,      SIGNAL(activated         (int )),                SLOT  (OnUiMidiDrvChg    (int )));
  connect(QCoPortInp,      SIGNAL(activated         (int )),                SLOT  (OnUiPortInpChg      (int )));
  connect(QCoPortOut,      SIGNAL(activated         (int )),                SLOT  (OnUiPortOutChg      (int )));
  connect(QChModeIgnActSn, SIGNAL(clicked           (bool)), theDrvIf,      SLOT  (OnModeIgnActSnChg (bool)));
  connect(QChModeIgnSysEx, SIGNAL(clicked           (bool)), theDrvIf,      SLOT  (OnModeIgnSysExChg (bool)));
  connect(QChModeIgnMiTim, SIGNAL(clicked           (bool)), theDrvIf,      SLOT  (OnModeIgnMiTimChg (bool)));


  connect(this,            SIGNAL(EmModeIgnActSnChg (bool)), theDrvIf,      SLOT(OnModeIgnActSnChg   (bool)));
  connect(this,            SIGNAL(EmModeIgnSysExChg (bool)), theDrvIf,      SLOT(OnModeIgnSysExChg   (bool)));
  connect(this,            SIGNAL(EmModeIgnMiTimChg (bool)), theDrvIf,      SLOT(OnModeIgnMiTimChg   (bool)));
  return;
}
void QVwConfig::Setup(void) {
  drvCnt = theDrvIf->DrvCntGet();
  if(drvCnt == 0)      {  fprintf(stderr, "no MIDI drivers found"); fflush(stderr);}
  for(int i = 0; i < drvCnt; i++)
    OnMidiDrvAdd(i, QString::fromStdString(theDrvIf->DrvNameGet(i)));
  drvNo = theDrvIf->DrvNumGet();
  OnMidiDrvChg(drvNo);

  portInpNo  = theDrvIf->PortInpNoGet();
  OnPortInpChg(portInpNo);

  portOutNo = theDrvIf->PortOutNoGet();
  OnPortOutChg(portOutNo);

  theDrvIf->OnModeIgnActSnChg   (theDrvIf->ModeIgnActSnGet()); QChModeIgnActSn->setChecked (theDrvIf->ModeIgnActSnGet());
  theDrvIf->OnModeIgnSysExChg   (theDrvIf->ModeIgnSysExGet()); QChModeIgnSysEx->setChecked (theDrvIf->ModeIgnSysExGet());
  theDrvIf->OnModeIgnMiTimChg   (theDrvIf->ModeIgnMiTimGet()); QChModeIgnMiTim->setChecked (theDrvIf->ModeIgnMiTimGet());
  return;
}

void QVwConfig::OnMidiDrvAdd      (int  i_dex, const QString &i_name) {  QCoMidiDrv->insertItem              (i_dex + 1, i_name); }
void QVwConfig::OnMidiDrvChg      (int  i_dex                       ) {  QCoMidiDrv->setCurrentIndex         (i_dex + 1      ); }
void QVwConfig::OnPortInpAdd      (int  i_dex, const QString &i_name) {  QCoPortInp->insertItem              (i_dex + 1, i_name); }
void QVwConfig::OnPortInpChg      (int  i_dex                       ) {  QCoPortInp->setCurrentIndex         (i_dex + 1      ); }
void QVwConfig::OnPortInpDel      (int  i_dex                       ) {  QCoPortInp->removeItem              (i_dex + 1      ); }
void QVwConfig::OnPortOutAdd      (int  i_dex, const QString &i_name) {  QCoPortOut->insertItem              (i_dex + 1, i_name); }
void QVwConfig::OnPortOutChg      (int  i_dex                       ) {  QCoPortOut->setCurrentIndex         (i_dex + 1      ); }
void QVwConfig::OnPortOutDel      (int  i_dex                       ) {  QCoPortOut->removeItem              (i_dex + 1      ); }

void QVwConfig::OnUiMidiDrvChg    (int  i_dex ) {
  theDrvIf->OnDrvChg     (i_dex - 1 );
  if(MSU_WinMainCheckTxEn != 0)    MSU_WinMainCheckTxEn->Execute(NULL);
  return;
}
void QVwConfig::OnUiPortInpChg      (int  i_dex ) {
  theDrvIf->OnPortInpChg (i_dex - 1 );
  if(MSU_WinMainCheckTxEn != 0)    MSU_WinMainCheckTxEn->Execute(NULL);
  return;
}
void QVwConfig::OnUiPortOutChg      (int  i_dex ) {
  theDrvIf->OnPortOutChg (i_dex - 1 );
  if(MSU_WinMainCheckTxEn != 0)    MSU_WinMainCheckTxEn->Execute(NULL);
  return;
}

