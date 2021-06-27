#ifndef oxygenexceptionmodel_h
#define oxygenexceptionmodel_h
//////////////////////////////////////////////////////////////////////////////
// oxygenexceptionmodel.h
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenlistmodel.h"
#include "oxygensettings.h"
#include "oxygen.h"

namespace Oxygen
{

    //* qlistview for object counters
    class ExceptionModel: public ListModel<InternalSettingsPtr>
    {

        public:

        //* number of columns
        enum { nColumns = 3 };

        //* column type enumeration
        enum ColumnType {
            ColumnEnabled,
            ColumnType,
            ColumnRegExp
        };


        //*@name methods reimplemented from base class
        //@{

        //* return data for a given index
        QVariant data(const QModelIndex &index, int role) const override;

        //* header data
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        //* number of columns for a given index
        int columnCount(const QModelIndex& ) const override
        { return nColumns; }

        //@}

        protected:

        //* sort
        void privateSort( int, Qt::SortOrder ) override
        {}

        private:

        //* column titles
        static const QString m_columnTitles[ nColumns ];

    };

}
#endif
