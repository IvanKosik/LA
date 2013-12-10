#include "SqlQuery.h"

#include <QMessageBox>
#include <QSqlError>
//------------------------------------------------------------------------------
SqlQuery::SqlQuery(const QString &query, QSqlDatabase db) : QSqlQuery(query, db)
{

}
//------------------------------------------------------------------------------
bool SqlQuery::exec()
{
    if (!QSqlQuery::exec()) {
        showErrorMessage();
        return false;
    }
    else {
        return true;
    }
}
//------------------------------------------------------------------------------
bool SqlQuery::exec(const QString &query)
{
    prepare(query);
    return exec();
}
//------------------------------------------------------------------------------
void SqlQuery::showErrorMessage()
{
    QMessageBox::warning(0, QObject::tr("Error"), QObject::tr("Cannot execute query!")
            + QObject::tr("\nError code: ") + QString::number(lastError().number())
            + QObject::tr("\nError text: ") + lastError().text());
}
//------------------------------------------------------------------------------
