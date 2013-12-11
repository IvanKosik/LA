#include "DB.h"

#include "SqlQuery.h"

#include <QFile>
#include <QDebug>
#include <QObject>
#include <QSqlQuery>
#include <QTextCodec>
#include <QMessageBox>

const QString DatabaseDriver = "QMYSQL"; //-? Or QMYSQL3 ?
const QString ConnectionName = "DatabaseConnection";

DB * DB::mInstance = NULL;
//------------------------------------------------------------------------------
DB * DB::getInstance()
{
    if (!mInstance) {
        mInstance = new DB();
    }

    return mInstance;
}
//------------------------------------------------------------------------------
qint32 DB::openConnection(QString hostName, QString databaseName,
                          QString userName, QString password, QString port)
{
    if (!QSqlDatabase::drivers().contains("QMYSQL")) {
        QMessageBox::critical(0, "Unable to load database", "This application needs the QMYSQL driver");
    }
    else {
        //-QMessageBox::information(0, "OK", "Driver was found");
    }

    //-! Может не работает из-за того, что я еще connectionName передаю!?
    //-!Когда удаляю ConnectionName, то хоть что-то появляется

    database = QSqlDatabase::addDatabase(DatabaseDriver);//-, ConnectionName); //-connectionName зачем это вообще надо мне?

    if(port == "")
        port = "3306"; // Default value for MySQL port.

    database.setHostName(hostName);
    database.setDatabaseName(databaseName);
    database.setUserName(userName);
    database.setPassword(password);
    database.setPort(port.toInt());

    if (!database.open()) {
        setLastError(database.lastError());

        return getLastErrorCode();
    }

    return 0;
}
//------------------------------------------------------------------------------
void DB::closeConnection()
{
    database.close();
    QSqlDatabase::database(ConnectionName).close(); //-?
    QSqlDatabase::removeDatabase(ConnectionName); //-?
}
//------------------------------------------------------------------------------
QString DB::getLastErrorText() {
    return lastErrorText;
}
//------------------------------------------------------------------------------
qint32 DB::getLastErrorCode() {
    return lastErrorCode;
}
//------------------------------------------------------------------------------
bool DB::executeScriptFromFile(QString hostName, QString databaseName,
                                     QString userName, QString password,
                                     QString port, QString scriptPath) //- лучше передавать отдельно Path и fileName (там дальше выводится имя файла при ошибке)
{

//    if (!QSqlDatabase::drivers().contains("QMYSQL3")) {
//        QMessageBox::critical(0, "Unable to load database", "This application needs the QMYSQL driver");
//    }
//    else {
//        QMessageBox::information(0, "OK", "Driver was found");
//    }


    //- Здесь создадим локальное соединение с БД (т.е. временное, запишем инфу со скрипта, а затем закроем соединение)    (сама БД - НЕ обязательно локальная... так что лучше изменить название переменной). ЛУЧШЕ НЕ ИСПОЛЬЗОВАТЬ в качестве названия database.
    QSqlDatabase localDatabase = QSqlDatabase::addDatabase(DatabaseDriver); //-?connection Name нужно или нет?

    /*-if(port == "")
        port = "3306"; // Default value for MySQL port. -*/ //- К одной базе одновременно через ОДИН порт подключаются или через разные?

    localDatabase.setHostName(hostName);
    localDatabase.setDatabaseName(databaseName);
    localDatabase.setUserName(userName);
    localDatabase.setPassword(password);
    localDatabase.setPort(port.toInt());

    if (!localDatabase.open()) {
        setLastError(localDatabase.lastError());

        return false;
    }


    QFile creationScriptFile("template_script.sql"); //-?

    if(scriptPath.length() != 0) { //-scriptPath != ""
        creationScriptFile.setFileName(scriptPath);
    }

    if(!creationScriptFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, QObject::tr("Error"),
                QObject::tr("Error opening create DB script-file %1").arg(creationScriptFile.fileName()));
        return true;
    }
    QTextStream textStream(&creationScriptFile);
    textStream.setCodec(QTextCodec::codecForName("UTF-8"));

    QString szData(""); //-queryFromFile or queryFromScripFile
    QTextStream out(&creationScriptFile);
    QString szLine;
    do {
        szLine = out.readLine(); //-? Почему бы не воспользоваться readAll();
        szData += szLine + "\r\n";
    }
    while(!szLine.isNull());

    creationScriptFile.close();
    //-qDebug() << szData; //-
    QSqlQuery execQueryInScript; //- sqlQuery or query
    if(execQueryInScript.exec(szData)) {
        /*-QMessageBox::information(0,
                                 QObject::tr("Execution"),
                                 QObject::tr("Query from script-file successfully executed"));-*/
        localDatabase.close();
        return true;
    }
    else {
        setLastError(execQueryInScript.lastError());
        localDatabase.close();
        return false;
    }
}
//------------------------------------------------------------------------------
QString DB::getDiagnosisName(quint64 diagnosisId)
{
    SqlQuery diagnosisNameQuery;
    diagnosisNameQuery.prepare("SELECT Name FROM Diagnosis WHERE ID=?");
    diagnosisNameQuery.addBindValue(diagnosisId);
    diagnosisNameQuery.exec();
    diagnosisNameQuery.first();
    QString diagnosisName = diagnosisNameQuery.value(0).toString();

    return diagnosisName;
}
//------------------------------------------------------------------------------
QString DB::getPhaseName(quint64 phaseId)
{
    SqlQuery phaseNameQuery;
    phaseNameQuery.prepare("SELECT Name FROM Phase WHERE ID=?");
    phaseNameQuery.addBindValue(phaseId);
    phaseNameQuery.exec();
    phaseNameQuery.first();
    QString phaseName = phaseNameQuery.value(0).toString();

    return phaseName;
}
//------------------------------------------------------------------------------
QString DB::getParameterName(quint64 parameterId)
{
    SqlQuery parameterNameQuery;
    parameterNameQuery.prepare("SELECT Name FROM Parameter WHERE ID=?");
    parameterNameQuery.addBindValue(parameterId);
    parameterNameQuery.exec();
    parameterNameQuery.first();
    QString parameterName = parameterNameQuery.value(0).toString();

    return parameterName;
}
//------------------------------------------------------------------------------
QString DB::getResearchName(quint64 researchId)
{
    SqlQuery researchQuery;
    researchQuery.prepare("SELECT Name FROM Research WHERE ID=?");
    researchQuery.addBindValue(researchId);
    researchQuery.exec();
    researchQuery.first();
    QString researchName = researchQuery.value(0).toString();

    return researchName;
}
//------------------------------------------------------------------------------
QSet<quint64> DB::getParameterResearches(quint64 parameterId)
{
    // Получаем все исследования, с помощью которых можно получить значение
    // параметра:
    SqlQuery researchQuery;
    researchQuery.prepare("SELECT ResearchID FROM ParameterResearch WHERE ParameterID=?");
    researchQuery.addBindValue(parameterId);
    researchQuery.exec();

    QSet<quint64> researchIds;
    while (researchQuery.next()) {
        quint64 researchId = researchQuery.value(0).toULongLong();
        researchIds.insert(researchId);
    }

    return researchIds;
}
//------------------------------------------------------------------------------
bool DB::getDiagnosisId(QString diagnosisName, quint64 &diagnosisId)
{
    SqlQuery diagnosisQuery;
    diagnosisQuery.prepare("SELECT ID FROM Diagnosis WHERE Name=?");
    diagnosisQuery.addBindValue(diagnosisName);
    diagnosisQuery.exec();

    // Если нашли диагноз с таким названием:
    if (diagnosisQuery.size() > 0) {
        diagnosisQuery.first();
        diagnosisId = diagnosisQuery.value(0).toULongLong();

        return true;
    }

    return false;
}
//------------------------------------------------------------------------------
QSet<QPair<quint64, QString> > DB::getDiagnosisParameters(quint64 diagnosisId)
{
    QSet<quint64> diagnosisPhases = getDiagnosisPhases(diagnosisId);

    QSet<QPair<quint64, QString> > diagnosisParameters;
    // Для каждой фазы диагноза находим список параметров:
    foreach (quint64 phaseId, diagnosisPhases) {
        QSet<quint64> parameterIds = getPhaseParameters(phaseId);

        // Добавляем все идентификаторы и названия параметров во множество:
        foreach (quint64 parameterId, parameterIds) {
            diagnosisParameters.insert(qMakePair(parameterId, getParameterName(parameterId)));
        }
    }

    return diagnosisParameters;
}
//------------------------------------------------------------------------------
QSet<quint64> DB::getDiagnosisPhases(quint64 diagnosisId)
{
    SqlQuery phaseQuery;
    phaseQuery.prepare("SELECT ID FROM Phase WHERE DiagnosisID=?");
    phaseQuery.addBindValue(diagnosisId);
    phaseQuery.exec();

    QSet<quint64> diagnosisPhases;
    while (phaseQuery.next()) {
        quint64 phaseId = phaseQuery.value(0).toULongLong();
        diagnosisPhases.insert(phaseId);
    }

    return diagnosisPhases;
}
//------------------------------------------------------------------------------
QSet<quint64> DB::getPhaseParameters(quint64 phaseId)
{
    SqlQuery parameterQuery;
    parameterQuery.prepare("SELECT ParameterID FROM PhaseParameter WHERE PhaseId=?");
    parameterQuery.addBindValue(phaseId);
    parameterQuery.exec();

    QSet<quint64> phaseParameters;
    while (parameterQuery.next()) {
        quint64 parameterId = parameterQuery.value(0).toULongLong();
        phaseParameters.insert(parameterId);
    }

    return phaseParameters;
}
//------------------------------------------------------------------------------
QList<QPair<quint64, QString> > DB::getAllDiagnosisIdsNames()
{
    SqlQuery diagnosisQuery;
    diagnosisQuery.exec("SELECT ID, Name FROM Diagnosis");

    QList<QPair<quint64, QString> > diagnosisIdsNames;
    while (diagnosisQuery.next()) {
        quint64 diagnosisId = diagnosisQuery.value(0).toULongLong();
        QString diagnosisName = diagnosisQuery.value(1).toString();

        diagnosisIdsNames.append(qMakePair(diagnosisId, diagnosisName));
    }

    return diagnosisIdsNames;
}
//------------------------------------------------------------------------------
QSet<QPair<QDateTime, Id> > DB::getPatientVerifications(Id patientId)
{
    SqlQuery verificationQuery;
    verificationQuery.prepare("SELECT DateTime, ID FROM Verification WHERE PatientID=?");
    verificationQuery.addBindValue(patientId);
    verificationQuery.exec();

    QSet<QPair<QDateTime, Id> > verifications;
    while (verificationQuery.next()) {
        QDateTime dateTime = verificationQuery.value(0).toDateTime();
        Id verificationId = verificationQuery.value(1).toULongLong();
        verifications.insert(qMakePair(dateTime, verificationId));
    }

    return verifications;
}
//------------------------------------------------------------------------------
QSet<QPair<QDateTime, Id> > DB::getPatientPrognoses(Id patientId)
{
    SqlQuery prognosisQuery;
    prognosisQuery.prepare("SELECT DateTime, ID FROM Prognosis WHERE PatientID=?");
    prognosisQuery.addBindValue(patientId);
    prognosisQuery.exec();

    QSet<QPair<QDateTime, Id> > prognoses;
    while (prognosisQuery.next()) {
        QDateTime dateTime = prognosisQuery.value(0).toDateTime();
        Id prognosisId = prognosisQuery.value(1).toULongLong();
        prognoses.insert(qMakePair(dateTime, prognosisId));
    }

    return prognoses;
}
//------------------------------------------------------------------------------
QString DB::getVerificationConclusion(Id expertiseId)
{
    SqlQuery query;
    query.prepare("SELECT Conclusion FROM Verification WHERE ID=?");
    query.addBindValue(expertiseId);
    query.exec();
    query.first();

    return query.value(0).toString();
}
//------------------------------------------------------------------------------
double DB::getPhaseParameterWeight(Id phaseParameterId)
{
    SqlQuery query;
    query.prepare("SELECT Weight FROM PhaseParameter WHERE ID = :id");
    query.bindValue(":id", phaseParameterId);
    query.exec();
    query.first();

    return query.value(0).toDouble();
}
//------------------------------------------------------------------------------
DB::DB()
{
}
//------------------------------------------------------------------------------
DB::~DB()
{
}
//------------------------------------------------------------------------------
void DB::setLastError(QSqlError error)
{
    lastErrorText = error.text();
    lastErrorCode = error.number();
}
//------------------------------------------------------------------------------
inline uint qHash(const QDateTime &key)
{
    return qHash(key.toString());
}
//------------------------------------------------------------------------------
