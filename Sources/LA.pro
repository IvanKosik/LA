#-------------------------------------------------
#
# Project created by QtCreator 2012-10-24T10:41:34
#
#-------------------------------------------------

QT       += core gui sql

TARGET = LA
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    PatientDialogs.cpp \
    DB.cpp \
    TableView.cpp \
    VerificationDialog.cpp \
    SqlQueryModel.cpp \
    SqlTableModel.cpp \
    ItemDelegates.cpp \
    TableWidgets/TableWidget.cpp \
    TableWidgets/VerificationParameterTableWidget.cpp \
    SqlQuery.cpp \
    PrognosisDialog.cpp \
    ViewVerificationDialog.cpp \
    ViewPrognosisDialog.cpp \
    PreeditDialog.cpp

HEADERS  += MainWindow.h \
    PatientDialogs.h \
    DB.h \
    TableView.h \
    VerificationDialog.h \
    SqlQueryModel.h \
    SqlTableModel.h \
    ItemDelegates.h \
    TableWidgets/TableWidget.h \
    TableWidgets/VerificationParameterTableWidget.h \
    SqlQuery.h \
    PrognosisDialog.h \
    ViewVerificationDialog.h \
    ViewPrognosisDialog.h \
    PreeditDialog.h

FORMS    += MainWindow.ui \
    PatientCard.ui \
    DiagnosisEditor.ui \
    Verification.ui \
    Prognosis.ui \
    ViewVerificationDialog.ui \
    ViewPrognosisDialog.ui \
    PreeditDialog.ui

DEPENDPATH +=.
INCLUDEPATH +=.

TRANSLATIONS += Translations/LA_ru.ts \
    Translations/designer_ru.ts \
    Translations/qtbase_ru.qm

RESOURCES += LA.qrc

win32:RC_FILE += LA.rc
