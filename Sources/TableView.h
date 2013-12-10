#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

#include <QtCore/QAbstractTableModel>
#include <QtSql/QSqlQueryModel>
//------------------------------------------------------------------------------
class TableView : public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = 0);

    virtual void adjust(QAbstractTableModel *model);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class PatientTableView : public TableView
{
    Q_OBJECT
public:
    explicit PatientTableView(QWidget *parent = 0);

    virtual void adjust(QAbstractTableModel *model);

protected:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

/*-protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);-*/

signals:
    void currentPatientChanged();
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class ProbabilityTableView : public TableView
{
    Q_OBJECT
public:
    explicit ProbabilityTableView(QWidget *parent = 0);

    virtual void adjust(QSqlQueryModel *model);

protected:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

signals:
    void currentChanged();
};
//------------------------------------------------------------------------------
/*-class VerificationParameterTableView : public TableView
{
    Q_OBJECT
public:
    explicit VerificationParameterTableView(QWidget *parent = 0);

    virtual void adjust(QAbstractTableModel *model);
};-*/
//------------------------------------------------------------------------------
#endif // TABLEVIEW_H
