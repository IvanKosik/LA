#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "SqlTableModel.h"
#include "DB.h"

#include <QMainWindow>

//------------------------------------------------------------------------------
bool hasFirstPairLowerDate(QPair<QDateTime, Id> firstPair, QPair<QDateTime, Id> secondPair);
//------------------------------------------------------------------------------
namespace ReadableExpertise {
    // Readable field names:
    extern const char *Verification;
    extern const char *Prognosis;
}
//------------------------------------------------------------------------------
namespace Ui {
    class MainWindow;
}
//------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    void createModels();
    void adjustTables();

    Ui::MainWindow *ui;

    PatientSqlTableModel *mPatientModel;
    IdList mExpertiseIds;

private slots:
    void on_addPatientPushButton_clicked();
    void on_editPatientPushButton_clicked();
    void on_deletePatientPushButton_clicked();

    void on_verificationPushButton_clicked();
    void on_prognosisPushButton_clicked();

    void on_viewExpertisePushButton_clicked();
    void on_deleteExpertisePushButton_clicked();

    void on_preeditPushButton_clicked();
    void on_printPushButton_clicked();

    void patientChanged();
    void on_expertiseTableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
};
//------------------------------------------------------------------------------
#endif // MAINWINDOW_H
