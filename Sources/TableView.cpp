#include "TableView.h"

#include "SqlTableModel.h"
//#include "Database.h"

#include <QtGui/QHeaderView>

#include <QDebug> //-
//------------------------------------------------------------------------------
TableView::TableView(QWidget *parent) : QTableView(parent)
{
}
//------------------------------------------------------------------------------
void TableView::adjust(QAbstractTableModel *model)
{
    // Настраиваем в этом методе, а не в конструкторе, т.к. неизвестно, как
    // изменить параметры конструктора в дизайнере.
    setModel(model);

    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-
}
//------------------------------------------------------------------------------
/*-void TableView::adjust(QSqlQueryModel *model)
{
    // Настраиваем в этом методе, а не в конструкторе, т.к. неизвестно, как
    // изменить параметры конструктора в дизайнере.
    setModel(model);

    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-
}-*/
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PatientTableView::PatientTableView(QWidget *parent) : TableView(parent)
{
}
//------------------------------------------------------------------------------
void PatientTableView::adjust(QAbstractTableModel *model)
{
    TableView::adjust(model);

    // Запретим редактирование таблицы:
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    setSelectionBehavior(QAbstractItemView::SelectRows);

    //-ui.bookTable->setItemDelegate(new BookDelegate(ui.bookTable));
    //-setSelectionMode(QAbstractItemView::SingleSelection);

    SqlTableModel *tabelModel = (SqlTableModel *)model;
    setColumnHidden(tabelModel->fieldIndex(Patient::Id), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::BirthDate), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::Sex), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::Addresses), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::Phones), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::Job), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::Post), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::PassportSeries), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::PassportNumber), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::PassportIssueDate), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::PassportIssuingAuthority), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::PassportPersonalNumber), true);
    setColumnHidden(tabelModel->fieldIndex(Patient::AdditionalInformation), true);
}
//------------------------------------------------------------------------------
void PatientTableView::currentChanged(const QModelIndex &current,
                                      const QModelIndex &previous)
{
    TableView::currentChanged(current, previous);


//-    qDebug() << "cur:" << current << "pr:" << previous;
    if (current.row() != previous.row()) {
        emit currentPatientChanged();
    }
}
//------------------------------------------------------------------------------
/*-void PatientTableView::selectionChanged(const QItemSelection &selected,
                                        const QItemSelection &deselected)
{
    QAbstractItemView::selectionChanged(selected, deselected);

    qDebug() << "selChanged" << selected << deselected; //-



    // Если выделен хотя бы один пациент:
    if (!selected.isEmpty()) {
        PatientSqlTableModel *patientModel = (PatientSqlTableModel *)model();

        qint32 row = currentIndex().row();
        qint32 column = patientModel->fieldIndex(Patient::Id);
        quint64 patientId = patientModel->data(patientModel->index(row, column)).toULongLong();

        QMap<QDateTime, quint64> *verifications;// = new QMap<QDateTime, quint64>();
//        QMap<QDateTime, quint64> prognoses;



        qDebug() << "OK"; //-
    }
}-*/
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ProbabilityTableView::ProbabilityTableView(QWidget *parent)
    : TableView(parent)
{

}
//------------------------------------------------------------------------------
void ProbabilityTableView::adjust(QSqlQueryModel *model)
{
    TableView::adjust(model);

    // Запретим редактирование таблицы:
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    setSelectionBehavior(QAbstractItemView::SelectRows);
}
//------------------------------------------------------------------------------
void ProbabilityTableView::currentChanged(const QModelIndex &current,
                                                   const QModelIndex &previous)
{
    TableView::currentChanged(current, previous);


    if (current.row() != previous.row()) {
        emit currentChanged();
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*-VerificationParameterTableView::VerificationParameterTableView(QWidget *parent)
    : TableView(parent)
{
}
//------------------------------------------------------------------------------
void VerificationParameterTableView::adjust(QAbstractTableModel *model)
{
    TableView::adjust(model);

    //-horizontalHeader()->setEditTriggers(QAbstractItemView::AllEditTriggers);

    //-Запретим редактирование таблицы:
    //-setEditTriggers(QAbstractItemView::NoEditTriggers);

    //-ui.bookTable->setItemDelegate(new BookDelegate(ui.bookTable));
    //-setSelectionMode(QAbstractItemView::SingleSelection);
}-*/
//------------------------------------------------------------------------------
