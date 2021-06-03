//////////////////////////////////////////////////////////////////////////////
// oxygenprogressbar.cpp
// data container for progressbar animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenprogressbardata.h"

#include <QProgressBar>

namespace Oxygen
{

    //______________________________________________
    ProgressBarData::ProgressBarData( QObject* parent, QWidget* target, int duration ):
        GenericData( parent, target, duration ),
        _startValue(0),
        _endValue(0)
    {

        target->installEventFilter( this );

        // set animation curve shape
        animation().data()->setEasingCurve( QEasingCurve::InOutQuad );

        // make sure target is a progressbar and store relevant values
        QProgressBar* progress = qobject_cast<QProgressBar*>( target );
        Q_CHECK_PTR( progress );
        _startValue = progress->value();
        _endValue = progress->value();

        // setup connections
        connect( target, SIGNAL(valueChanged(int)), SLOT(valueChanged(int)) );

    }

    //______________________________________________
    bool ProgressBarData::eventFilter( QObject* object, QEvent* event )
    {

        if( !( enabled() && object && object == target().data() ) ) return AnimationData::eventFilter( object, event );
        switch( event->type() )
        {
            case QEvent::Show:
            {

                // reset start and end value
                QProgressBar* progress = static_cast<QProgressBar*>( target().data() );
                _startValue = progress->value();
                _endValue = progress->value();
                break;

            }

            case QEvent::Hide:
            {
                if( animation().data()->isRunning() )
                { animation().data()->stop(); }
                break;
            }

            default: break;

        }

        return AnimationData::eventFilter( object, event );

    }

    //______________________________________________
    void ProgressBarData::valueChanged( int value )
    {

        // do nothing if not enabled
        if( !enabled() ) return;

        // do nothing if progress is invalid
        QProgressBar* progress = static_cast<QProgressBar*>( target().data() );
        if( !( progress && progress->maximum() != progress->minimum() ) ) return;

        // update start and end values
        bool isRunning( animation().data()->isRunning() );
        if( isRunning )
        {

            // in case next value arrives while animation is running,
            // end animation, set value immediately
            // and trigger target update. This increases responsiveness of progressbars
            _startValue = value;
            _endValue = value;
            animation().data()->stop();
            setOpacity(0);

            if( target() ) target().data()->update();

            return;

        }

        _startValue = _endValue;
        _endValue = value;

        // start animation only if target is enabled, visible, not running,
        // and if end and start values are different enough
        // (with end value being larger than start value)
        if( !(target() && target().data()->isEnabled() && target().data()->isVisible()) ) return;
        if( isRunning || _endValue-_startValue < 2 ) return;

        animation().data()->start();

    }

}
