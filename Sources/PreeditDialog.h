#ifndef PREEDITDIALOG_H
#define PREEDITDIALOG_H

#include <QDialog>

namespace Ui {
    class PreeditDialog;
}
//------------------------------------------------------------------------------
class PreeditDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PreeditDialog(const QString &text, QWidget *parent = 0);
    ~PreeditDialog();
    
private slots:
    void on_cancelPushButton_clicked();

    void on_printPushButton_clicked();

private:
    Ui::PreeditDialog *ui;
};
//------------------------------------------------------------------------------
#endif // PREEDITDIALOG_H
