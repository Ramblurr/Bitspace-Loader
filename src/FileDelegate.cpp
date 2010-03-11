/****************************************************************************************
 * Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>                             *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 3 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/
#include "FileDelegate.h"

#include "loader_global.h"

#include <QStyleOptionProgressBar>
#include <QApplication>
#include <QDebug>

void FileDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    if ( index.column() != 1 || ( Bitspace::ItemStates ) index.data( Bitspace::State ).toInt() != Bitspace::InProgress )
    {
        QStyledItemDelegate::paint( painter, option, index );
        return;
    }

    QStyleOptionProgressBar progressBarOption;
    progressBarOption.state = QStyle::State_Enabled;
    progressBarOption.direction = QApplication::layoutDirection();
    progressBarOption.rect = option.rect;
    progressBarOption.fontMetrics = QApplication::fontMetrics();
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.textAlignment = Qt::AlignCenter;
    progressBarOption.textVisible = true;

    double progress = index.data( Bitspace::Progress ).toDouble();
    progressBarOption.progress = progress < 0 ? 0 : progress;
    progressBarOption.text = QString().sprintf( "%d%%", progressBarOption.progress );

    // Draw the progress bar onto the view.
    QApplication::style()->drawControl( QStyle::CE_ProgressBar, &progressBarOption, painter );
}

QSize FileDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    return QStyledItemDelegate::sizeHint( option, index );
}
