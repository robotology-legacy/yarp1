; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=COptionsDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "graspcapture.h"
LastPage=0

ClassCount=8
Class1=CGraspCaptureApp
Class2=CAboutDlg
Class3=CGraspCaptureDlg
Class4=CLiveCameraDlg
Class5=CLiveGloveDlg
Class6=CLiveTrackerDlg
Class7=CMessagesDlg

ResourceCount=7
Resource1=IDD_TRACKER (English (U.S.))
Resource2=IDD_GRASPCAPTURE_DIALOG (English (U.S.))
Resource3=IDD_DATAGLOVE (English (U.S.))
Resource4=IDD_CAMERA (English (U.S.))
Resource5=IDD_MESSAGES (English (U.S.))
Resource6=IDD_ABOUTBOX (English (U.S.))
Class8=COptionsDlg
Resource7=IDD_OPTIONS

[CLS:CGraspCaptureApp]
Type=0
BaseClass=CWinApp
HeaderFile=GraspCapture.h
ImplementationFile=GraspCapture.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=GraspCaptureDlg.cpp
ImplementationFile=GraspCaptureDlg.cpp
LastObject=CAboutDlg

[CLS:CGraspCaptureDlg]
Type=0
BaseClass=CDialog
HeaderFile=GraspCaptureDlg.h
ImplementationFile=GraspCaptureDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CGraspCaptureDlg

[CLS:CLiveCameraDlg]
Type=0
BaseClass=CDialog
HeaderFile=LiveCameraDlg.h
ImplementationFile=LiveCameraDlg.cpp

[CLS:CLiveGloveDlg]
Type=0
BaseClass=CDialog
HeaderFile=LiveGloveDlg.h
ImplementationFile=LiveGloveDlg.cpp

[CLS:CLiveTrackerDlg]
Type=0
BaseClass=CDialog
HeaderFile=LiveTrackerDlg.h
ImplementationFile=LiveTrackerDlg.cpp

[CLS:CMessagesDlg]
Type=0
BaseClass=CDialog
HeaderFile=MessagesDlg.h
ImplementationFile=MessagesDlg.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_GRASPCAPTURE_DIALOG]
Type=1
Class=CGraspCaptureDlg

[DLG:IDD_CAMERA]
Type=1
Class=CLiveCameraDlg

[DLG:IDD_DATAGLOVE]
Type=1
Class=CLiveGloveDlg

[DLG:IDD_TRACKER]
Type=1
Class=CLiveTrackerDlg

[DLG:IDD_MESSAGES]
Type=1
Class=CMessagesDlg

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342177294

[DLG:IDD_TRACKER (English (U.S.))]
Type=1
Class=?
ControlCount=15
Control1=IDC_STATIC,static,1342181390
Control2=IDC_POS_X,edit,1350641794
Control3=IDC_STATIC,static,1342308352
Control4=IDC_POS_Y,edit,1350641794
Control5=IDC_STATIC,static,1342308352
Control6=IDC_POS_Z,edit,1350641794
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,button,1342177287
Control9=IDC_OR_AZIMUTH,edit,1350641794
Control10=IDC_STATIC,static,1342308352
Control11=IDC_OR_ELEVATION,edit,1350641794
Control12=IDC_STATIC,static,1342308352
Control13=IDC_OR_ROLL,edit,1350641794
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,button,1342177287

[DLG:IDD_CAMERA (English (U.S.))]
Type=1
Class=?
ControlCount=0

