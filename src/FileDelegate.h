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
#ifndef FILEDELEGATE_H
#define FILEDELEGATE_H

#include <QStyledItemDelegate>

class FileDelegate : public QStyledItemDelegate
{
public:
    FileDelegate( QWidget *parent = 0 ) : QStyledItemDelegate( parent ) {}
    virtual void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    virtual QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const;

private:
    QString bytesTotal( const QModelIndex &index ) const;
    QString bytesRunning( const QModelIndex &index ) const;
    QString speed( const QModelIndex &index ) const;


};

#endif // FILEDELEGATE_H
