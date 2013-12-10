#include "SqlTableModel.h"

#include <QCoreApplication>

#include <QDebug> //-

//------------------------------------------------------------------------------
// Patient table:
// Table name:
const QString Patient::TableName = "Patient";

// Field names:
const QString Patient::Id = "ID";
const QString Patient::LastName = "LastName";
const QString Patient::FirstName = "FirstName";
const QString Patient::MiddleName = "MiddleName";
const QString Patient::BirthDate = "BirthDate";
const QString Patient::Sex = "Sex";
const QString Patient::Addresses = "Addresses";
const QString Patient::Phones = "Phones";
const QString Patient::Job = "Job";
const QString Patient::Post = "Post";
const QString Patient::PassportSeries = "PassportSeries";
const QString Patient::PassportNumber = "PassportNumber";
const QString Patient::PassportIssueDate = "PassportIssueDate";
const QString Patient::PassportIssuingAuthority = "PassportIssuingAuthority";
const QString Patient::PassportPersonalNumber = "PassportPersonalNumber";
const QString Patient::AdditionalInformation = "AdditionalInformation";
//------------------------------------------------------------------------------
// ReadablePatient:
// Readable field names:
const char *ReadablePatient::LastName = QT_TRANSLATE_NOOP("ColumnNames", "Last Name");
const char *ReadablePatient::FirstName = QT_TRANSLATE_NOOP("ColumnNames", "First Name");
const char *ReadablePatient::MiddleName = QT_TRANSLATE_NOOP("ColumnNames", "Middle Name");
//------------------------------------------------------------------------------
/*-SqlTableModel::SqlTableModel(QObject *parent)
    : QSqlTableModel(parent)
{
}-*/
//------------------------------------------------------------------------------
SqlTableModel::SqlTableModel(const QString &tableName,
                                                 QObject *parent)
    : QSqlTableModel(parent)
{
    setTable(tableName);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    qDebug() << "Edit strategy" << editStrategy(); //-
    qDebug()<< "select table: " <<select(); //-Чтобы потом не делать для всех зависимых select() каждый раз, можно попробовать переопределить этот метод (или еще какой)

    // Сортировка по фамилии:
//    setFilter("ORDER BY LastName");
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PatientSqlTableModel::PatientSqlTableModel(QObject *parent)
    : SqlTableModel(Patient::TableName, parent)
{   
    setHeaderData(fieldIndex(Patient::LastName), Qt::Horizontal,
                  QCoreApplication::translate("ColumnNames", ReadablePatient::LastName));
    setHeaderData(fieldIndex(Patient::FirstName), Qt::Horizontal,
                  QCoreApplication::translate("ColumnNames", ReadablePatient::FirstName));
    setHeaderData(fieldIndex(Patient::MiddleName), Qt::Horizontal,
                  QCoreApplication::translate("ColumnNames", ReadablePatient::MiddleName));
}
//------------------------------------------------------------------------------
