#ifndef VIEWVERIFICATIONDIALOG_H
#define VIEWVERIFICATIONDIALOG_H

#include <QDialog>

#include "DB.h"

#include <QSqlQueryModel>
//------------------------------------------------------------------------------
namespace Ui {
    class ViewVerificationDialog;
}
//------------------------------------------------------------------------------
class ViewVerificationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ViewVerificationDialog(Id verificationId, QWidget *parent = 0);
    ~ViewVerificationDialog();
    
private:
    /*!
     \brief Метод отображает значения параметров верификации и описания.
     \param isSelectedDiagnosis True, если выделен какой-либо диагноз,
            иначе false.
     \param selectedDiagnosisId Идентификатор выделенного диагноза.
     */
    void displayParameters(bool isSelectedDiagnosis = false,
                           Id selectedDiagnosisId = 0);

    Ui::ViewVerificationDialog *ui;
    Id mVerificationId;
    QSqlQueryModel *mDiagnosisProbabilityModel;

private slots:
    void diagnosisChanged();
    void on_dialogButtonBox_accepted();
};
//------------------------------------------------------------------------------
#endif // VIEWVERIFICATIONDIALOG_H
