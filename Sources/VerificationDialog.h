#ifndef VerificationDialog_h
#define VerificationDialog_h

#include <QtGui/QDialog>

#include <QtSql/QSqlRelationalDelegate>
#include <QtGui/QDataWidgetMapper>

#include "DB.h"

class QAbstractButton;
class QTableWidgetItem;
//------------------------------------------------------------------------------
namespace ReadableVerificationParameter {
    // Readable field names:
    extern const char *Name;
    extern const char *Value;
    extern const char *Description;
}
//------------------------------------------------------------------------------
namespace ReadableDiagnosis {
    // Readable field names:
    extern const char *Name;
    extern const char *Probability;
}
//------------------------------------------------------------------------------
namespace ReadableUnknownParameter {
    // Readable field names:
    extern const char *ParameterName;
    extern const char *ResearchName;
}
//------------------------------------------------------------------------------
namespace Ui {
    class Verification;
}
//------------------------------------------------------------------------------
class VerificationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VerificationDialog(quint64 patientId, QWidget *parent = 0);
    virtual ~VerificationDialog();

protected:
    virtual void closeEvent(QCloseEvent *);

    bool save();
    virtual void cancel();

    Ui::Verification *ui;

    QDataWidgetMapper *mMapper;

protected slots:
    //-void on_dialogButtonBox_accepted();
    void on_dialogButtonBox_rejected();
    void on_dialogButtonBox_clicked(QAbstractButton *button);

private:
    /*!
     \brief Функция обновляет значения вероятностей для всех диагнозов.
     */
    void updateProbabilitiesOfAllDiagnoses();
    /*!
     \brief Функция возвращает список идентификаторов диагнозов, у которых
        хотя бы одна фаза содержит указанный параметр.
     \param parameterId Идентификатор параметра
     \return QList<quint64> Список идентификаторов диагнозов
     */
    QList<quint64> diagnosesWithParameter(quint64 parameterId);
    /*!
     \brief Функция обновляет значения вероятностей для диагнозов, идентификаторы
        которых переданы.
     \param diagnosisIds Идентификаторы диагнозов
     */
    void updateDiagnosisProbabilities(QList<quint64> &diagnosisIds);
    qreal calculateAverageDiagnosisProbability(quint64 diagnosisId);
    qreal calculateAveragePhaseProbability(quint64 phaseId);
    qreal getPhaseParameterProbability(quint64 phaseParameterId, quint64 parameterId);
    void addNecessaryResearches(quint64 parameterId);
    bool saveUnknownDiagnosisParameters(QSet<quint64> unknownDiagnosisParameters,
                                        quint64 diagnosisId);
    bool saveResearches(QSet<quint64> parameterResearches,
                        quint64 unknownDiagnosisParameterId);
    bool saveDescriptions(QHash<Id, QString> parameterDescriptions,
                          QHash<Id, Id> verificationParameters,
                          Id diagnosisId);
    QString formConclusion();

    quint64 mPatientId;
    QList<quint64> mAllParameterIds; // Список идентификаторов тех параметров,
    // которые в списке отображаются. Это все параметры, которые существуют
    // в БД на момент открытия диалога верификации.
    QList<QSet<quint64> > mUnknownDiagnosisParameters;
    QSet<quint64> mUnknownParameters;
    QHash<Id, QString>  mDiagnosisParameterDescription; // QHash<parameterId, description>.
    QList<QHash<Id, QString> > mDiagnosisParameterDescriptions;
    IdList mDiagnosisIds; // Для возможности определить Id последнего выделенного
    // диагноза.
    Id mLastSelectedDiagnosis; // Идентификатор последнего выделенного диагноза.

private slots:
    void on_verificationParameterTableWidget_itemChanged(QTableWidgetItem *item);
    void on_diagnosisTableWidget_currentItemChanged(QTableWidgetItem *current,
                                                    QTableWidgetItem *previous);
};
//------------------------------------------------------------------------------
#endif // VerificationDialog_h
