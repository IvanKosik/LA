#ifndef Database_h
#define Database_h

#include <QString>
#include <QtSql/QSqlError>
#include <QSqlDatabase>
#include <QSet>
#include <QDateTime>


typedef quint64 Id;
typedef QSet<Id> IdSet;
typedef QList<Id> IdList;
typedef QString Name;
typedef QPair<Id, Name> IdNamePair;

//-Database * Database::mInstance = NULL;
//------------------------------------------------------------------------------
inline uint qHash(const QDateTime &key);
//------------------------------------------------------------------------------
// Use the Singleton pattern:
class DB {
//public:
    static DB* mInstance; //-public и т.п. не нужен статическому полю?

public:
    static DB * getInstance(); //-public со статиком методу нужен?

    qint32 openConnection(QString hostName, QString databaseName,
                           QString userName, QString password, QString port);
    void closeConnection();

    QString getLastErrorText();
    qint32  getLastErrorCode();

    bool executeScriptFromFile(QString hostName, QString databaseName,
                               QString userName, QString password, QString port,
                               QString scriptPath); //- Откуда вызывать эту штуку и когда (вызвать перед доступом к БД, чтобы создались таблицы и заполнилась инфа) (там используется не поле этого класса QSqlDatabase database, а просто другая локальная переменная для базы: можно ведь одновременно к одной базе подключиться и делать какие-то с ней транзакции?).

    static QString getDiagnosisName(quint64 diagnosisId);
    static QString getPhaseName(quint64 phaseId);
    static QString getParameterName(quint64 parameterId);
    static QString getResearchName(quint64 researchId);
    static QSet<quint64> getParameterResearches(quint64 parameterId);
    static bool getDiagnosisId(QString diagnosisName, quint64 &diagnosisId);
    static QSet<QPair<quint64, QString> > getDiagnosisParameters(quint64 diagnosisId);
    static QSet<quint64> getDiagnosisPhases(quint64 diagnosisId);
    static QSet<quint64> getPhaseParameters(quint64 phaseId);
    static QList<QPair<quint64, QString> > getAllDiagnosisIdsNames();
    static QSet<QPair<QDateTime, Id> > getPatientVerifications(Id patientId);
    static QSet<QPair<QDateTime, Id> > getPatientPrognoses(Id patientId);
    static QString getVerificationConclusion(Id expertiseId);
    static double getPhaseParameterWeight(Id phaseParameterId);

private:
    DB();
    /*-Database(const Database &); //-Может понадобиться только если наследоваться, а потом в конструктор посылать наследника.
    Database & operator=(const Database &); -*/ //-
    ~DB();

    void setLastError(QSqlError error);

    QSqlDatabase database; //-Можно наследовать класс от этого. тогда это вряд ли нужно. ТОЛЬКО ВОТ ТОГДА СИНГЛЕТОН ПОЛУЧИТСЯ ЛИ? ВЕДЬ КОНСТРУКТОР БАЗОВОГО КЛАССА БУДЕТ ДОСТУПЕН.

    QString lastErrorText;
    qint32  lastErrorCode;


    /*-static QString dbHost;
    static QString dbName;
    static QString dbUser;
    static QString dbPass;-*/
};
//------------------------------------------------------------------------------
#endif // Database_h
