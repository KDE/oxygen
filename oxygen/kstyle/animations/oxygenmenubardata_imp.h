#ifndef oxygenmenubardata_imp_h
#define oxygenmenubardata_imp_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmenubardata_imp.h
// implements menubar data templatized methods
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

namespace Oxygen
{


    //________________________________________________________________________
    template< typename T > void MenuBarDataV1::enterEvent( const QObject* object )
    {

        const T* local = qobject_cast<const T*>( object );
        if( !local ) return;

        // if the current action is still active, one does nothing
        if( local->activeAction() == currentAction() ) return;

        if( currentTimeLine()->isRunning() ) currentTimeLine()->stop();
        clearCurrentAction();
        clearCurrentRect();

        return;

    }

    //________________________________________________________________________
    template< typename T > void MenuBarDataV1::leaveEvent( const QObject* object )
    {

        const T* local = qobject_cast<const T*>( object );
        if( !local ) return;

        // if the current action is still active, one does nothing
        if( local->activeAction() == currentAction() ) return;

        if( currentTimeLine()->isRunning() ) currentTimeLine()->stop();
        if( previousTimeLine()->isRunning() ) previousTimeLine()->stop();
        if( currentAction() )
        {
            setPreviousRect( currentRect() );
            clearCurrentAction();
            clearCurrentRect();
            previousTimeLine()->start();
        }

        return;
    }

    //________________________________________________________________________
    template< typename T > void MenuBarDataV1::mouseMoveEvent( const QObject* object )
    {

        const T* local = qobject_cast<const T*>( object );
        if( !local ) return;

        // check action
        if( local->activeAction() == currentAction() ) return;

        // check current action
        if( currentAction() )
        {
            if( currentTimeLine()->isRunning() ) currentTimeLine()->stop();
            if( previousTimeLine()->isRunning() ) previousTimeLine()->stop();
            setPreviousRect( currentRect() );
            previousTimeLine()->start();
            clearCurrentAction();
            clearCurrentRect();
        }

        // check if local current actions is valid
        if( local->activeAction() && local->activeAction()->isEnabled() && !local->activeAction()->isSeparator())
        {
            if( currentTimeLine()->isRunning() ) currentTimeLine()->stop();
            setCurrentAction( local->activeAction() );
            setCurrentRect( local->actionGeometry( currentAction() ) );
            currentTimeLine()->start();
        }

    }

}

#endif
