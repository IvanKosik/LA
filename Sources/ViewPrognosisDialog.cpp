#include "ViewPrognosisDialog.h"
#include "ui_ViewPrognosisDialog.h"

#include "SqlQuery.h"

#include <QDebug> //-
//------------------------------------------------------------------------------
ViewPrognosisDialog::ViewPrognosisDialog(Id prognosisId, QWidget *parent)
    : mPrognosisId(prognosisId), QDialog(parent), ui(new Ui::ViewPrognosisDialog)
{
    ui->setupUi(this);


    // Получим название диагноза:
    SqlQuery prognosisQuery;
    prognosisQuery.prepare("SELECT DiagnosisName FROM Prognosis WHERE ID=?");
    prognosisQuery.addBindValue(mPrognosisId);
    prognosisQuery.exec();
    prognosisQuery.first();
    QString diagnosisName = prognosisQuery.value(0).toString();

    // Выводим название диагноза:
    ui->diagnosisNameLabel->setText(ui->diagnosisNameLabel->text() + diagnosisName);


    // Настраиваем таблицы:
    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->parameterValueTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-


    // Отображаем значения параметров диагноза:
    QSqlQueryModel *parameterValueModel = new QSqlQueryModel(this);
    SqlQuery parameterValueQuery;
    parameterValueQuery.prepare("SELECT Name, Value FROM ParameterValue WHERE PrognosisID=?");
    parameterValueQuery.addBindValue(mPrognosisId);
    // Если запрос выполнен удачно:
    if (parameterValueQuery.exec()) {
        parameterValueModel->setQuery(parameterValueQuery);
    }

    parameterValueModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    parameterValueModel->setHeaderData(1, Qt::Horizontal, tr("Value"));

    ui->parameterValueTableView->setModel(parameterValueModel);


    // Отображаем вероятности фаз:
    mPhaseProbabilityModel = new QSqlQueryModel(this);

    ui->phaseProbabilityTableView->adjust(mPhaseProbabilityModel);

    SqlQuery phaseProbabilityQuery;
    QString query("SELECT ID, Name, Probability FROM PhaseProbability WHERE PrognosisID=?");
    phaseProbabilityQuery.prepare(query);
    phaseProbabilityQuery.addBindValue(mPrognosisId);
    // Если запрос выполнен удачно:
    if (phaseProbabilityQuery.exec()) {
        mPhaseProbabilityModel->setQuery(phaseProbabilityQuery);
    }

    mPhaseProbabilityModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    mPhaseProbabilityModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
    mPhaseProbabilityModel->setHeaderData(2, Qt::Horizontal, tr("Probability, %"));

    // Скрываем колонку идентификаторов:
    ui->phaseProbabilityTableView->setColumnHidden(0, true);

    connect(ui->phaseProbabilityTableView, SIGNAL(currentChanged()),
            SLOT(phaseChanged()));


    // Отображаем неизвестные параметры и дополнительные исследования:
    // Выделяем первую фазу, чтобы сразу показывались её неизвестные параметры:
    if (mPhaseProbabilityModel->rowCount() > 0) {
        ui->phaseProbabilityTableView->selectRow(0);
    }

    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->unknownParameterTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-
}
//------------------------------------------------------------------------------
ViewPrognosisDialog::~ViewPrognosisDialog()
{
    delete ui;
}
//------------------------------------------------------------------------------
void ViewPrognosisDialog::phaseChanged()
{
    // Получаем идентификатор выбранной фазы:
    quint32 row = ui->phaseProbabilityTableView->currentIndex().row();
    Id phaseId = mPhaseProbabilityModel->data(mPhaseProbabilityModel->index(row, 0)).toULongLong();

    SqlQuery query;
    QString unknownParametersQuery("((SELECT ID, Name AS ParameterName FROM UnknownPhaseParameter WHERE PhaseProbabilityID=?) AS ParameterTable)");
    QString researchesQuery("((SELECT Name AS ResearchName, UnknownPhaseParameterID FROM NecessaryResearch) AS ResearchTable)");
    query.prepare("SELECT ParameterTable.ParameterName, ResearchTable.ResearchName FROM "
                  + unknownParametersQuery + " LEFT OUTER JOIN "
                  + researchesQuery + " ON ParameterTable.ID=ResearchTable.UnknownPhaseParameterID");
    query.addBindValue(phaseId);
    query.exec();
    qDebug() << "size:" << query.size();//-


    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(query);

    model->setHeaderData(0, Qt::Horizontal, tr("Unknown Parameter"));
    model->setHeaderData(1, Qt::Horizontal, tr("Necessary Research"));

    ui->unknownParameterTableView->setModel(model);
}
//------------------------------------------------------------------------------
void ViewPrognosisDialog::on_dialogButtonBox_accepted()
{
    done(QDialog::Accepted);
}
//------------------------------------------------------------------------------
