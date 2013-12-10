#include "VerificationParameterTableWidget.h"

#include "ItemDelegates.h"

#include <QSqlQuery>
#include <QHeaderView>

#include <QDebug> //-
//------------------------------------------------------------------------------
// ReadableVerificationParameter:
// Readable field names:
//const QString ReadableVerificationParameter::Name = QObject::tr("Name");
//const QString ReadableVerificationParameter::Value = QObject::tr("Value");
//------------------------------------------------------------------------------
VerificationParameterTableWidget::VerificationParameterTableWidget(QWidget *parent)
    : TableWidget(parent)
{
//    setColumnCount(2); //-Значение установлено и у формы. Но если убрать либо здесь, либо
//    //-у формы, то не отображает данные.

//    QSqlQuery query;
//    query.exec("SELECT Name FROM Parameter");

//    setRowCount(query.size());
//    QStringList headerLabels;
//    headerLabels << ReadableVerificationParameter::Name
//                 << ReadableVerificationParameter::Value;
//    setHorizontalHeaderLabels(headerLabels);
//    setItemDelegateForColumn(0, new NoEditItemDelegate);
//    // Растягиваем колонки на всю ширину. Плохо то, что после этого пользователю
//    // нельзя самому менять размер конкретной колонки.
//    horizontalHeader()->setResizeMode(QHeaderView::Stretch); //-

//    quint32 curRow = 0;
//    while (query.next()) {
//        QString parameterName = query.value(0).toString();
//        qDebug() << "parameterName" << query.value(0).toString(); //-
//        QTableWidgetItem *item = new QTableWidgetItem();
//        qDebug() << "columnCount()" << columnCount();
//        item->setText(parameterName);
//        setItem(curRow++, 0, item);
//    }
}
//------------------------------------------------------------------------------
