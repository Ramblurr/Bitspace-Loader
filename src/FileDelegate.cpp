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
#include <QPainter>
#include <QPalette>
#include <QSize>
#include <QRect>

void FileDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    if ( index.column() == 0 )
    {
        QStyledItemDelegate::paint( painter, option, index );
        return;
    }

    Bitspace::ItemStates state = (Bitspace::ItemStates) index.data( Bitspace::StateRole ).toInt();

    if( index.column() == 1 ) // "Transfer"
    {
        QString title;
        QString greyed;
        if( state == Bitspace::InProgress )
        {
            title = speed( index );
            greyed = bytesRunning( index );
        }
        else if( state == Bitspace::Pending )
        {
            title = tr( "Pending" );
            greyed = bytesTotal( index );
        }
        else if( state == Bitspace::Complete )
        {
            title = tr( "Complete" );
            greyed = bytesTotal( index );
        }
        else if( state == Bitspace::Errored )
        {
            title = tr( "Error" );
            greyed = bytesTotal( index );
        }

        QRect topRect( option.rect );
        topRect.setHeight( option.rect.height() / 2 );

        QRect bottomRect( topRect );
        bottomRect.setTop( topRect.bottom() );

        painter->save();

        QFont font = option.font;
        font.setBold( true );
        painter->setFont( font );
        painter->drawText( option.rect , Qt::AlignHCenter | Qt::AlignTop, title );

        painter->restore();
        painter->save();
        painter->setPen( QApplication::palette().color( QPalette::Disabled, QPalette::Text ) );

        painter->drawText( option.rect , Qt::AlignHCenter | Qt::AlignBottom, greyed );
        painter->restore();
        return;
    }

    if( index.column() == 2 )
    {
            QStyleOptionProgressBar progressBarOption;
            progressBarOption.state = QStyle::State_Enabled;
            progressBarOption.direction = QApplication::layoutDirection();
            progressBarOption.rect = option.rect;
            progressBarOption.fontMetrics = QApplication::fontMetrics();
            progressBarOption.minimum = 0;
            progressBarOption.maximum = 100;
            progressBarOption.textAlignment = Qt::AlignCenter;
            progressBarOption.textVisible = true;

            double progress = index.data( Bitspace::ProgressRole ).toDouble();
            progressBarOption.progress = progress < 0 ? 0 : progress;
            progressBarOption.text = QString().sprintf( "%d%%", progressBarOption.progress );
            progressBarOption.rect.setY(progressBarOption.rect.y() +
                                        (option.rect.height() - QApplication::fontMetrics().height()) / 2);
            progressBarOption.rect.setHeight(QApplication::fontMetrics().height());

            // Draw the progress bar onto the view.
            QApplication::style()->drawControl( QStyle::CE_ProgressBar, &progressBarOption, painter );
    }
}

QSize FileDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    if ( index.column() == 0 )
    {
        return QStyledItemDelegate::sizeHint( option, index );
    }
    Bitspace::ItemStates state = (Bitspace::ItemStates) index.data( Bitspace::StateRole ).toInt();
    if( index.column() == 1 ) // "Transfer"
    {
        QString title;
        QString greyed;
        if( state == Bitspace::InProgress )
        {
            title = speed( index );
            greyed = bytesRunning( index );
        }
        else if( state == Bitspace::Pending )
        {
            title = tr( "Pending" );
            greyed = bytesTotal( index );
        }
        else if( state == Bitspace::Complete )
        {
            title = tr( "Complete" );
            greyed = bytesTotal( index );
        }
        else if( state == Bitspace::Errored )
        {
            title = tr( "Error" );
            greyed = bytesTotal( index );
        }
        QFont font = option.font;
        font.setBold( true );
        QFontMetrics fmTitle( font );
        int w = qMax( fmTitle.width( title ), option.fontMetrics.width( greyed ) );
        int h = fmTitle.height() + option.fontMetrics.height() + 5;
        qDebug() << "fmTitle.height() + option.fontMetrics.height();" << fmTitle.height() << "+" << option.fontMetrics.height() << "=" <<h;
        return QSize(w, h);
    }
    return QStyledItemDelegate::sizeHint( option, index );
}

QString FileDelegate::bytesTotal( const QModelIndex &index ) const
{
    int bytesTotal = index.data(Bitspace::TotalBytesRole).toInt();
    double megabytesTotal = bytesTotal / (1000.0*1000.0);
    return QString( "%1 MB" ).arg(megabytesTotal, 0, 'f', 2);
}

QString FileDelegate::bytesRunning( const QModelIndex &index ) const
{
    int bytesProcessed = index.data(Bitspace::ProcessedBytesRole).toInt();
    int bytestotal = index.data(Bitspace::TotalBytesRole).toInt();
    double megabytesProcessed = bytesProcessed / (1000.0*1000.0);
    double megabytesTotal = bytestotal / (1000.0*1000.0);

    return QString( "%1 MB of %2 MB" ).arg(megabytesProcessed, 0, 'f', 2).arg(megabytesTotal, 0, 'f', 2);
}

QString FileDelegate::speed(const QModelIndex &index) const
{
    double speed = index.data(Bitspace::SpeedRole).toInt() / 1000;
    return QString( "%1 KB/s" ).arg( speed, 0, 'f', 0 );
}
