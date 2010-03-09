#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace bitspace {
    class Upload;
}

class FileModel;
class QNetworkReply;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void slotAddFiles();
    void slotAddFolders();
    void slotUpload();
    void slotUploadProgress( qint64, qint64 );
private:
    void setupActions();
    QStringList nameFilters() const;
    Ui::MainWindow *ui;

    QAction* m_addFiles;
    QAction* m_addFolders;
    QAction* m_upload;

    FileModel* m_model;

    bitspace::Upload* m_uploader;
};

#endif // MAINWINDOW_H
