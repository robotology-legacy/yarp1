; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=COptionsDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "cameraTune.h"

ClassCount=7
Class1=CCameraTuneApp
Class2=CCameraTuneDlg
Class3=CAboutDlg

ResourceCount=9
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_CAMERATUNE_DIALOG
Resource4=IDD_MESSAGE
Resource5=IDD_ABOUTBOX (English (U.S.))
Class4=CLiveCameraDlg
Resource6=IDD_EDGES
Class5=CLiveEdgesDlg
Resource7=IDD_CAMERA
Class6=CMessageDlg
Resource8=IDD_CAMERATUNE_DIALOG (English (U.S.))
Class7=COptionsDlg
Resource9=IDD_OPTIONS

[CLS:CCameraTuneApp]
Type=0
HeaderFile=cameraTune.h
ImplementationFile=cameraTune.cpp
Filter=N

[CLS:CCameraTuneDlg]
Type=0
HeaderFile=cameraTuneDlg.h
ImplementationFile=cameraTuneDlg.cpp
Filter=D
LastObject=CCameraTuneDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=cameraTuneDlg.h
ImplementationFile=cameraTuneDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_CAMERATUNE_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CCameraTuneDlg

[DLG:IDD_CAMERATUNE_DIALOG (English (U.S.))]
Type=1
Class=CCameraTuneDlg
ControlCount=7
Control1=IDC_SHOW_CAMERA,button,1342242816
Control2=IDC_SHOW_EDGES,button,1342242816
Control3=IDC_START_CAMERA,button,1342242816
Control4=IDC_CALIBRATE,button,1342242816
Control5=IDC_OPTIONS,button,1342242816
Control6=IDC_PROGRESS1,msctls_progress32,1350565889
Control7=IDC_PROG_DESC,static,1342308864

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_CAMERA]
Type=1
Class=CLiveCameraDlg
ControlCount=0

[CLS:CLiveCameraDlg]
Type=0
HeaderFile=LiveCameraDlg.h
ImplementationFile=LiveCameraDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CLiveCameraDlg
VirtualFilter=dWC

[CLS:CLiveEdgesDlg]
Type=0
HeaderFile=LiveEdgesDlg.h
ImplementationFile=LiveEdgesDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CLiveEdgesDlg
VirtualFilter=dWC

[DLG:IDD_EDGES]
Type=1
Class=CLiveEdgesDlg
ControlCount=0

[DLG:IDD_MESSAGE]
Type=1
Class=CMessageDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_OUTPUT,edit,1352730820
Control3=IDC_SAVE,button,1342242816

[CLS:CMessageDlg]
Type=0
HeaderFile=MessageDlg.h
ImplementationFile=MessageDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CMessageDlg
VirtualFilter=dWC

[CLS:COptionsDlg]
Type=0
HeaderFile=OptionsDlg.h
ImplementationFile=OptionsDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COptionsDlg

[DLG:IDD_OPTIONS]
Type=1
Class=COptionsDlg
ControlCount=26
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_AUTOSAVE,button,1342245891
Control4=IDC_FILE_NAME,edit,1350631552
Control5=IDC_SET_FILE_NAME,button,1342242816
Control6=IDC_STATIC,static,1342308864
Control7=IDC_BOARD_N,edit,1350639744
Control8=IDC_STATIC,static,1342308864
Control9=IDC_SIZE_X,edit,1350639744
Control10=IDC_STATIC,static,1342308864
Control11=IDC_SIZE_Y,edit,1350639744
Control12=IDC_STATIC,button,1342177287
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308864
Control15=IDC_RHO,edit,1350639744
Control16=IDC_STATIC,static,1342308864
Control17=IDC_THETA,edit,1350639744
Control18=IDC_STATIC,button,1342177287
Control19=IDC_STATIC,static,1342308864
Control20=IDC_REFRESH,edit,1350639744
Control21=IDC_STATIC,static,1342308864
Control22=N_IMAGES,edit,1350639744
Control23=IDC_STATIC,button,1342177287
Control24=IDC_LOAD,button,1342242816
Control25=IDC_SAVE,button,1342242816
Control26=IDC_DEFAULT,button,1342242816

