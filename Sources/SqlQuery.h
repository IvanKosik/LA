#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QSqlQuery>
//------------------------------------------------------------------------------
class SqlQuery : public QSqlQuery
{
    //-Q_OBJECT

public:
    explicit SqlQuery(const QString &query = QString(),
                      QSqlDatabase db = QSqlDatabase());

    bool exec();
    bool exec(const QString &query);

private:
    void showErrorMessage();
};
//------------------------------------------------------------------------------
#endif // SQLQUERY_H
