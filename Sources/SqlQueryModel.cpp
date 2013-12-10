#include "SqlQueryModel.h"

#include <QDebug> //-
//------------------------------------------------------------------------------
SqlQueryModel::SqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VerificationParameterSqlQueryModel::VerificationParameterSqlQueryModel(QObject *parent)
    : SqlQueryModel(parent)
{
    setQuery("SELECT Name FROM Parameter");

    //-insertColumn(1); // Колонка для значений параметров.

    //-setData(index(0, 1), 10, Qt::DisplayRole);
    //-setData(index(0, 1), 12, Qt::EditRole);

    //-setHeaderData(0, Qt::Horizontal, ReadableVerificationParameter::Name);
    //-setHeaderData(1, Qt::Horizontal, ReadableVerificationParameter::Value);
}
//------------------------------------------------------------------------------
//Qt::ItemFlags VerificationParameterSqlQueryModel::flags(const QModelIndex &index) const
//{
//    Qt::ItemFlags flags = SqlQueryModel::flags(index); //-? or QSqlQueryModel::flags(...

//    // Делаем колонку для значений параметров редактируемой:
//    if (index.column() == 1) {
//        flags |= Qt::ItemIsEditable;
//    }

//    return flags;
//}
//------------------------------------------------------------------------------
//bool VerificationParameterSqlQueryModel::setData(const QModelIndex &index,
//        const QVariant &value, int role)
//{
////    if (index.column() == 1) {
////        return false;
////    }

//    return SqlQueryModel::setData(index, value, role);
//}
//------------------------------------------------------------------------------
//QVariant VerificationParameterSqlQueryModel::data(const QModelIndex &item, int role) const
//{/*
//    if (item.column() != 1) {
//        QSqlQueryModel::data(item, role);
//    }*/
////    if (item.column() == 1 && role == Qt::EditRole && item.row() < 3) { //Qt::DisplayRole
////        return data(item, Qt::EditRole);
////        //return QVariant(5);
////             // handle column separately
////    }
////    else if (item.column() == 1 && role == Qt::DisplayRole && item.row() < 3) {
////        qDebug() << "isValid:" << item.isValid();
////        qDebug() << "---";
////        qDebug() << "item.data()" << item.data(Qt::EditRole);
////        if (item.isValid() && item.data() != 0)
////            return item.data();//item.data();
////        return 10;
////    }

////    qDebug() << "column:" << item.column();
////    qDebug() << "row:" << item.row();

//    return SqlQueryModel::data(item, role);
//}
//------------------------------------------------------------------------------
