#ifndef PROGNOSISDIALOG_H
#define PROGNOSISDIALOG_H

#include <QDialog>

#include "DB.h"

class QTableWidgetItem;
class QAbstractButton;
//------------------------------------------------------------------------------
namespace ReadableParameterValue {
    // Readable field names:
    extern const char *Name;
    extern const char *Value;
}
//------------------------------------------------------------------------------
namespace ReadablePhase {
    // Readable field names:
    extern const char *Name;
    extern const char *Probability;
}
//------------------------------------------------------------------------------
namespace ReadableUnknownParameter2 {
    // Readable field names:
    extern const char *ParameterName;
    extern const char *ResearchName;
}
//------------------------------------------------------------------------------
namespace Ui {
class Prognosis;
}
//------------------------------------------------------------------------------
class PrognosisDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PrognosisDialog(quint64 patientId, QWidget *parent = 0);
    ~PrognosisDialog();
            
protected:
    virtual void closeEvent(QCloseEvent *);

    bool save();
    virtual void cancel();

protected slots:
    //-void on_dialogButtonBox_accepted();
    void on_dialogButtonBox_rejected();
    void on_dialogButtonBox_clicked(QAbstractButton *button);

private:
    /*!
     \brief Функция обновляет значения вероятностей для всех фаз.
     */
    void updateProbabilitiesOfAllPhases();
    /*!
     \brief Функция обновляет значения вероятностей для фаз, идентификаторы
        которых переданы.
     \param phaseIds Идентификаторы фаз
     */
    void updatePhaseProbabilities(IdList &phaseIds);
    //! Calculates average phase probability, doesn't take into account parameter weights.
    qreal calculateAveragePhaseProbability(Id phaseId);
    //! Calculates average phase probability, takes into account parameter weights.
    qreal calculateAverageWeightPhaseProbability(Id phaseId);
    qreal getPhaseParameterProbability(Id phaseParameterId, Id parameterId);
    void addNecessaryResearches(Id unknownParameterId);
    bool saveUnknownPhaseParameters(IdSet unknownPhaseParameters,
                                    Id phaseProbabilityId);
    bool saveResearches(IdSet parameterResearches,
                        Id unknownPhaseParameterId);

    Ui::Prognosis *ui;
    Id mPatientId;
    Id mDiagnosisId;
    IdList mAllParameterIds; // Список идентификаторов тех параметров,
    // которые в списке отображаются. Это все параметры, которые существуют
    // в БД на момент открытия диалога верификации.
    QList<IdNamePair> mDiagnosisIdsNames;
    QList<IdSet> mUnknownPhaseParameters;
    IdSet mUnknownParameters;

private slots:
    void on_diagnosisLineEdit_textChanged(const QString &arg1);
    void on_diagnosisComboBox_currentIndexChanged(int index);

    void on_parameterValueTableWidget_itemChanged(QTableWidgetItem *item);
    void on_phaseTableWidget_currentItemChanged(QTableWidgetItem *current,
                                                QTableWidgetItem *previous);
};
//------------------------------------------------------------------------------
#endif // PROGNOSISDIALOG_H
