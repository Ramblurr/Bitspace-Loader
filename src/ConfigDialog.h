#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConfigDialog *ui;

private slots:
    void on_buttonBox_accepted();
    void on_lineEdit_textChanged(QString );
};

#endif // CONFIGDIALOG_H
