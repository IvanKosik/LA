#ifndef SQLTABLEMODEL_H
#define SQLTABLEMODEL_H

#include <QSqlTableModel>
//------------------------------------------------------------------------------
namespace Patient {
    // Table name:
    extern const QString TableName;

    // Field names:
    extern const QString Id;
    extern const QString LastName;
    extern const QString FirstName;
    extern const QString MiddleName;
    extern const QString BirthDate;
    extern const QString Sex;
    extern const QString Addresses;
    extern const QString Phones;
    extern const QString Job;
    extern const QString Post;
    extern const QString PassportSeries;
    extern const QString PassportNumber;
    extern const QString PassportIssueDate;
    extern const QString PassportIssuingAuthority;
    extern const QString PassportPersonalNumber;
    extern const QString AdditionalInformation;
}
//------------------------------------------------------------------------------
namespace ReadablePatient {
    // Readable field names:
    extern const char *LastName;
    extern const char *FirstName;
    extern const char *MiddleName;
}
//------------------------------------------------------------------------------
class SqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    //-explicit SqlTableModel(QObject *parent = 0);
    explicit SqlTableModel(const QString &tableName,
                                     QObject *parent = 0); //-! 2 explicit - это нормально?
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class PatientSqlTableModel : public SqlTableModel
{
    Q_OBJECT
public:
    explicit PatientSqlTableModel(QObject *parent = 0);
    /*-explicit PatientSqlTableModel(const QString &tableName,
                                            QObject *parent = 0);-*/
};
//------------------------------------------------------------------------------
#endif // SQLTABLEMODEL_H
