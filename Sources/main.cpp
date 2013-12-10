#include "MainWindow.h"
#include "DB.h"

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QFile>
#include <QTranslator>

#include <QSettings>

QTranslator * loadTranslationFile(const char *fileName);
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);


    // Загружаем русский перевод:
    loadTranslationFile("LA_ru.qm");
    loadTranslationFile("designer_ru.qm");
    loadTranslationFile("qtbase_ru.qm");
//    QTranslator translator;
//    qDebug() << "dirPath:" << QApplication::applicationDirPath();
//    if (QFile::exists("LaboratoryAlgorithm_ru.qm")) {
//    } else {
//        qWarning() << "No Ru-file!";
//    }

//    if (translator.load("LaboratoryAlgorithm_ru.qm")) {
//        application.installTranslator(&translator);
//        qDebug() << "Ru-file was loaded.";
//    } else {
//        qDebug() << "Can not load Ru-file!";
//    }

    //--------------------------------------------------------------------------
    // Database initialization:
    //-Лучше один раз получить Database::getInstance(), присвоить временной переменной и пользоваться.
    //-res_ExecScript
    /*bool res_ExecScript = Database::getInstance()->executeScriptFromFile("localhost",
            "LaboratoryAlgorithmDatabaseSchema", "root", "root", "3306",
            "G:/Users/IvanKosik/Projects/Qt/Laboratory Algorithm/trunk/Database/LaboratoryAlgorithmDatabaseInitializationScript.sql"); //-$Обеспечить работу с относительным путем!

    if(!res_ExecScript) {
        QMessageBox::critical(0, QObject::tr("Error"),
                Database::getInstance()->getLastErrorText() + "\n"
                        + QObject::tr("Code: ")
                        + QString::number(Database::getInstance()->getLastErrorCode()));

        return 1; //-
    }*/

    //--------------------------------------------------------------------------


    // Open database connection:
    DB *database = DB::getInstance();
    quint32 returnValue = database->openConnection("localhost",
            "LaboratoryAlgorithmDatabaseSchema", "root", "root", "3306");


    // If a database connection error:
    if (returnValue) {
        QMessageBox::critical(0, QObject::tr("Connection Error"),
                       database->getLastErrorText() + "\n" +
                       QObject::tr("Code: ") +
                       QString::number(database->getLastErrorCode()),
                       QMessageBox::Ok);
        return 1;
    }



    //-Test data addition:


    //INSERT INTO `laboratoryalgorithmdbschema`.`patient` (`ID`, `Name`) VALUES (3, 'Nick');
  /*-  QSqlQuery q;
    if (!q.exec(QLatin1String("create table books(id integer primary key, title varchar, author integer, genre integer, year integer, rating integer)"))) {
        QMessageBox::critical(0, QObject::tr("Data Addition Error"),
                       q.lastError());
    }

    -*/

   /*- QSqlQuery q;
    if (!q.prepare(QLatin1String("insert into patient(id, name) values(?, ?)"))) {
        QMessageBox::critical(0, QObject::tr("Data Addition Error"),
                       q.lastError().text());
    }

    q.addBindValue(3);
    q.addBindValue(QString("Nickelodian"));
    q.exec();
    QMessageBox::information(0, QObject::tr("Data Addition Status"),
                   q.lastError().text());
-*/

 /*-   QSqlQuery q;
    if (!q.prepare("insert into patient values(:ID, :Name);")) {
            QMessageBox::critical(0, QObject::tr("Data Addition Error"),
                           q.lastError().text());
        }

    qDebug()<<QVariant(QVariant::Int);
        q.addBindValue(8);
        q.addBindValue(QString("h"));
    QString str = "INSERT INTO patient VALUES(%1, '%2');";
    qDebug()<<q.exec(/*str.arg(QVariant(QVariant::Int).toString()).arg("Jimmy")*/    //);
/*-        QMessageBox::information(0, QObject::tr("Data Addition Status"),
                       q.lastError().text());
-*/



   /*- QVariant asimovId = addPatient(q, QLatin1String("Isaac Asimov"), QDate(1920, 2, 1));
    QVariant greeneId = addPatient(q, QLatin1String("Graham Greene"), QDate(1904, 10, 2));
    QVariant pratchettId = addPatient(q, QLatin1String("Terry Pratchett"), QDate(1948, 4, 28));



    QVariant addAuthor(QSqlQuery &q, const QString &name, const QDate &birthdate)
    {
        q.addBindValue(name);
        q.addBindValue(birthdate);
        q.exec();
        return q.lastInsertId();
    }-*/




    MainWindow mainWindow;
    mainWindow.show();
    
    return application.exec();
}
//------------------------------------------------------------------------------
QTranslator *loadTranslationFile(const char *fileName)
{
    QTranslator *translator = new QTranslator; //% необходимо выделить память, иначе после выхода из функции транслятор удалится.
    if (!QFile::exists(fileName)) {
        qWarning() << QString("No %1 Ru-file!").arg(fileName);
        return 0;
    }

    if (translator->load(fileName)) {
        QCoreApplication::installTranslator(translator);
        qDebug() << QString("%1 Ru-file was loaded.").arg(fileName);
    } else {
        qDebug() << QString("Can not load %1 Ru-file!").arg(fileName);
        return 0;
    }

    return translator;
}
//------------------------------------------------------------------------------
