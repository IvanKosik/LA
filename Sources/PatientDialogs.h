#ifndef PatientDialogs_h
#define PatientDialogs_h

#include <QtGui/QDialog>

#include <QtSql/QSqlRelationalDelegate>
#include <QtGui/QDataWidgetMapper>

class QAbstractButton;

namespace Ui {
    class PatientCard;
}
//------------------------------------------------------------------------------
class PatientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PatientDialog(QSqlTableModel *patientModel,
                           QWidget *parent = 0);
    virtual ~PatientDialog();

protected:
    virtual void closeEvent(QCloseEvent *);

    bool apply();
    virtual void cancel();

    Ui::PatientCard *ui;

    QDataWidgetMapper *mMapper;

protected slots:
    void on_dialogButtonBox_accepted();
    void on_dialogButtonBox_rejected();
    virtual void on_dialogButtonBox_clicked(QAbstractButton *button);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class AddPatientDialog : public PatientDialog
{
    Q_OBJECT
public:
    explicit AddPatientDialog(QSqlTableModel *patientModel,
                              QWidget *parent = 0);
    ~AddPatientDialog();

private:
    void cancel();

private slots:
    void on_dialogButtonBox_clicked(QAbstractButton *button);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class EditPatientDialog : public PatientDialog
{
    Q_OBJECT
public:
    explicit EditPatientDialog(QSqlTableModel *patientModel,
                               qint32 rowIndex, QWidget *parent = 0);
    ~EditPatientDialog();

private:
    qint32 mRowIndex;

private slots:
    void on_dialogButtonBox_clicked(QAbstractButton *button);
};
//------------------------------------------------------------------------------
#endif // PatientDialogs_h
