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
#ifndef TRANSFER_H
#define TRANSFER_H

#include "loader_global.h"

#include "upload/Upload.h" //libbitspace

#include <QObject>
#include <QTime>
#include <QFileInfo>
#include <QPointer>
#include <QNetworkReply>
#include <QList>
#include <QPointer>

class TransferItem;
typedef QList<TransferItem*> TransferItemList;
class TransferItem : public QObject
{
Q_OBJECT
public:
    explicit TransferItem( QObject *parent = 0 );
    explicit TransferItem( const QString &file, QObject *parent = 0);
    ~TransferItem();

    void start();

    /**
      * Set the file name for this transferitem.
      * Implies a call to reset() after changing the file.
      */
    void setFile( const QString &file );

    /**
      * Keep the same file, but reset state to Pending
      * and clear any progress information
      */
    void reset();

    QString filePath() const;
    Bitspace::ItemStates status() const;
    qint64 bytesTotal() const;
    qint64 bytesProcessed() const;
    int elapsedTime() const;
    int currentSpeed() const;
    int averageTransferSpeed() const;
    int remainingTime() const;
    double percentProgressed() const;

signals:
    void jobFinished( TransferItem* );

public slots:
    void slotUploadProgress( qint64, qint64 );
    void slotUploadFinished();
    void slotUploadError( QString );
    void slotAbortJob();

private:

    QFileInfo m_file;
    QTime m_runningTime;
    int m_runningSeconds;
    Bitspace::ItemStates m_state;
    qint64 m_uploadedSize;
    bool m_errored;
    bool m_aborted;

    QPointer<bitspace::Upload> m_reply;

};

#endif // TRANSFER_H
