#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SqlTableModel.h"
#include "PatientDialogs.h"
#include "VerificationDialog.h"
#include "PrognosisDialog.h"
#include "ViewVerificationDialog.h"
#include "ViewPrognosisDialog.h"
#include "PreeditDialog.h"
#include "SqlQuery.h"

#include <QtGui/QMessageBox>
#include <QSqlError>
#include <QPrinter>
#include <QPrintDialog>

#include <algorithm>

#include <QtCore/QDebug> //-
//------------------------------------------------------------------------------
// ReadableExpertise:
// Readable field names:
const char *ReadableExpertise::Verification = QT_TRANSLATE_NOOP("ColumnNames", "Verification");
const char *ReadableExpertise::Prognosis = QT_TRANSLATE_NOOP("ColumnNames", "Prognosis");
//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createModels();
    adjustTables();
}
//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}
//------------------------------------------------------------------------------
void MainWindow::createModels()
{
    mPatientModel = new PatientSqlTableModel();

    //-Add other models...
}
//------------------------------------------------------------------------------
void MainWindow::adjustTables()
{
    ui->patientTableView->adjust(mPatientModel);
    connect(ui->patientTableView, SIGNAL(currentPatientChanged()), SLOT(patientChanged()));


    // Настраиваем таблицу экспертиз:
    QStringList headerLabels;
    headerLabels << QCoreApplication::translate("ColumnNames", ReadableExpertise::Verification)
                 << QCoreApplication::translate("ColumnNames", ReadableExpertise::Prognosis);
    ui->expertiseTableWidget->setHorizontalHeaderLabels(headerLabels);
    ui->expertiseTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // � астягиваем колонки на всю ширину. Плохо то, что после этого пользователю
    // нельзя самому менять размер конкретной колонки.
    ui->expertiseTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);


    //-Add other tableViews...
}
//------------------------------------------------------------------------------
void MainWindow::on_addPatientPushButton_clicked()
{
    AddPatientDialog *addPatientDialog = new AddPatientDialog(mPatientModel, this); //-! Очистить память!
    qDebug() << "dialog code: " << addPatientDialog->exec();
    //-addPatientDialog->show(); //-Если нужна возможность откывать много окон. Тогда еще убрать setModal и нужно хранить его, как поле (а то диалог потеряется при выходе из этого метода)

    //-mPatientModel->submitAll();//-
    //-mPatientModel->submit();
    qDebug() << "Patient addition is finished."; //-
}
//------------------------------------------------------------------------------
void MainWindow::on_editPatientPushButton_clicked()
{
    // Если выделен хотя бы один пациент:
    if (!ui->patientTableView->selectionModel()->selectedIndexes().isEmpty()) {
        EditPatientDialog *editPatientDialog = new EditPatientDialog(mPatientModel,
                ui->patientTableView->currentIndex().row(), this); //-! Очистить память!
        qDebug() << "curRow:" << ui->patientTableView->currentIndex().row(); //-
        editPatientDialog->exec();
    }
    else {
        QMessageBox::information(this, tr("Edit Patient"),
                                 tr("Select at least one patient record."));
    }
}
//------------------------------------------------------------------------------
void MainWindow::on_deletePatientPushButton_clicked()
{
    // Если выделен хотя бы один пациент:
    if (!ui->patientTableView->selectionModel()->selectedIndexes().isEmpty()) {
        qint32 returnValue = QMessageBox::question(this, tr("Delete Patient?"),
                tr("Are you sure you want to delete the selected patient records?"),
                QMessageBox::Ok, QMessageBox::Cancel);

        if (returnValue == QMessageBox::Ok) {
            foreach (QModelIndex modelIndex,
                     ui->patientTableView->selectionModel()->selectedIndexes()) {
                mPatientModel->removeRow(modelIndex.row());
            }
//            QModelIndexList indexes = ui->patientTableView->selectionModel()->selectedIndexes();
//            // Список не отсортирован. Поэтому ищем минимальный индекс ряда:
//            qint32 minIndex = indexes.first().row();
//            foreach (QModelIndex index, indexes) {
//                if (index.row() < minIndex) {
//                    minIndex = index.row();
//                }
//            }
//            // Удаляем выделенные записи:
//            mPatientModel->removeRows(minIndex, indexes.size());

            if (!mPatientModel->submitAll()) {
                QSqlError lastError = mPatientModel->lastError();

                QMessageBox::warning(this, tr("Error"), tr("Cannot delete "
                        "the selected records from the model!")
                        + tr("\nError code: ") + QString::number(lastError.number())
                        + tr("\nError text: ") + lastError.text());
            }
        }
    }
    else {
        QMessageBox::information(this, tr("Delete Patient"),
                tr("Select at least one patient record."));
    }
}
//------------------------------------------------------------------------------
void MainWindow::on_verificationPushButton_clicked()
{
    // Если выделен хотя бы один пациент:
    if (!ui->patientTableView->selectionModel()->selectedIndexes().isEmpty()) {
        qint32 row = ui->patientTableView->currentIndex().row();
        qint32 column = mPatientModel->fieldIndex(Patient::Id);
        quint64 patientId = mPatientModel->data(mPatientModel->index(row, column)).toULongLong();

        VerificationDialog *verificationDialog = new VerificationDialog(patientId, this); //-! Очистить память!
        if (verificationDialog->exec() == QDialog::Accepted) {
            patientChanged(); // Чтобы обновилась таблица экспертиз.
        }
    }
    else {
        QMessageBox::information(this, tr("Verification"),
                                 tr("Select at least one patient record."));
    }
}
//------------------------------------------------------------------------------
void MainWindow::on_prognosisPushButton_clicked()
{
    // Если выделен хотя бы один пациент:
    if (!ui->patientTableView->selectionModel()->selectedIndexes().isEmpty()) {
        qint32 row = ui->patientTableView->currentIndex().row();
        qint32 column = mPatientModel->fieldIndex(Patient::Id);
        quint64 patientId = mPatientModel->data(mPatientModel->index(row, column)).toULongLong();

        PrognosisDialog *prognosisDialog = new PrognosisDialog(patientId, this); //-! Очистить память!
        if (prognosisDialog->exec() == QDialog::Accepted) {
            patientChanged(); // Чтобы обновилась таблица экспертиз.
        }
    }
    else {
        QMessageBox::information(this, tr("Prognosis"),
                                 tr("Select at least one patient record."));
    }
}
//------------------------------------------------------------------------------
void MainWindow::on_viewExpertisePushButton_clicked()
{
    // Если выделена хотя бы одна экспертиза:
    if (!ui->expertiseTableWidget->selectionModel()->selectedIndexes().isEmpty()) {
        quint32 row = ui->expertiseTableWidget->currentIndex().row();
        Id expertiseId = mExpertiseIds.at(row);

        // Если ячейка в первом столбце создана, то в данном ряде записана верификация:
        if (ui->expertiseTableWidget->item(row, 0) != 0) {
            ViewVerificationDialog *viewVerificationDialog
                    = new ViewVerificationDialog(expertiseId, this); //-! Очистить память!
            viewVerificationDialog->exec();
        }
        // Иначе в выделенном ряде запись о прогнозе:
        else {
            ViewPrognosisDialog *viewPrognosisDialog
                    = new ViewPrognosisDialog(expertiseId, this); //-! Очистить память!
            viewPrognosisDialog->exec();
        }
    }
    else {
        QMessageBox::information(this, tr("View Expertise"),
                                 tr("Select at least one expertise record."));
    }
}
//------------------------------------------------------------------------------
void MainWindow::on_deleteExpertisePushButton_clicked()
{
    // Если выделена хотя бы одна экспертиза:
    if (!ui->expertiseTableWidget->selectionModel()->selectedIndexes().isEmpty()) {
        qint32 returnValue = QMessageBox::question(this, tr("Delete Expertise?"),
                tr("Are you sure you want to delete the selected expertise records?"),
                QMessageBox::Ok, QMessageBox::Cancel);

        if (returnValue == QMessageBox::Ok) {
            QList<quint32> deletedRows; // Список рядов, которые необходимо удалить.

            foreach (QModelIndex modelIndex,
                     ui->expertiseTableWidget->selectionModel()->selectedRows()) {
                quint32 row = modelIndex.row();
                // Добавляем номер ряда в список, чтобы потом удалить эти ряды из списка экспертиз:
                deletedRows.append(row);
            }

            // Сортируем список по возрастанию:
            qSort(deletedRows);

            // Удаляем ряды по убыванию:
            for (qint32 i = deletedRows.size() - 1; i >= 0; --i) {
                // Находим идентификатор экспертизы:
                quint32 row = deletedRows.at(i);
                Id expertiseId = mExpertiseIds.at(row);


                // Если ячейка в первом столбце создана, то в данном ряде записана верификация:
                if (ui->expertiseTableWidget->item(row, 0) != 0) {
                    // Удаляем запись верификации из БД:
                    SqlQuery verificationQuery("DELETE FROM Verification WHERE ID=?");
                    verificationQuery.addBindValue(expertiseId);
                    verificationQuery.exec();
                }
                // Иначе в выделенном ряде запись о прогнозе:
                else {
                    // Удаляем запись прогноза из БД:
                    SqlQuery prognosisQuery("DELETE FROM Prognosis WHERE ID=?");
                    prognosisQuery.addBindValue(expertiseId);
                    prognosisQuery.exec();
                }


                // Удаляем идентификатор из списка:
                mExpertiseIds.removeAt(row);

                // Удаляем ряд из таблицы:
                ui->expertiseTableWidget->removeRow(row);
            }
        }
    }
    else {
        QMessageBox::information(this, tr("Delete Expertise"),
                tr("Select at least one expertise record."));
    }
}
//------------------------------------------------------------------------------
void MainWindow::patientChanged()
{
    qDebug() << "patientChanged" << ui->patientTableView->currentIndex();//-

    // Очистим таблицу экспертиз и поле заключения:
    ui->expertiseTableWidget->clearContents();
    ui->expertiseTableWidget->setRowCount(0);
    ui->conclusionTextEdit->clear();
    // Очистим список идентификаторов экспертиз:
    mExpertiseIds.clear();

    qint32 row = ui->patientTableView->currentIndex().row();
    // Если не выделен ни один пациент:
    if (row == -1) {
        // Очистим таблицу экспертиз и поле заключения:
//        ui->expertiseTableWidget->clearContents();
//        ui->conclusionTextEdit->clear();

        return;
    }

    qint32 column = mPatientModel->fieldIndex(Patient::Id);
    Id patientId = mPatientModel->data(mPatientModel->index(row, column)).toULongLong();

    qDebug() << "Id" << patientId;//-


    QSet<QPair<QDateTime, Id> > verifications = DB::getPatientVerifications(patientId);
    QSet<QPair<QDateTime, Id> > prognoses = DB::getPatientPrognoses(patientId);

    quint32 curRow = 0;


    QSet<QPair<QDateTime, Id> >::iterator minVerification;
    QSet<QPair<QDateTime, Id> >::iterator minPrognosis;
    // Если оба множества не пусты:
//    if (!verifications.isEmpty() && !prognoses.isEmpty()) {
        minVerification = std::min_element(verifications.begin(),
                verifications.end(), hasFirstPairLowerDate);
        if (minVerification == verifications.end()) {
            qDebug() << "it:" << "0";
        }
        minPrognosis = std::min_element(prognoses.begin(),
                prognoses.end(), hasFirstPairLowerDate);
//    }


    // Пока хотя бы одно множество не пустое (пустое, если итератор указывает на элемент, следующий
        // за последним):
    while ((minVerification != verifications.end())
           || (minPrognosis != prognoses.end())) {
        // Ищем пару с минимальной датой в каждом из множеств:
//        QPair<QDateTime, Id> *minVerification = findPairWithMinDate(verifications);
//        QPair<QDateTime, Id> *minPrognosis = findPairWithMinDate(prognoses);


//        QSet<QPair<QDateTime, Id> >::iterator minPrognosis = std::min_element(prognoses.begin(),
//                prognoses.end(), hasFirstPairLowerDate);

        ui->expertiseTableWidget->setRowCount(curRow + 1);

        // Среди двух минимальных ищем меньшую:
//        QPair<QDateTime, Id> minExpertise;
        if ((minVerification != verifications.end())
                && (((minPrognosis != prognoses.end())
                && ((*minVerification).first <= (*minPrognosis).first))
                || (minPrognosis == prognoses.end()))) {
            // Добавляем дату в таблицу экспертиз в колонку верификаций:
            QTableWidgetItem *verificationDateItem
                    = new QTableWidgetItem((*minVerification).first.toString());
            ui->expertiseTableWidget->setItem(curRow++, 0, verificationDateItem);
            // Добавляем идентификатор экспертизы в список:
            mExpertiseIds.append((*minVerification).second);
//            minExpertise = *minVerification;
            // Удаляем пару из множества:
            verifications.erase(minVerification);
            // Находим новую пару с минимальной датой:
            minVerification = std::min_element(verifications.begin(),
                    verifications.end(), hasFirstPairLowerDate);
        }
        else {
            // Добавляем дату в таблицу экспертиз в колонку прогнозов:
            QTableWidgetItem *prognosisDateItem
                    = new QTableWidgetItem((*minPrognosis).first.toString());
            ui->expertiseTableWidget->setItem(curRow++, 1, prognosisDateItem);
            // Добавляем идентификатор экспертизы в список:
            mExpertiseIds.append((*minPrognosis).second);
//            minExpertise = *minVerification;
//            minExpertise = *minPrognosis;
            // Удаляем пару из множества:
            prognoses.erase(minPrognosis);
            // Находим новую пару с минимальной датой:
            minPrognosis = std::min_element(prognoses.begin(),
                    prognoses.end(), hasFirstPairLowerDate);
        }
    }

    // Добавляем остальные экспертизы, которые возможно остались в одном из множеств:
//    if

}
//------------------------------------------------------------------------------
void MainWindow::on_preeditPushButton_clicked()
{
    PreeditDialog *preeditDialog
            = new PreeditDialog(ui->conclusionTextEdit->toPlainText(), this); //-% Очистить память!
    preeditDialog->exec();
}
//------------------------------------------------------------------------------
void MainWindow::on_printPushButton_clicked()
{
    QPrinter *printer = new QPrinter();
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        ui->conclusionTextEdit->print(printer);//%  - Тоже классный вариант! Сразу содержимое на печать отправляет.

        /*%
        QTextDocument *t = ui->conclusionTextEdit->document(); // Этот способ получает документ
        t->print(); потом отправляет его на печать.
        %*/

        /*% Делаем такой вариант:
          (Перед этим проверить с priPrinter и настоящим принтером, можно ли прямо из QtextEdit печатать!!!)
          В окне заключения нужно запретить редактировать
          Будет 2 кнопки: первая, значить, сразу печать (нужно подредактировать только текст
               добавить имя пациента и все такое.
            вторая, значить, будет предпросмотр с возможностью редактирования.
              (там сразу в QtextEdit будет имя добавлено и все такое)
               можно будет что-то дописать - и нажать печать. Все.
               Доступно?
        %*/

//        QTextDocument *textDocument = new QTextDocument;
//        textDocument->setPlainText("Yes!!! This is work with English text!");
//        textDocument->print(printer);

        qDebug() << "Was printed!";
        // print ...
    }

    delete printer;

//    mPatientModel->submitAll(); //-

    qDebug() << ui->patientTableView->currentIndex(); //-
}
//------------------------------------------------------------------------------
bool hasFirstPairLowerDate(QPair<QDateTime, Id> firstPair, QPair<QDateTime, Id> secondPair)
{
    return firstPair.first < secondPair.first;
}
//------------------------------------------------------------------------------
void MainWindow::on_expertiseTableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    //%! Тут она сюда заходит когда очищается вообще таблица экспертиз.. и всё падает тогда. Или как-то так...
    if (currentRow >= 0 && currentRow < mExpertiseIds.size()) {
        Id expertiseId = mExpertiseIds.at(currentRow);
        // Если ячейка в первом столбце создана, то в данном ряде записана верификация:
        if (ui->expertiseTableWidget->item(currentRow, 0) != 0) {
            QString conclusion = DB::getVerificationConclusion(expertiseId);
            ui->conclusionTextEdit->setPlainText(conclusion);
        }
    }
}
//------------------------------------------------------------------------------
