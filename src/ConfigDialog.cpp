#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"

#include <QSettings>
#include <QPushButton>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    QSettings settings;
    QString token = settings.value( "apitoken", ui->lineEdit->text() ).toString();
    ui->lineEdit->setText( token );
    on_lineEdit_textChanged(token);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ConfigDialog::on_lineEdit_textChanged(QString token)
{
    Q_UNUSED(token)
    if( ui->lineEdit->text().trimmed().isEmpty() )
        ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    else
        ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( true );

}

void ConfigDialog::on_buttonBox_accepted()
{
    QSettings settings;
    settings.setValue( "apitoken", ui->lineEdit->text().trimmed() );
    settings.sync();
}
