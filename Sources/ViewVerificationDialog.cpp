#include "ViewVerificationDialog.h"
#include "ui_ViewVerificationDialog.h"

#include "SqlQuery.h"

#include <QDebug> //-
//------------------------------------------------------------------------------
ViewVerificationDialog::ViewVerificationDialog(Id verificationId, QWidget *parent)
    : mVerificationId(verificationId), QDialog(parent), ui(new Ui::ViewVerificationDialog)
{
    ui->setupUi(this);


    // Настраиваем таблицы:
    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->parameterValueTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-
    //-ui->diagnosisProbabilityTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-


    // Отображаем вероятности диагнозов:
    mDiagnosisProbabilityModel = new QSqlQueryModel(this);

    ui->diagnosisProbabilityTableView->adjust(mDiagnosisProbabilityModel);

    SqlQuery presumptiveVerificationDiagnosisQuery;
    QString query("SELECT ID, Name, Probability FROM PresumptiveVerificationDiagnosis WHERE VerificationID=?");
    presumptiveVerificationDiagnosisQuery.prepare(query);
    presumptiveVerificationDiagnosisQuery.addBindValue(mVerificationId);
    // Если запрос выполнен удачно:
    if (presumptiveVerificationDiagnosisQuery.exec()) {
        mDiagnosisProbabilityModel->setQuery(presumptiveVerificationDiagnosisQuery);
    }

    mDiagnosisProbabilityModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    mDiagnosisProbabilityModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    mDiagnosisProbabilityModel->setHeaderData(2, Qt::Horizontal, tr("Probability, %"));

    // Скрываем колонку идентификаторов:
    ui->diagnosisProbabilityTableView->setColumnHidden(0, true);

    connect(ui->diagnosisProbabilityTableView, SIGNAL(currentChanged()),
            SLOT(diagnosisChanged()));


    // Отображаем неизвестные параметры и дополнительные исследования:
    // Выделяем первый диагноз, чтобы сразу показывались его неизвестные параметры:
    if (mDiagnosisProbabilityModel->rowCount() > 0) {
        ui->diagnosisProbabilityTableView->selectRow(0);
    } else {
        // Отображаем параметры и их значения. Колонка с описаниями будет пустой:
        displayParameters();
    }

    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->unknownParameterTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-
}
//------------------------------------------------------------------------------
ViewVerificationDialog::~ViewVerificationDialog()
{
    delete ui;
}
//------------------------------------------------------------------------------
void ViewVerificationDialog::displayParameters(bool isSelectedDiagnosis,
                                               Id selectedDiagnosisId)
{
    QSqlQueryModel *parameterValueModel = new QSqlQueryModel(this);
    // Формируем таблицу с параметрами и их значениями:
    QString verificationParameterQuery("((SELECT ID, Name, Value FROM VerificationParameter WHERE VerificationID=?) AS ParameterTable)");
    // Формируем таблицу с описаниями:
    QString descriptionQuery;
    // Если выделен какой-либо диагноз:
    if (isSelectedDiagnosis) {
        // Получаем таблицу, содержащую описания параметров выделенного диагноза:
        descriptionQuery = "((SELECT Text, VerificationParameterID FROM VerificationDescription WHERE PresumptiveVerificationDiagnosisID=?) AS DescriptionTable)";
    } else {
        // Получаем пустую таблицу:
        descriptionQuery = "((SELECT Text, VerificationParameterID FROM VerificationDescription WHERE 0=1) AS DescriptionTable)";
    }

    // Объединяем полученные таблицы:
    SqlQuery query;
    query.prepare("SELECT ParameterTable.Name, ParameterTable.Value, DescriptionTable.Text FROM "
                  + verificationParameterQuery + " LEFT OUTER JOIN "
                  + descriptionQuery + " ON ParameterTable.ID=DescriptionTable.VerificationParameterID");
    query.addBindValue(mVerificationId);
    if (isSelectedDiagnosis) {
        query.addBindValue(selectedDiagnosisId);
    }
    // Если запрос выполнен удачно:
    if (query.exec()) {
        parameterValueModel->setQuery(query);
    }

    parameterValueModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    parameterValueModel->setHeaderData(1, Qt::Horizontal, tr("Value"));
    parameterValueModel->setHeaderData(2, Qt::Horizontal, tr("Description"));

    ui->parameterValueTableView->setModel(parameterValueModel);
}
//------------------------------------------------------------------------------
void ViewVerificationDialog::diagnosisChanged()
{
    // Получаем идентификатор выбранного диагноза:
    quint32 row = ui->diagnosisProbabilityTableView->currentIndex().row();
    Id diagnosisId = mDiagnosisProbabilityModel->data(mDiagnosisProbabilityModel->index(row, 0)).toULongLong();

    SqlQuery query;
    QString unknownParametersQuery("((SELECT ID, Name AS ParameterName FROM UnknownDiagnosisParameter WHERE PresumptiveVerificationDiagnosisID=?) AS ParameterTable)");
    QString researchesQuery("((SELECT Name AS ResearchName, UnknownDiagnosisParameterID FROM NecessaryAdditionalParameterResearch) AS ResearchTable)");
    query.prepare("SELECT ParameterTable.ParameterName, ResearchTable.ResearchName FROM "
                  + unknownParametersQuery + " LEFT OUTER JOIN "
                  + researchesQuery + " ON ParameterTable.ID=ResearchTable.UnknownDiagnosisParameterID");
    query.addBindValue(diagnosisId);
    query.exec();
    qDebug() << "size:" << query.size();//-


    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(query);

    model->setHeaderData(0, Qt::Horizontal, tr("Unknown Parameter"));
    model->setHeaderData(1, Qt::Horizontal, tr("Necessary Research"));

    ui->unknownParameterTableView->setModel(model);


    // Отображаем описания параметров для выделенного диагноза:
    displayParameters(true, diagnosisId);
}
//------------------------------------------------------------------------------
void ViewVerificationDialog::on_dialogButtonBox_accepted()
{
     done(QDialog::Accepted);
}
//------------------------------------------------------------------------------
