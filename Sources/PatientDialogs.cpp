#include "PatientDialogs.h"
#include "ui_PatientCard.h"

#include "SqlTableModel.h"

#include <QAbstractButton>

#include <QMessageBox> //-
#include <QDebug> //-
#include <QSqlError> //-

//------------------------------------------------------------------------------
PatientDialog::PatientDialog(QSqlTableModel *patientModel,
                             QWidget *parent)
    : QDialog(parent), ui(new Ui::PatientCard)
{
    ui->setupUi(this);

    setModal(true);

    setResult(QDialog::Rejected);

    mMapper = new QDataWidgetMapper(this);
    mMapper->setModel(patientModel);
    mMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mMapper->setItemDelegate(new QSqlRelationalDelegate(mMapper)); //-?

    mMapper->addMapping(ui->lastNameLineEdit, patientModel->fieldIndex(Patient::LastName));
    mMapper->addMapping(ui->firstNameLineEdit, patientModel->fieldIndex(Patient::FirstName));
    mMapper->addMapping(ui->middleNameLineEdit, patientModel->fieldIndex(Patient::MiddleName));
    mMapper->addMapping(ui->birthDateEdit, patientModel->fieldIndex(Patient::BirthDate));
    mMapper->addMapping(ui->maleRadioButton, patientModel->fieldIndex(Patient::Sex));
    mMapper->addMapping(ui->addressesLineEdit, patientModel->fieldIndex(Patient::Addresses));
    mMapper->addMapping(ui->phonesLineEdit, patientModel->fieldIndex(Patient::Phones));
    mMapper->addMapping(ui->jobLineEdit, patientModel->fieldIndex(Patient::Job));
    mMapper->addMapping(ui->postLineEdit, patientModel->fieldIndex(Patient::Post));
    mMapper->addMapping(ui->passportSeriesLineEdit, patientModel->fieldIndex(Patient::PassportSeries));
    mMapper->addMapping(ui->passportNumberLineEdit, patientModel->fieldIndex(Patient::PassportNumber));
    mMapper->addMapping(ui->passportIssueDateEdit, patientModel->fieldIndex(Patient::PassportIssueDate));
    mMapper->addMapping(ui->passportIssuingAuthorityLineEdit, patientModel->fieldIndex(Patient::PassportIssuingAuthority));
    mMapper->addMapping(ui->passportPersonalNumberLineEdit, patientModel->fieldIndex(Patient::PassportPersonalNumber));
    mMapper->addMapping(ui->additionalInformationTextEdit, patientModel->fieldIndex(Patient::AdditionalInformation));
}
//------------------------------------------------------------------------------
PatientDialog::~PatientDialog()
{
    delete ui;
}
//------------------------------------------------------------------------------
void PatientDialog::closeEvent(QCloseEvent *)
{
    cancel();
}
//------------------------------------------------------------------------------
bool PatientDialog::apply()
{
    SqlTableModel *model = (SqlTableModel *)(mMapper->model());

    // model->submitAll() необходимо, если setEditStrategy(QSqlTableModel::OnManualSubmit):
    if (!(mMapper->submit() && model->submitAll())) {
        QSqlError lastError = model->lastError();

        QMessageBox::warning(this, tr("Error"), tr("Cannot set the values in the model!")
                + tr("\nError code: ") + QString::number(lastError.number())
                + tr("\nError text: ") + lastError.text());

        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
void PatientDialog::cancel()
{
}
//------------------------------------------------------------------------------
void PatientDialog::on_dialogButtonBox_accepted()
{
    if (apply()) {
        done(QDialog::Accepted);
    }
}
//------------------------------------------------------------------------------
void PatientDialog::on_dialogButtonBox_rejected()
{
    cancel();
    done(QDialog::Rejected); //- or done(result()); тогда будет учитывать, нажимали ли до этого Применить.
}
//------------------------------------------------------------------------------
void PatientDialog::on_dialogButtonBox_clicked(QAbstractButton *button)
{
    if (ui->dialogButtonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        // Если данные успешно добавлены:
        if (apply()) {
            setResult(QDialog::Accepted);
        }
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AddPatientDialog::AddPatientDialog(QSqlTableModel *patientModel,
                                   QWidget *parent) :
    PatientDialog(patientModel, parent)
{
    qint32 rowCount = patientModel->rowCount();
    patientModel->insertRow(rowCount);
    mMapper->toLast();


    // Чтобы номер карточки (он равен id пациента) не был занят другим, пока
    // заполняется карточка, нужно сразу зарезервировать его. Для этого сразу
    // создаётся запись пациента с id.
    mMapper->submit();
    patientModel->submitAll(); //- Не менять местами со следующей строкой!
    mMapper->toLast(); // Необходимо, т.к. после submit() currentIndex устанавливается равным -1.


    QModelIndex modelIndex = patientModel->index(rowCount,
                                                 patientModel->fieldIndex(Patient::Id));
    QString patientId = patientModel->data(modelIndex).toString();
    QLabel *label = ui->cardNumberLabel;
    label->setText(label->text() + patientId);
}
//------------------------------------------------------------------------------
AddPatientDialog::~AddPatientDialog()
{
}
//------------------------------------------------------------------------------
void AddPatientDialog::cancel()
{
    if (result() == QDialog::Rejected) {
        // Удаляем последний ряд, который добавили, но не заполнили данными:
        QAbstractItemModel *model = mMapper->model();
        model->removeRow(model->rowCount() - 1);
        ((SqlTableModel *)mMapper->model())->submitAll(); //-вот добавил это. Теперь верно работает. Подумать, правильно ли я сделал, добавив это.
    }
}
//------------------------------------------------------------------------------
void AddPatientDialog::on_dialogButtonBox_clicked(QAbstractButton *button)
{
    PatientDialog::on_dialogButtonBox_clicked(button);


    if (ui->dialogButtonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        mMapper->toLast(); // Необходимо, т.к. после submit() currentIndex устанавливается равным -1.
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
EditPatientDialog::EditPatientDialog(QSqlTableModel *patientModel,
                                     qint32 rowIndex, QWidget *parent) :
    PatientDialog(patientModel, parent)
{
    mRowIndex = rowIndex;

    // Устанавливаем сразу положение в female, т.к. если это окажется male, то
    // mMapper его изменит.
    ui->femaleRadioButton->setChecked(true);

    mMapper->setCurrentIndex(rowIndex);


    QModelIndex modelIndex = patientModel->index(rowIndex,
                                                 patientModel->fieldIndex(Patient::Id));
    QString patientId = patientModel->data(modelIndex).toString();
    QLabel *label = ui->cardNumberLabel;
    label->setText(label->text() + patientId);
}
//------------------------------------------------------------------------------
EditPatientDialog::~EditPatientDialog()
{
}
//------------------------------------------------------------------------------
void EditPatientDialog::on_dialogButtonBox_clicked(QAbstractButton *button)
{
    PatientDialog::on_dialogButtonBox_clicked(button);


    if (ui->dialogButtonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        mMapper->setCurrentIndex(mRowIndex); // Необходимо, т.к. после submit() currentIndex устанавливается равным -1.
    }
}
//------------------------------------------------------------------------------
