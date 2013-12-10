#ifndef VIEWPROGNOSISDIALOG_H
#define VIEWPROGNOSISDIALOG_H

#include <QDialog>

#include "DB.h"

#include <QSqlQueryModel>
//------------------------------------------------------------------------------
namespace Ui {
    class ViewPrognosisDialog;
}
//------------------------------------------------------------------------------
class ViewPrognosisDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ViewPrognosisDialog(Id prognosisId, QWidget *parent = 0);
    ~ViewPrognosisDialog();
    
private:
    Ui::ViewPrognosisDialog *ui;
    Id mPrognosisId;
    QSqlQueryModel *mPhaseProbabilityModel;

private slots:
    void phaseChanged();
    void on_dialogButtonBox_accepted();
};
//------------------------------------------------------------------------------
#endif // VIEWPROGNOSISDIALOG_H
