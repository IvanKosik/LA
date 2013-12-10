#ifndef SQLQUERYMODEL_H
#define SQLQUERYMODEL_H

#include <QSqlQueryModel>
//------------------------------------------------------------------------------
class SqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit SqlQueryModel(QObject *parent = 0);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class VerificationParameterSqlQueryModel : public SqlQueryModel
{
    Q_OBJECT
public:
    explicit VerificationParameterSqlQueryModel(QObject *parent = 0);

    //-Qt::ItemFlags flags(const QModelIndex &index) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role);
    //QVariant data(const QModelIndex &item, int role) const;
};
//------------------------------------------------------------------------------
#endif // SQLQUERYMODEL_H