[DLG:IDD_MESSAGES (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_MESS_BOX,edit,1350633668

[DLG:IDD_DATAGLOVE (English (U.S.))]
Type=1
Class=?
ControlCount=57
Control1=IDC_STATIC,static,1342181390
Control2=IDC_FINGER_1,edit,1350641794
Control3=IDC_STATIC,static,1342308352
Control4=IDC_FINGER_2,edit,1350641794
Control5=IDC_STATIC,static,1342308352
Control6=IDC_FINGER_3,edit,1350641794
Control7=IDC_STATIC,static,1342308352
Control8=IDC_FINGER_4,edit,1350641794
Control9=IDC_STATIC,static,1342308352
Control10=IDC_FINGER_5,edit,1350641794
Control11=IDC_STATIC,static,1342308352
Control12=IDC_FINGER_6,edit,1350641794
Control13=IDC_STATIC,static,1342308352
Control14=IDC_FINGER_7,edit,1350641794
Control15=IDC_STATIC,static,1342308352
Control16=IDC_FINGER_8,edit,1350641794
Control17=IDC_STATIC,static,1342308352
Control18=IDC_FINGER_9,edit,1350641794
Control19=IDC_STATIC,static,1342308352
Control20=IDC_FINGER_10,edit,1350641794
Control21=IDC_STATIC,static,1342308352
Control22=IDC_FINGER_11,edit,1350641794
Control23=IDC_STATIC,static,1342308352
Control24=IDC_FINGER_12,edit,1350641794
Control25=IDC_STATIC,static,1342308352
Control26=IDC_FINGER_13,edit,1350641794
Control27=IDC_STATIC,static,1342308352
Control28=IDC_FINGER_14,edit,1350641794
Control29=IDC_STATIC,static,1342308352
Control30=IDC_FINGER_15,edit,1350641794
Control31=IDC_STATIC,static,1342308352
Control32=IDC_STATIC,button,1342177287
Control33=IDC_ABD_1,edit,1350641794
Control34=IDC_STATIC,static,1342308352
Control35=IDC_ABD_2,edit,1350641794
Control36=IDC_STATIC,static,1342308352
Control37=IDC_ABD_3,edit,1350641794
Control38=IDC_STATIC,static,1342308352
Control39=IDC_ABD_4,edit,1350641794
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,button,1342177287
Control42=IDC_PALM,edit,1350641794
Control43=IDC_STATIC,static,1342308352
Control44=IDC_WR_ABD,edit,1350641794
Control45=IDC_STATIC,static,1342308352
Control46=IDC_WR_FLX,edit,1350641794
Control47=IDC_STATIC,static,1342308352
Control48=IDC_STATIC,button,1342177287
Control49=IDC_PRES_1,edit,1350641794
Control50=IDC_STATIC,static,1342308352
Control51=IDC_PRES_2,edit,1350641794
Control52=IDC_STATIC,static,1342308352
Control53=IDC_PRES_3,edit,1350641794
Control54=IDC_STATIC,static,1342308352
Control55=IDC_PRES_4,edit,1350641794
Control56=IDC_STATIC,static,1342308352
Control57=IDC_STATIC,button,1342177287

[DLG:IDD_GRASPCAPTURE_DIALOG (English (U.S.))]
Type=1
Class=CGraspCaptureDlg
ControlCount=15
Control1=IDC_CONNECT,button,1342242816
Control2=IDC_DISCONNECT,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_ACQ_START,button,1342242816
Control5=IDC_ACQ_STOP,button,1342242816
Control6=IDC_STATIC,button,1342177287
Control7=IDC_LIVE_CAMERA,button,1342242816
Control8=IDC_LIVE_GLOVE,button,1342242816
Control9=IDC_LIVE_TRACKER,button,1342242816
Control10=IDC_STATIC,button,1342177287
Control11=IDC_OPTIONS,button,1342242816
Control12=IDC_CALIBRATION,button,1342242816
Control13=IDC_DEBUG_WND,button,1342242816
Control14=IDC_STATIC,button,1342177287
Control15=IDC_KILL,button,1342242816

[DLG:IDD_OPTIONS]
Type=1
Class=COptionsDlg
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LOAD,button,1342242816
Control4=IDC_SAVE,button,1342242816
Control5=IDC_DEFAULT,button,1342242816
Control6=IDC_SETPATH,button,1342242816
Control7=IDC_STATIC,static,1342308864
Control8=IDC_PORT_N,edit,1350631552
Control9=IDC_STATIC,static,1342308864
Control10=IDC_NET_N,edit,1350631552
Control11=IDC_STATIC,static,1342308864
Control12=IDC_REFRESH,edit,1350639746
Control13=IDC_STATIC,static,1342308864
Control14=IDC_STATIC,static,1342308864
Control15=IDC_PREFIX,edit,1350631552
Control16=IDC_STATIC,static,1342308864
Control17=IDC_SAVEPATH,edit,1350631552

[CLS:COptionsDlg]
Type=0
HeaderFile=OptionsDlg.h
ImplementationFile=OptionsDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=COptionsDlg
VirtualFilter=dWC

