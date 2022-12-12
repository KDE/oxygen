#ifndef ItemModel_h
#define ItemModel_h

//////////////////////////////////////////////////////////////////////////////
// itemmodel.h
// -------------------
//
// SPDX-FileCopyrightText: 2009-2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygen_export.h"

#include <QAbstractItemModel>

namespace Oxygen
{

//* Job model. Stores job information for display in lists
class OXYGEN_EXPORT ItemModel : public QAbstractItemModel
{
public:
    //* constructor
    explicit ItemModel(QObject * = nullptr);

    //* return all indexes in model starting from parent [recursive]
    QModelIndexList indexes(int column = 0, const QModelIndex &parent = QModelIndex()) const;

    //*@name sorting
    //@{

    //* sort
    void sort(void)
    {
        sort(sortColumn(), sortOrder());
    }

    //* sort
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    //* current sorting column
    int sortColumn(void) const
    {
        return _sortColumn;
    }

    //* current sort order
    Qt::SortOrder sortOrder(void) const
    {
        return _sortOrder;
    }

    //@}

protected:
    //* this sort columns without calling the layout changed callbacks
    void privateSort(void)
    {
        privateSort(sortColumn(), sortOrder());
    }

    //* private sort, with no signals emmitted
    virtual void privateSort(int column, Qt::SortOrder order) = 0;

    //* used to sort items in list
    class SortFTor
    {
    public:
        //* constructor
        explicit SortFTor(const int &type, Qt::SortOrder order = Qt::AscendingOrder)
            : _type(type)
            , _order(order)
        {
        }

    protected:
        //* column
        int _type = 0;

        //* order
        Qt::SortOrder _order = Qt::AscendingOrder;
    };

private:
    //* sorting column
    int _sortColumn = 0;

    //* sorting order
    Qt::SortOrder _sortOrder = Qt::AscendingOrder;
};
}

#endif
