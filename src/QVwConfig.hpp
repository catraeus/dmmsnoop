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

#ifndef __QVwCONFIG_HPP__
#define __QVwCONFIG_HPP__

#include "DrvIf.hpp"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>

#include "QVwDesgn.hpp"

class QVwConfig: public QVwDesgn {
  Q_OBJECT

public:
  explicit          QVwConfig(DrvIf *i_theDrvIf, QObject *parent=0);
                   ~QVwConfig();
           void    Build     (void);
public slots:
           void    OnMidiDrvAdd        (int  i_dex, const QString &i_name);
           void    OnPortInpAdd        (int  i_dex, const QString &i_name);
           void    OnPortOutAdd        (int  i_dex, const QString &i_name);
           void    OnPortInpDel        (int  i_dex);
           void    OnPortOutDel        (int  i_dex);
           void    OnMidiDrvChg        (int  i_dex);
           void    OnModeIgnActSnChg   (bool i_ign);
           void    OnModeIgnSysExChg   (bool i_ign);
           void    OnModeIgnMiTimChg   (bool i_ign);
           void    OnPortInpChg        (int  i_dex);
           void    OnPortOutChg        (int  i_dex);

signals:
           void    EmModeIgnActSnChg   (bool i_ign);
           void    EmModeIgnSysExChg   (bool i_ign);
           void    EmModeIgnMiTimChg   (bool i_ign);

private slots:
           void    DoMidiDrvChg        (int i_dex);
           void    DoPortInpChg        (int i_dex);
           void    DoPortOutChg        (int i_dex);

private:

           QPushButton *QPbDlgClose;
           QComboBox   *QCoMidiDrv;
           QComboBox   *QCoPortInp;
           QComboBox   *QCoPortOut;
           QCheckBox   *QChModeIgnActSn;
           QCheckBox   *QChModeIgnSysEx;
           QCheckBox   *QChModeIgnMiTim;

           DrvIf       *theDrvIf;
  int      drvCnt;
  int      drvNo;

  int      portInpNo;
  int      portOutNo;

};

#endif
