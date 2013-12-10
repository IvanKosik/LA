#include "ItemDelegates.h"
//------------------------------------------------------------------------------
NoEditItemDelegate::NoEditItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}
//------------------------------------------------------------------------------
QWidget * NoEditItemDelegate::createEditor(QWidget *,
        const QStyleOptionViewItem &, const QModelIndex &) const
{
    return 0;
}
//------------------------------------------------------------------------------
