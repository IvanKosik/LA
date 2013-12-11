#include "PrognosisDialog.h"
#include "ui_Prognosis.h"

#include "ItemDelegates.h"
#include "SqlQuery.h"
#include "DB.h"

#include <QMessageBox>
#include <QDateTime>
#include <QPushButton>

#include <QDebug> //-

//------------------------------------------------------------------------------
// ReadableParameterValue:
// Readable field names:
const char *ReadableParameterValue::Name = QT_TRANSLATE_NOOP("ColumnNames", "Name");
const char *ReadableParameterValue::Value = QT_TRANSLATE_NOOP("ColumnNames", "Value");
//------------------------------------------------------------------------------
// ReadablePhase:
// Readable field names:
const char *ReadablePhase::Name = QT_TRANSLATE_NOOP("ColumnNames", "Name");
const char *ReadablePhase::Probability = QT_TRANSLATE_NOOP("ColumnNames", "Probability, %");
//------------------------------------------------------------------------------
// ReadableUnknownParameter:
// Readable field names:
const char *ReadableUnknownParameter2::ParameterName = QT_TRANSLATE_NOOP("ColumnNames", "Unknown Parameter");
const char *ReadableUnknownParameter2::ResearchName = QT_TRANSLATE_NOOP("ColumnNames", "Necessary Research");
//------------------------------------------------------------------------------
PrognosisDialog::PrognosisDialog(quint64 patientId, QWidget *parent)
    : mPatientId(patientId), QDialog(parent), ui(new Ui::Prognosis)
{
    ui->setupUi(this);

    ui->dialogButtonBox->button(QDialogButtonBox::Save)->setDefault(true);

    setModal(true);


    // Заполняем выпадающий список диагнозами:
    mDiagnosisIdsNames = DB::getAllDiagnosisIdsNames();
    QStringList diagnosisNames;
    foreach (IdNamePair diagnosisIdName, mDiagnosisIdsNames) {
        diagnosisNames.append(diagnosisIdName.second);
    }
    ui->diagnosisComboBox->addItems(diagnosisNames);


    // Настраиваем таблицу значений параметров:
    QStringList headerLabels;
    headerLabels << QCoreApplication::translate("ColumnNames", ReadableParameterValue::Name)
                 << QCoreApplication::translate("ColumnNames", ReadableParameterValue::Value);
    ui->parameterValueTableWidget->setHorizontalHeaderLabels(headerLabels);
    //-ui->parameterValueTableWidget->setItemDelegateForColumn(0, new NoEditItemDelegate);
    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->parameterValueTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-


    // Настраиваем таблицу фаз:
    headerLabels.clear();
    headerLabels << QCoreApplication::translate("ColumnNames", ReadablePhase::Name)
                 << QCoreApplication::translate("ColumnNames", ReadablePhase::Probability);
    ui->phaseTableWidget->setHorizontalHeaderLabels(headerLabels);
    ui->phaseTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->phaseTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-


    // Настраиваем таблицу неизвестных параметров:
    headerLabels.clear();
    headerLabels << QCoreApplication::translate("ColumnNames", ReadableUnknownParameter2::ParameterName)
                 << QCoreApplication::translate("ColumnNames", ReadableUnknownParameter2::ResearchName);
    ui->unknownParameterTableWidget->setHorizontalHeaderLabels(headerLabels);
    ui->unknownParameterTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->unknownParameterTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-


    setResult(QDialog::Rejected);
}
//------------------------------------------------------------------------------
PrognosisDialog::~PrognosisDialog()
{
    delete ui;
}
//------------------------------------------------------------------------------
void PrognosisDialog::closeEvent(QCloseEvent *)
{
    cancel();
}
//------------------------------------------------------------------------------
bool PrognosisDialog::save()
{
    qDebug() << "-------";//-

    // Сохраним данные прогноза:
    SqlQuery prognosisQuery;
    prognosisQuery.prepare("INSERT INTO Prognosis (ID, PatientID, DateTime, DiagnosisName) VALUES (NULL, ?, ?, ?)");
    prognosisQuery.addBindValue(mPatientId);
    prognosisQuery.addBindValue(QDateTime::currentDateTime());
    prognosisQuery.addBindValue(ui->diagnosisComboBox->currentText());
    if (!prognosisQuery.exec()) {
        return false;
    }

    Id prognosisId = prognosisQuery.lastInsertId().toULongLong();

    // Сохраняем указанные пользователем значения параметров:
    for (qint32 i = 0; i < ui->parameterValueTableWidget->rowCount(); ++i) {
        QTableWidgetItem *valueItem = ui->parameterValueTableWidget->item(i, 1);
        // Если указано значение параметра:
        QString strValue = valueItem->data(Qt::DisplayRole).toString();
        if (!strValue.isEmpty()) {
            qDebug() << "save:" << strValue; //-

            QTableWidgetItem *nameItem = ui->parameterValueTableWidget->item(i, 0);

            SqlQuery parameterValueQuery;
            parameterValueQuery.prepare(QString("INSERT INTO ParameterValue ")
                    + QString("(ID, Name, Value, PrognosisID) VALUES (NULL, ?, ?, ?)"));
            parameterValueQuery.addBindValue(nameItem->data(Qt::DisplayRole).toString());
            parameterValueQuery.addBindValue(strValue.toDouble());
            parameterValueQuery.addBindValue(prognosisId);
            if (!parameterValueQuery.exec()) {
                return false;
            }
        }
    }


    // Сохраняем вероятности фаз диагноза:
    for (qint32 i = 0; i < ui->phaseTableWidget->rowCount(); ++i) {
        QTableWidgetItem *nameItem = ui->phaseTableWidget->item(i, 0);
        QTableWidgetItem *probabilityItem = ui->phaseTableWidget->item(i, 1);

        SqlQuery phaseProbabilityQuery;
        phaseProbabilityQuery.prepare(QString("INSERT INTO PhaseProbability ")
                + QString("(ID, Name, Probability, PrognosisID) VALUES (NULL, ?, ?, ?)"));
        phaseProbabilityQuery.addBindValue(nameItem->data(Qt::DisplayRole).toString());
        phaseProbabilityQuery.addBindValue(probabilityItem->data(Qt::DisplayRole).toDouble());
        phaseProbabilityQuery.addBindValue(prognosisId);
        phaseProbabilityQuery.exec();


        // Сохраняем для каждой фазы список неизвестных параметров:
        quint64 phaseProbabilityId = phaseProbabilityQuery.lastInsertId().toULongLong();
        if (!saveUnknownPhaseParameters(mUnknownPhaseParameters.at(i),
                                        phaseProbabilityId)) {
            return false;
        }
    }

    return true;
}
//------------------------------------------------------------------------------
bool PrognosisDialog::saveUnknownPhaseParameters(IdSet unknownPhaseParameters,
                                                 Id phaseProbabilityId)
{
    foreach (quint64 parameterId, unknownPhaseParameters) {
        SqlQuery query;
        query.prepare(QString("INSERT INTO UnknownPhaseParameter ")
                      + QString("(ID, Name, PhaseProbabilityID) VALUES (NULL, ?, ?)"));
        query.addBindValue(DB::getParameterName(parameterId));
        query.addBindValue(phaseProbabilityId);
        if (!query.exec()) {
            return false;
        }


        // Сохраняем для каждого параметра список исследований, с помощью которых его можно определить:
        quint64 unknownPhaseParameterId = query.lastInsertId().toULongLong();
        if (!saveResearches(DB::getParameterResearches(parameterId),
                            unknownPhaseParameterId)) {
            return false;
        }
    }

    return true;
}
//------------------------------------------------------------------------------
bool PrognosisDialog::saveResearches(IdSet parameterResearches,
                                     Id unknownPhaseParameterId)
{
    foreach (quint64 researchId, parameterResearches) {
        SqlQuery query;
        query.prepare(QString("INSERT INTO NecessaryResearch ")
                      + QString("(ID, Name, UnknownPhaseParameterID) VALUES (NULL, ?, ?)"));
        query.addBindValue(DB::getResearchName(researchId));
        query.addBindValue(unknownPhaseParameterId);
        if (!query.exec()) {
            return false;
        }
    }

    return true;
}
//------------------------------------------------------------------------------
void PrognosisDialog::cancel()
{
}
//------------------------------------------------------------------------------
/*-void PrognosisDialog::on_dialogButtonBox_accepted()
{
    if (save()) {
        done(QDialog::Accepted);
    }
}-*/
//------------------------------------------------------------------------------
void PrognosisDialog::on_dialogButtonBox_rejected()
{
    cancel();
    done(result()); // Если нажимали до этого "Сохранить", то вернет QDialog::Accepted.
}
//------------------------------------------------------------------------------
void PrognosisDialog::on_dialogButtonBox_clicked(QAbstractButton *button)
{
    if ((QPushButton *)button == ui->dialogButtonBox->button(QDialogButtonBox::Save)) {
        // Если данные успешно сохранены:
        if (save()) {
            setResult(QDialog::Accepted);
        }
    }

    if ((QPushButton *)button == ui->dialogButtonBox->button(QDialogButtonBox::Ok)) {
        if (save()) {
            done(QDialog::Accepted);
        }
    }
}
//------------------------------------------------------------------------------
void PrognosisDialog::updateProbabilitiesOfAllPhases()
{
    // Получаем список идентификаторов фаз указанного диагноза:
    SqlQuery query;
    query.prepare("SELECT ID FROM Phase WHERE DiagnosisID=?");
    query.addBindValue(mDiagnosisId);
    query.exec();
    IdList phaseIds;
    while (query.next()) {
        Id phaseId = query.value(0).toULongLong();
        phaseIds.append(phaseId);
    }

    // Обновляем значения вероятностей для всех фаз указанного диагноза:
    updatePhaseProbabilities(phaseIds);
}
//------------------------------------------------------------------------------
void PrognosisDialog::updatePhaseProbabilities(IdList &phaseIds)
{
    // Находим среднюю вероятность каждой фазы:
    foreach (Id phaseId, phaseIds) {
        mUnknownParameters.clear();

        // Средняя вероятность фазы без учета весов параметров:
        /*
        qreal averagePhaseProbability
                = calculateAveragePhaseProbability(phaseId);
        */
        // Средняя вероятность фазы с учетом весов параметров:
        qreal averageWeightPhaseProbability
                = calculateAverageWeightPhaseProbability(phaseId);

        qDebug() << "phaseId: " << phaseId
                 << "\nProbability: " << averageWeightPhaseProbability;//-

        // В таблицу заносим только те фазы, вероятность которых не равна 0:
        if (averageWeightPhaseProbability != 0) {
            // Добавляем один ряд к таблице:
            qint32 rowCount = ui->phaseTableWidget->rowCount();
            ui->phaseTableWidget->setRowCount(rowCount + 1);

            QString phaseName = DB::getPhaseName(phaseId);
            QTableWidgetItem *nameItem = new QTableWidgetItem(phaseName);

            QString phaseProbability = QString::number(averageWeightPhaseProbability * 100, 'f', 2);
            QTableWidgetItem *probabilityItem = new QTableWidgetItem(phaseProbability);

            ui->phaseTableWidget->setItem(rowCount, 0, nameItem);
            ui->phaseTableWidget->setItem(rowCount, 1, probabilityItem);

            mUnknownPhaseParameters.append(mUnknownParameters);
        }
    }

    // Если в таблице есть хотя бы одна фаза, то выделим её, чтобы сразу
    // отображались неизвестные параметры и необходимые исследования:
    if (ui->phaseTableWidget->rowCount() > 0) {
        ui->phaseTableWidget->selectRow(0);
    }
}
//------------------------------------------------------------------------------
qreal PrognosisDialog::calculateAveragePhaseProbability(Id phaseId)
{
    // Находим пары: идентификатор параметра фазы, идентификатор параметра:
    SqlQuery phaseParameterIdQuery;
    phaseParameterIdQuery.prepare("SELECT ID, ParameterID FROM PhaseParameter WHERE PhaseID=?");
    phaseParameterIdQuery.addBindValue(phaseId);
    phaseParameterIdQuery.exec();
    qreal averagePhaseProbability = 0; // Средняя вероятность по всем параметрам фазы.
    // Находим вероятности параметров, указанные пользователем:
    while (phaseParameterIdQuery.next()) {
        qreal phaseParameterProbability
                = getPhaseParameterProbability(phaseParameterIdQuery.value(0).toULongLong(),
                                               phaseParameterIdQuery.value(1).toULongLong());
        averagePhaseProbability += phaseParameterProbability / phaseParameterIdQuery.size();
    }

    return averagePhaseProbability;
}
//------------------------------------------------------------------------------
qreal PrognosisDialog::calculateAverageWeightPhaseProbability(Id phaseId)
{
    // Находим пары: идентификатор параметра фазы, идентификатор параметра:
    SqlQuery phaseParameterIdQuery;
    phaseParameterIdQuery.prepare("SELECT ID, ParameterID FROM PhaseParameter WHERE PhaseID=?");
    phaseParameterIdQuery.addBindValue(phaseId);
    phaseParameterIdQuery.exec();
    qreal weightPhaseProbabilitySum = 0; // Сумма всех вероятностей параметров фазы с учетом весов.
    // Находим вероятности параметров, указанные пользователем:
    double weightSum = 0; // Сумма весов всех параметров.
    while (phaseParameterIdQuery.next()) {
        qreal phaseParameterProbability
                = getPhaseParameterProbability(phaseParameterIdQuery.value(0).toULongLong(),
                                               phaseParameterIdQuery.value(1).toULongLong());
        // Вес текущего параметра стадии:
        double weight = DB::getPhaseParameterWeight(phaseParameterIdQuery.value(0).toULongLong());
        weightSum += weight;
        weightPhaseProbabilitySum += phaseParameterProbability * weight;
    }

    // Средняя вероятность по всем параметрам фазы с учетом весов.
    qreal averageWeightPhaseProbability = weightPhaseProbabilitySum / weightSum;
    return averageWeightPhaseProbability;
}
//------------------------------------------------------------------------------
qreal PrognosisDialog::getPhaseParameterProbability(Id phaseParameterId,
                                                    Id parameterId)
{
    // Находим вероятность параметра, указанную пользователем:
    qDebug() << "\t--- ---"; //-
    qDebug() << "phaseParameterId" << phaseParameterId; //-
    qDebug() << "parameterId" << parameterId; //-

    qint32 row = mAllParameterIds.indexOf(parameterId);
    // Если не нашли параметр в списке параметров, который создали чуть раньше:
    if (row == -1) {
        mUnknownParameters.insert(parameterId);
        return 0;
    }


    qDebug() << "row: " << row; //-

    // Получаем значение параметра, указанное пользователем:
    QTableWidgetItem *userValueItem = ui->parameterValueTableWidget->item(row, 1);
    QString stringUserValue = userValueItem->data(Qt::DisplayRole).toString();
    // Если значение параметра не указано пользователем:
    if (stringUserValue.isEmpty()) {
        mUnknownParameters.insert(parameterId);
        return 0;
    }


    double userValue = stringUserValue.toDouble();
    qDebug() << "userValue:" << userValue;

    // Находим диапазоны, в которые попало пользовательское значение параметра:
    SqlQuery rangeQuery;
    rangeQuery.prepare(QString("SELECT RangeBegin, RangeEnd, BeginProbability, EndProbability ")
                       + QString("FROM PhaseParameterRange WHERE PhaseParameterID=? AND RangeBegin<? AND ?<=RangeEnd"));
    rangeQuery.addBindValue(phaseParameterId);
    rangeQuery.addBindValue(userValue);
    rangeQuery.addBindValue(userValue);
    rangeQuery.exec();
    // Если не найдено диапазонов, в которые папало пользовательское значение параметра:
    if (rangeQuery.size() < 1) {
        return 0;
    }
    // Если пользовательское значение попало в несколько диапазонов:
    else if (rangeQuery.size() > 1) {
        // Получим название параметра:
        QString parameterName = DB::getParameterName(parameterId);
        QMessageBox::warning(this, tr("Error"), tr("Value ranges of ")
                             + parameterName + tr(" parameter should not intersect.\n")
                             + tr("Please correct them in the database."));
        return 0;
    }


    // Находим значение вероятности, соответствующее введенному пользователем
    // значению параметра:
    rangeQuery.first();
    double begin = rangeQuery.value(0).toDouble();
    double end = rangeQuery.value(1).toDouble();
    qreal beginProbability = rangeQuery.value(2).toReal();
    qreal endProbability = rangeQuery.value(3).toReal();

    double rangeLength = end - begin;
    // Изменение вероятности:
    double probabilityChange = endProbability - beginProbability;
    // Пользовательское значение параметра относительно начала диапазона:
    double relativeUserValue = userValue - begin;

    qreal probability = beginProbability + (relativeUserValue * probabilityChange) / rangeLength;

    return probability;
}
//------------------------------------------------------------------------------
void PrognosisDialog::addNecessaryResearches(Id parameterId)
{
    // Получаем все исследования, с помощью которых можно получить значение
    // параметра:
    IdSet researchIds = DB::getParameterResearches(parameterId);

    // Если нет исследований, позволяющих определить параметр:
    if (researchIds.isEmpty()) {
        // Добавляем один ряд к таблице:
        qint32 rowCount = ui->unknownParameterTableWidget->rowCount();
        ui->unknownParameterTableWidget->setRowCount(rowCount + 1);

        QString parameterName = DB::getParameterName(parameterId);
        QTableWidgetItem *parameterItem = new QTableWidgetItem(parameterName);

        ui->unknownParameterTableWidget->setItem(rowCount, 0, parameterItem);
    }

    // Если есть исследования, позволяющие определить параметр:
    foreach (Id researchId, researchIds) {
        // Добавляем один ряд к таблице:
        qint32 rowCount = ui->unknownParameterTableWidget->rowCount();
        ui->unknownParameterTableWidget->setRowCount(rowCount + 1);

        QString parameterName = DB::getParameterName(parameterId);
        QTableWidgetItem *parameterItem = new QTableWidgetItem(parameterName);

        QString researchName = DB::getResearchName(researchId);
        QTableWidgetItem *researchItem = new QTableWidgetItem(researchName);

        ui->unknownParameterTableWidget->setItem(rowCount, 0, parameterItem);
        ui->unknownParameterTableWidget->setItem(rowCount, 1, researchItem);
    }
}
//------------------------------------------------------------------------------
void PrognosisDialog::on_diagnosisLineEdit_textChanged(const QString &arg1)
{
    quint64 diagnosisId;
    // Если есть диагноз с таким названием:
    if (DB::getDiagnosisId(arg1, diagnosisId)) {
        QSet<QPair<quint64, QString> > diagnosisParameters
                = DB::getDiagnosisParameters(diagnosisId);


        // Разъединяем сигнал со слотом на время заполнения таблицы:
        disconnect(ui->parameterValueTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
                   this, SLOT(on_parameterValueTableWidget_itemChanged(QTableWidgetItem*)));

        ui->parameterValueTableWidget->setRowCount(diagnosisParameters.size());

        quint32 curRow = 0;
        mAllParameterIds.clear();
        foreach (IdNamePair diagnosisParameter, diagnosisParameters) {
            quint64 parameterId = diagnosisParameter.first;
            mAllParameterIds.append(parameterId);

            QString parameterName = diagnosisParameter.second;

            QTableWidgetItem *nameItem = new QTableWidgetItem(parameterName);
            QTableWidgetItem *valueItem = new QTableWidgetItem();

            ui->parameterValueTableWidget->setItem(curRow, 0, nameItem);
            ui->parameterValueTableWidget->setItem(curRow, 1, valueItem);

            ++curRow;
        }

        // Соединяем сигнал со слотом после заполнения таблицы:
        connect(ui->parameterValueTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
                this, SLOT(on_parameterValueTableWidget_itemChanged(QTableWidgetItem*)));
    }
}
//------------------------------------------------------------------------------
void PrognosisDialog::on_diagnosisComboBox_currentIndexChanged(int index)
{
    mDiagnosisId = mDiagnosisIdsNames.at(index).first;

    QSet<QPair<quint64, QString> > diagnosisParameters
            = DB::getDiagnosisParameters(mDiagnosisId);

    // Разъединяем сигнал со слотом на время заполнения таблицы:
    disconnect(ui->parameterValueTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
               this, SLOT(on_parameterValueTableWidget_itemChanged(QTableWidgetItem*)));

    ui->parameterValueTableWidget->setRowCount(diagnosisParameters.size());

    quint32 curRow = 0;
    mAllParameterIds.clear();
    foreach (IdNamePair diagnosisParameter, diagnosisParameters) {
        quint64 parameterId = diagnosisParameter.first;
        mAllParameterIds.append(parameterId);

        QString parameterName = diagnosisParameter.second;
        QTableWidgetItem *nameItem = new QTableWidgetItem(parameterName);
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *valueItem = new QTableWidgetItem();

        ui->parameterValueTableWidget->setItem(curRow, 0, nameItem);
        ui->parameterValueTableWidget->setItem(curRow, 1, valueItem);

        ++curRow;
    }

    // Соединяем сигнал со слотом после заполнения таблицы:
    connect(ui->parameterValueTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(on_parameterValueTableWidget_itemChanged(QTableWidgetItem*)));


    // Очищаем таблицы вероятностей фаз и неизвестных параметров:
    ui->phaseTableWidget->setRowCount(0);
    ui->unknownParameterTableWidget->setRowCount(0);
}
//------------------------------------------------------------------------------
void PrognosisDialog::on_parameterValueTableWidget_itemChanged(QTableWidgetItem *item)
{
    // Очищаем таблицы вероятностей фаз и неизвестных параметров:
    ui->phaseTableWidget->setRowCount(0);
    ui->unknownParameterTableWidget->setRowCount(0);
    mUnknownPhaseParameters.clear();

    updateProbabilitiesOfAllPhases();
}
//------------------------------------------------------------------------------
void PrognosisDialog::on_phaseTableWidget_currentItemChanged(QTableWidgetItem *current,
                                                             QTableWidgetItem *previous)
{
    // Если ничего не выделено:
    if (current == 0) {
        return;
    }

    // Если остался выделен тот же самый ряд в таблице:
    if ((previous != 0) && (current->row() == previous->row())) {
        return;
    }


    ui->unknownParameterTableWidget->setRowCount(0);
    qDebug() << "currentRow:" << current->row();
    foreach (Id unknownParameterId, mUnknownPhaseParameters.at(current->row())) {
        // Добавим необходимые исследования для параметра в таблицу:
        addNecessaryResearches(unknownParameterId);
    }
}
//------------------------------------------------------------------------------
