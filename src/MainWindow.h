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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTime>

namespace Ui
{
class MainWindow;
}

class TransferItem;
class FileModel;
class QNetworkReply;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

signals:
    void abort();

protected:
    void changeEvent( QEvent *e );
    void closeEvent( QCloseEvent *event );

private slots:
    void on_actionOptions_triggered();
    void on_actionQuit_triggered();
    void on_m_uploadAction_triggered();
    void slotAddFiles();
    void slotAddFolders();
    void slotUploadError( QString );
    void slotUploadProgress( qint64 , qint64 );
    void slotOptionsChanged();
    void slotAbortUpload();
    void slotIconActivated( QSystemTrayIcon::ActivationReason );
    void slotItemsChanged();
private:
    void setupActions();

    /**
      * Must be called after setupActions
      */
    void createTrayIcon();
    void addFile( const QString & file );
    void setUploadIcon();
    QStringList nameFilters() const;

    Ui::MainWindow *ui;

    QAction* m_addFiles;
    QAction* m_addFolders;
    QAction* m_uploadAction;
    QAction* m_quitAction;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;

    FileModel* m_model;
};

#endif // MAINWINDOW_H
