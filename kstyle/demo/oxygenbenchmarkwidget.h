#ifndef oxygenbenchmarkwidget_h
#define oxygenbenchmarkwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbenchmarkwidget.h
// oxygen buttons demo widget
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygendemowidget.h"
#include "ui_oxygenbenchmarkwidget.h"
#include "../oxygen.h"

#include <QWidget>
#include <QCheckBox>
#include <QPair>
#include <QVector>
#include <KPageDialog>

namespace Oxygen
{
    class BenchmarkWidget: public DemoWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit BenchmarkWidget( QWidget* = nullptr );

        //! setup widgets
        void init( KPageDialog*, QVector<KPageWidgetItem*> );

        Q_SIGNALS:

        void runBenchmark( void );

        private Q_SLOTS:

        //! button state
        void updateButtonState( void );

        //! grabMouse
        void updateGrabMouse( bool value )
        { Simulator::setGrabMouse( value ); }

        //! run
        void run( void );

        private:

        //! select page from index in parent page widget
        void selectPage( int ) const;

        //! ui
        Ui_BenchmarkWidget ui;

        //! pointer to pagewidget
        WeakPointer<KPageDialog> _pageDialog;

        //! map checkboxes to demo widgets
        using ItemPointer = WeakPointer<KPageWidgetItem>;
        using ItemPair = QPair<QCheckBox*, ItemPointer>;
        using ItemList = QVector<ItemPair>;
        ItemList _items;

    };

}

#endif
