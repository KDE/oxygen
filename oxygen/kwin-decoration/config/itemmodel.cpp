//////////////////////////////////////////////////////////////////////////////
// itemmodel.cpp
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

/*!
  \file    ItemModel.h
  \brief   Job model. Stores job information for display in lists
  \author  Hugo Pereira
  \version $Revision: 1.1 $
  \date    $Date: 2009/03/25 17:44:24 $
*/

#include "itemmodel.h"

using namespace std;

namespace Oxygen
{
  
  //_______________________________________________________________
  ItemModel::ItemModel( QObject* parent ):
    QAbstractItemModel( parent ),
    sort_column_(0),
    sort_order_( Qt::AscendingOrder )
  {}
  
  //____________________________________________________________
  void ItemModel::sort( int column, Qt::SortOrder order )
  {
    
    // store column and order
    sort_column_ = column;
    sort_order_ = order;
    
    // emit signals and call private methods
    emit layoutAboutToBeChanged();
    _sort( column, order );
    emit layoutChanged();
    
  }
  
  //____________________________________________________________
  QModelIndexList ItemModel::indexes( int column, const QModelIndex& parent ) const
  {
    QModelIndexList out;
    int rows( rowCount( parent ) );
    for( int row = 0; row < rows; row++ )
    {
      QModelIndex index( this->index( row, column, parent ) );
      if( !index.isValid() ) continue;
      out.push_back( index );
      out += indexes( column, index );
    }
    
    return out;
    
  }
  
}
