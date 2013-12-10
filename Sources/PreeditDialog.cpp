#include "PreeditDialog.h"
#include "ui_PreeditDialog.h"

#include <QPrinter>
#include <QPrintDialog>
//------------------------------------------------------------------------------
PreeditDialog::PreeditDialog(const QString &text, QWidget *parent)
        : QDialog(parent), ui(new Ui::PreeditDialog)
{
    ui->setupUi(this);

    ui->textEdit->setPlainText(text);
}
//------------------------------------------------------------------------------
PreeditDialog::~PreeditDialog()
{
    delete ui;
}
//------------------------------------------------------------------------------
void PreeditDialog::on_cancelPushButton_clicked()
{
    done(QDialog::Rejected);
}
//------------------------------------------------------------------------------
void PreeditDialog::on_printPushButton_clicked()
{
    QPrinter *printer = new QPrinter;
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        ui->textEdit->print(printer);
        //%done(QDialog::Accepted); //%Не закрываю окно, чтобы можно было что-то подправить и еще раз напечатать.
    }

    delete printer;
}
//------------------------------------------------------------------------------
