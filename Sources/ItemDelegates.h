#ifndef ITEMDELEGATES_H
#define ITEMDELEGATES_H

#include <QItemDelegate>
//------------------------------------------------------------------------------
class NoEditItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit NoEditItemDelegate(QObject *parent = 0);

    virtual QWidget * createEditor(QWidget *, const QStyleOptionViewItem &,
                                   const QModelIndex &) const;
};
//------------------------------------------------------------------------------
#endif // ITEMDELEGATES_H
