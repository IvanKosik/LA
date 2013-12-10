#include "VerificationDialog.h"
#include "ui_Verification.h"

#include "ItemDelegates.h"
//-#include "SqlQueryModel.h"
#include "SqlQuery.h"
#include "DB.h"

#include <QSqlQuery>
//#include <QTableWidgetItem>
#include <QAbstractButton>
#include <QDateTime>
#include <QPushButton>

#include <QMessageBox> //-
#include <QDebug> //-
#include <QSqlError> //-

typedef QPair<quint64, quint64> IdPair;

//%const char *ColumnNames = "ColumnNames"; Если делать так, то перевод не находит.
//------------------------------------------------------------------------------
// ReadableVerificationParameter:
// Readable field names:
const char *ReadableVerificationParameter::Name = QT_TRANSLATE_NOOP("ColumnNames", "Name");
const char *ReadableVerificationParameter::Value = QT_TRANSLATE_NOOP("ColumnNames", "Value");
const char *ReadableVerificationParameter::Description = QT_TRANSLATE_NOOP("ColumnNames", "Additional Description");
//------------------------------------------------------------------------------
// ReadableDiagnosis:
// Readable field names:
const char *ReadableDiagnosis::Name = QT_TRANSLATE_NOOP("ColumnNames", "Name");
const char *ReadableDiagnosis::Probability = QT_TRANSLATE_NOOP("ColumnNames", "Probability, %");
//------------------------------------------------------------------------------
// ReadableUnknownParameter:
// Readable field names:
const char *ReadableUnknownParameter::ParameterName = QT_TRANSLATE_NOOP("ColumnNames", "Unknown Parameter");
const char *ReadableUnknownParameter::ResearchName = QT_TRANSLATE_NOOP("ColumnNames", "Necessary Research");
//------------------------------------------------------------------------------
VerificationDialog::VerificationDialog(quint64 patientId, QWidget *parent)
    : mPatientId(patientId), QDialog(parent), ui(new Ui::Verification)
{
    ui->setupUi(this);

    ui->dialogButtonBox->button(QDialogButtonBox::Save)->setDefault(true);

    setModal(true);

    SqlQuery query;
    query.exec("SELECT ID, Name FROM Parameter");

    // � азъединяем сигнал со слотом на время заполнения таблицы:
    disconnect(ui->verificationParameterTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
               this, SLOT(on_verificationParameterTableWidget_itemChanged(QTableWidgetItem*)));

    ui->verificationParameterTableWidget->setRowCount(query.size());
    QStringList headerLabels;
    headerLabels << QCoreApplication::translate("ColumnNames", ReadableVerificationParameter::Name)
                 << QCoreApplication::translate("ColumnNames", ReadableVerificationParameter::Value)
                 << QCoreApplication::translate("ColumnNames", ReadableVerificationParameter::Description);
    ui->verificationParameterTableWidget->setHorizontalHeaderLabels(headerLabels);
    //-ui->verificationParameterTableWidget->setItemDelegateForColumn(0, new NoEditItemDelegate);
    // � астягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->verificationParameterTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-


    quint32 curRow = 0;
    mAllParameterIds.clear();
    while (query.next()) {
        quint64 parameterId = query.value(0).toULongLong();
        mAllParameterIds.append(parameterId);

        QString parameterName = query.value(1).toString();

        QTableWidgetItem *nameItem = new QTableWidgetItem(parameterName);
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem *valueItem = new QTableWidgetItem();

        QTableWidgetItem *descriptionItem = new QTableWidgetItem();
        descriptionItem->setFlags(descriptionItem->flags() ^ Qt::ItemIsEditable);

        ui->verificationParameterTableWidget->setItem(curRow, 0, nameItem);
        ui->verificationParameterTableWidget->setItem(curRow, 1, valueItem);
        ui->verificationParameterTableWidget->setItem(curRow, 2, descriptionItem);

        ++curRow;
    }


    // Устанавливаем ширину рядов по содержимому:
    //%ui->verificationParameterTableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);


    // Соединяем сигнал со слотом после заполнения таблицы:
    connect(ui->verificationParameterTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(on_verificationParameterTableWidget_itemChanged(QTableWidgetItem*)));


    // Настраиваем таблицу диагнозов:
    headerLabels.clear();
    headerLabels << QCoreApplication::translate("ColumnNames", ReadableDiagnosis::Name)
                 << QCoreApplication::translate("ColumnNames", ReadableDiagnosis::Probability);
    ui->diagnosisTableWidget->setHorizontalHeaderLabels(headerLabels);
    ui->diagnosisTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // � астягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->diagnosisTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-


    // Настраиваем таблицу неизвестных параметров:
    headerLabels.clear();
    headerLabels << QCoreApplication::translate("ColumnNames", ReadableUnknownParameter::ParameterName)
                 << QCoreApplication::translate("ColumnNames", ReadableUnknownParameter::ResearchName);
    ui->unknownParameterTableWidget->setHorizontalHeaderLabels(headerLabels);
    ui->unknownParameterTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // � астягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->unknownParameterTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-


    setResult(QDialog::Rejected);
}
//------------------------------------------------------------------------------
VerificationDialog::~VerificationDialog()
{
    delete ui;
}
//------------------------------------------------------------------------------
void VerificationDialog::closeEvent(QCloseEvent *)
{
    cancel();
}
//------------------------------------------------------------------------------
bool VerificationDialog::save()
{
//    qDebug() << "-------";//-

    // Добавим данные верификации:
    SqlQuery verificationQuery;
    verificationQuery.prepare("INSERT INTO Verification (ID, PatientID, DateTime, Conclusion) VALUES (NULL, ?, ?, ?)");
    verificationQuery.addBindValue(mPatientId);
    verificationQuery.addBindValue(QDateTime::currentDateTime());
    verificationQuery.addBindValue(formConclusion());
    if (!verificationQuery.exec()) {
        return false;
    }

    quint64 verificationId = verificationQuery.lastInsertId().toULongLong();

    // Сохраняем указанные пользователем значения параметров:
    QHash<Id, Id> verificationParameters; // QHash<parameterId, verificationParameterId>.
    // Служит для сохранения доп. описаний.
    for (qint32 i = 0; i < ui->verificationParameterTableWidget->rowCount(); ++i) {
        QTableWidgetItem *valueItem = ui->verificationParameterTableWidget->item(i, 1);
        // Если указано значение параметра:
        QString strValue = valueItem->data(Qt::DisplayRole).toString();
        if (!strValue.isEmpty()) {
//            qDebug() << "save:" << strValue; //-

            QTableWidgetItem *nameItem = ui->verificationParameterTableWidget->item(i, 0);

            SqlQuery verificationParameterQuery;
            verificationParameterQuery.prepare(QString("INSERT INTO VerificationParameter ")
                    + QString("(ID, Name, Value, VerificationID) VALUES (NULL, ?, ?, ?)"));
            verificationParameterQuery.addBindValue(nameItem->data(Qt::DisplayRole).toString());
            verificationParameterQuery.addBindValue(strValue.toDouble());
            verificationParameterQuery.addBindValue(verificationId);
            if (!verificationParameterQuery.exec()) {
                return false;
            } else {
                // Добавляем пары в хэш, дальше используем его для сохранения описаний:
                Id parameterId = mAllParameterIds.at(i);
                Id verificationParameterId = verificationParameterQuery.lastInsertId().toULongLong();
                verificationParameters.insert(parameterId, verificationParameterId);
            }
        }
    }


    // Сохраняем вероятности предполагаемых диагнозов:
    for (qint32 i = 0; i < ui->diagnosisTableWidget->rowCount(); ++i) {
        QTableWidgetItem *nameItem = ui->diagnosisTableWidget->item(i, 0);
        QTableWidgetItem *probabilityItem = ui->diagnosisTableWidget->item(i, 1);

        SqlQuery presumptiveVerificationDiagnosisQuery;
        presumptiveVerificationDiagnosisQuery.prepare(QString("INSERT INTO PresumptiveVerificationDiagnosis ")
                + QString("(ID, Name, Probability, VerificationID) VALUES (NULL, ?, ?, ?)"));
        presumptiveVerificationDiagnosisQuery.addBindValue(nameItem->data(Qt::DisplayRole).toString());
        presumptiveVerificationDiagnosisQuery.addBindValue(probabilityItem->data(Qt::DisplayRole).toDouble());
        presumptiveVerificationDiagnosisQuery.addBindValue(verificationId);
        presumptiveVerificationDiagnosisQuery.exec();


        // Сохраняем для каждого диагноза список неизвестных параметров:
        Id presumptiveVerificationDiagnosisId
                = presumptiveVerificationDiagnosisQuery.lastInsertId().toULongLong();
        if (!saveUnknownDiagnosisParameters(mUnknownDiagnosisParameters.at(i),
                                       presumptiveVerificationDiagnosisId)) {
            return false;
        }


        // Сохраняем для каждого диагноза дополнительные описания параметров:
        if (!saveDescriptions(mDiagnosisParameterDescriptions.at(i),
                              verificationParameters,
                              presumptiveVerificationDiagnosisId)) {
            return false;
        }
    }

    return true;
}
//------------------------------------------------------------------------------
void VerificationDialog::cancel()
{
}
//------------------------------------------------------------------------------
/*-void VerificationDialog::on_dialogButtonBox_accepted()
{
    if (save()) {
        done(QDialog::Accepted);
    }
}-*/
//------------------------------------------------------------------------------
void VerificationDialog::on_dialogButtonBox_rejected()
{
    cancel();
    done(result()); // Если нажимали до этого "Сохранить", то вернет QDialog::Accepted.
}
//------------------------------------------------------------------------------
void VerificationDialog::on_dialogButtonBox_clicked(QAbstractButton *button)
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
void VerificationDialog::updateProbabilitiesOfAllDiagnoses()
{
    // Получаем список всех идентификаторов диагнозов:
    SqlQuery query;
    query.exec("SELECT ID FROM Diagnosis");
    QList<quint64> diagnosisIds;
    while (query.next()) {
        quint64 diagnosisId = query.value(0).toULongLong();
        diagnosisIds.append(diagnosisId);
    }

    // Обновляем значения вероятностей для всех диагнозов:
    updateDiagnosisProbabilities(diagnosisIds);
}
//------------------------------------------------------------------------------
QList<quint64> VerificationDialog::diagnosesWithParameter(quint64 parameterId)
{

}
//------------------------------------------------------------------------------
void VerificationDialog::updateDiagnosisProbabilities(QList<quint64> &diagnosisIds)
{
    // Оставим только уникальные идентификаторы:
    QList<quint64> uniqueDiagnosisIds;
    std::unique_copy(diagnosisIds.begin(), diagnosisIds.end(),
                     std::back_inserter(uniqueDiagnosisIds));

    // Находим среднюю вероятность каждого диагноза:
    foreach (quint64 diagnosisId, uniqueDiagnosisIds) {
        mUnknownParameters.clear();
        mDiagnosisParameterDescription.clear();

        qreal averageDiagnosisProbability
                = calculateAverageDiagnosisProbability(diagnosisId);

//        qDebug() << "diagnosisId: " << diagnosisId
//                 << "\nProbability: " << averageDiagnosisProbability;//-

        // В таблицу заносим только те диагнозы, вероятность которых не равна 0:
        if (averageDiagnosisProbability != 0) {
            // Заносим ID диагноза в список:
            mDiagnosisIds.append(diagnosisId);

            // Добавляем один ряд к таблице:
            qint32 rowCount = ui->diagnosisTableWidget->rowCount();
            ui->diagnosisTableWidget->setRowCount(rowCount + 1);

            QString diagnosisName = DB::getDiagnosisName(diagnosisId);
            QTableWidgetItem *nameItem = new QTableWidgetItem(diagnosisName);

            QString diagnosisProbability = QString::number(averageDiagnosisProbability * 100, 'f', 2);
            QTableWidgetItem *probabilityItem = new QTableWidgetItem(diagnosisProbability);

            ui->diagnosisTableWidget->setItem(rowCount, 0, nameItem);
            ui->diagnosisTableWidget->setItem(rowCount, 1, probabilityItem);

            mUnknownDiagnosisParameters.append(mUnknownParameters);

            mDiagnosisParameterDescriptions.append(mDiagnosisParameterDescription);
        }
    }


    // Если в таблице есть хотя бы один диагноз:
    if (ui->diagnosisTableWidget->rowCount() > 0) {
        // Если в таблице есть диагоз, который был выделен до этого:
        if (mDiagnosisIds.contains(mLastSelectedDiagnosis)) {
            // Выделим его:
            qint32 row = mDiagnosisIds.indexOf(mLastSelectedDiagnosis);
            ui->diagnosisTableWidget->selectRow(row);
        } else {
            // Выделим самый первый, чтобы сразу отображались неизвестные
            // параметры и необходимые исследования:
            ui->diagnosisTableWidget->selectRow(0);
        }
    }
}
//------------------------------------------------------------------------------
qreal VerificationDialog::calculateAverageDiagnosisProbability(quint64 diagnosisId)
{
    // Находим фазы диагноза:
    SqlQuery phaseIdQuery;
    phaseIdQuery.prepare("SELECT ID FROM Phase WHERE DiagnosisID=?");
    phaseIdQuery.addBindValue(diagnosisId);
    phaseIdQuery.exec();
    qreal averageDiagnosisProbability = 0; // Средняя вероятность по всем фазам диагноза.
    // Находим среднюю вероятность каждой фазы диагноза:
    while (phaseIdQuery.next()) {
        qreal averagePhaseProbability
                = calculateAveragePhaseProbability(phaseIdQuery.value(0).toULongLong());
        averageDiagnosisProbability += averagePhaseProbability / phaseIdQuery.size();
    }

    return averageDiagnosisProbability;
}
//------------------------------------------------------------------------------
qreal VerificationDialog::calculateAveragePhaseProbability(quint64 phaseId)
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
qreal VerificationDialog::getPhaseParameterProbability(quint64 phaseParameterId,
                                                       quint64 parameterId)
{
    // Находим вероятность параметра, указанную пользователем:
//    qDebug() << "\t--- ---"; //-
//    qDebug() << "phaseParameterId" << phaseParameterId; //-
//    qDebug() << "parameterId" << parameterId; //-

    qint32 row = mAllParameterIds.indexOf(parameterId);
    // Если не нашли параметр в списке параметров, который создали чуть раньше:
    if (row == -1) {
        mUnknownParameters.insert(parameterId);
        return 0;
    }


//    qDebug() << "row: " << row; //-

    // Получаем значение параметра, указанное пользователем:
    QTableWidgetItem *userValueItem = ui->verificationParameterTableWidget->item(row, 1);
    QString stringUserValue = userValueItem->data(Qt::DisplayRole).toString();
    // Если значение параметра не указано пользователем:
    if (stringUserValue.isEmpty()) {
        mUnknownParameters.insert(parameterId);
        return 0;
    }


    double userValue = stringUserValue.toDouble();
//    qDebug() << "userValue:" << userValue;

    // Находим диапазоны, в которые попало пользовательское значение параметра:
    SqlQuery rangeQuery;
    rangeQuery.prepare(QString("SELECT RangeBegin, RangeEnd, BeginProbability, EndProbability, Description ")
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

    rangeQuery.first();


    // Добавляем дополнительно описание в хеш:
    QString description = rangeQuery.value(4).toString();
    QString oldDescription = mDiagnosisParameterDescription.value(parameterId);
    if (!oldDescription.isEmpty()) {
        oldDescription.append('\n');
    }
    mDiagnosisParameterDescription.insert(parameterId, oldDescription + description);


    // Находим значение вероятности, соответствующее введенному пользователем
    // значению параметра:
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
void VerificationDialog::addNecessaryResearches(quint64 parameterId)
{
    // Получаем все исследования, с помощью которых можно получить значение
    // параметра:
    QSet<quint64> researchIds = DB::getParameterResearches(parameterId);

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
    foreach (quint64 researchId, researchIds) {
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
bool VerificationDialog::saveUnknownDiagnosisParameters(QSet<quint64> unknownDiagnosisParameters,
                                                        quint64 diagnosisId)
{
    foreach (quint64 parameterId, unknownDiagnosisParameters) {
        SqlQuery query;
        query.prepare(QString("INSERT INTO UnknownDiagnosisParameter ")
                      + QString("(ID, Name, PresumptiveVerificationDiagnosisID) VALUES (NULL, ?, ?)"));
        query.addBindValue(DB::getParameterName(parameterId));
        query.addBindValue(diagnosisId);
        if (!query.exec()) {
            return false;
        }


        // Сохраняем для каждого параметра список исследований, с помощью которых его можно определить:
        quint64 unknownDiagnosisParameterId = query.lastInsertId().toULongLong();
        if (!saveResearches(DB::getParameterResearches(parameterId),
                            unknownDiagnosisParameterId)) {
            return false;
        }
    }

    return true;
}
//------------------------------------------------------------------------------
bool VerificationDialog::saveResearches(QSet<quint64> parameterResearches,
                                        quint64 unknownDiagnosisParameterId)
{
    foreach (Id researchId, parameterResearches) {
        SqlQuery query;
        query.prepare(QString("INSERT INTO NecessaryAdditionalParameterResearch ")
                      + QString("(ID, Name, UnknownDiagnosisParameterID) VALUES (NULL, ?, ?)"));
        query.addBindValue(DB::getResearchName(researchId));
        query.addBindValue(unknownDiagnosisParameterId);
        if (!query.exec()) {
            return false;
        }
    }

    return true;
}
//------------------------------------------------------------------------------
bool VerificationDialog::saveDescriptions(QHash<Id, QString> parameterDescriptions,
                                          QHash<Id, Id> verificationParameters,
                                          Id diagnosisId)
{
    QHash<Id, QString>::const_iterator i = parameterDescriptions.constBegin();
    while (i != parameterDescriptions.constEnd()) {
        QString description = i.value();

        Id parameterId = i.key();
        Id verificationParameterId = verificationParameters.value(parameterId);

        SqlQuery query;
        query.prepare(QString("INSERT INTO VerificationDescription ")
                      + QString("(ID, Text, VerificationParameterID, PresumptiveVerificationDiagnosisID) ")
                      + QString("VALUES (NULL, ?, ?, ?)"));
        query.addBindValue(description);
        query.addBindValue(verificationParameterId);
        query.addBindValue(diagnosisId);
        if (!query.exec()) {
            return false;
        }

        ++i;
    }

    return true;
}
//------------------------------------------------------------------------------
QString VerificationDialog::formConclusion()
{
    // В заключение добавляем только те диагнозы, вероятность которых больше 50%:
    QStringList diagnoses;
    diagnoses.clear();
    for (quint32 i = 0; i < ui->diagnosisTableWidget->rowCount(); ++i) {
        // Если вероятность диагноза > 50%:
        qreal probability = ui->diagnosisTableWidget->item(i, 1)->data(Qt::DisplayRole).toDouble();
        if (probability > 50) {
            QString diagnosisName = ui->diagnosisTableWidget->item(i, 0)->data(Qt::DisplayRole).toString();
            qDebug() << probability;
            diagnoses.append(diagnosisName + " - " + QString::number(probability));
        }
    }

    QString conclusion = "";
    // Если есть хотя бы один диагноз с нужной вероятностью:
    if (!diagnoses.isEmpty()) {
        conclusion.append(tr("Most probable diagnoses:"));
        foreach (QString diagnosis, diagnoses) {
            conclusion.append("\n\t" + diagnosis);
        }
    }

    qDebug() << conclusion;
    return conclusion;
}
//------------------------------------------------------------------------------
void VerificationDialog::on_verificationParameterTableWidget_itemChanged(QTableWidgetItem *item)
{
    // Если изменено именно значение параметра (а не доп. описание, например):
    if (item->column() == 1) {
        // Очищаем диагнозы:
        // Очищаем таблицу диагнозов:
        ui->diagnosisTableWidget->setRowCount(0);
        // Очищаем список диагнозов:
        mDiagnosisIds.clear();

        // Очищаем неизвестные параметры:
        // Очищаем таблицу неизвестных параметров:
        ui->unknownParameterTableWidget->setRowCount(0);
        // Очищаем список неизвестных параметров:
        mUnknownDiagnosisParameters.clear();

        // Очищаем дополнительные исследования:
        // Очищаем доп. описание в колонке таблицы:
        QTableWidgetItem *descriptionItem;
        descriptionItem = ui->verificationParameterTableWidget->item(item->row(), 2);
        descriptionItem->setData(Qt::DisplayRole, QVariant());
        // Очищаем список дополнительных исследований:
        mDiagnosisParameterDescriptions.clear();


        updateProbabilitiesOfAllDiagnoses();
    }
}
//------------------------------------------------------------------------------
void VerificationDialog::on_diagnosisTableWidget_currentItemChanged(QTableWidgetItem *current,
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


    mLastSelectedDiagnosis = mDiagnosisIds.at(current->row());


    ui->unknownParameterTableWidget->setRowCount(0);
    //%qDebug() << "currentRow:" << current->row();
    foreach (quint64 unknownParameterId, mUnknownDiagnosisParameters.at(current->row())) {
        // Добавим необходимые исследования для параметра в таблицу:
        addNecessaryResearches(unknownParameterId);
    }


    // Очищаем старые доп. описания:
    for (quint32 i = 0; i < ui->verificationParameterTableWidget->rowCount(); ++i) {
        ui->verificationParameterTableWidget->item(i, 2)->setData(Qt::DisplayRole, QVariant());
    }

    // Показываем новые описания значений параметров для текущего диагноза:
    QHash<Id, QString> diagnosisParameterDescription = mDiagnosisParameterDescriptions.at(current->row());
    QHash<Id, QString>::const_iterator i = diagnosisParameterDescription.constBegin();
    while (i != diagnosisParameterDescription.constEnd()) {
        // Находим ряд с данным параметром:
        qint32 row = mAllParameterIds.indexOf(i.key());

        ui->verificationParameterTableWidget->item(row, 2)->setData(Qt::DisplayRole, i.value());
        ++i;
    }
    //%Второй способ (он медленнее):
    /*%for (quint32 i = 0; i < mAllParameterIds.size(); ++i) {
        if (diagnosisParameterDescription.contains(mAllParameterIds.at(i))) {
            QString description = diagnosisParameterDescription.value(mAllParameterIds.at(i));
            ui->verificationParameterTableWidget->item(i, 2)->setData(Qt::DisplayRole, description);
        }
    }%*/
}
//------------------------------------------------------------------------------
