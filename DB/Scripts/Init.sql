SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

DROP SCHEMA IF EXISTS `LaboratoryAlgorithmDatabaseSchema` ;
CREATE SCHEMA IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema` DEFAULT CHARACTER SET utf8 ;
USE `LaboratoryAlgorithmDatabaseSchema` ;

-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`Diagnosis`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`Diagnosis` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  PRIMARY KEY (`ID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`Phase`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`Phase` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `DiagnosisID` INT NOT NULL ,
  `Name` TINYTEXT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_Phase_Diagnosis_idx` (`DiagnosisID` ASC) ,
  CONSTRAINT `fk_Phase_Diagnosis`
    FOREIGN KEY (`DiagnosisID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Diagnosis` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`Parameter`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  PRIMARY KEY (`ID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `PhaseID` INT NOT NULL ,
  `ParameterID` INT NOT NULL ,
  `Weight` FLOAT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_PhaseParameter_Phase1_idx` (`PhaseID` ASC) ,
  INDEX `fk_PhaseParameter_Parameter1_idx` (`ParameterID` ASC) ,
  CONSTRAINT `fk_PhaseParameter_Phase1`
    FOREIGN KEY (`PhaseID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Phase` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_PhaseParameter_Parameter1`
    FOREIGN KEY (`ParameterID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`Research`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`Research` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  PRIMARY KEY (`ID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`ParameterResearch`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`ParameterResearch` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `ParameterID` INT NOT NULL ,
  `ResearchID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_ParameterResearch_Parameter1_idx` (`ParameterID` ASC) ,
  INDEX `fk_ParameterResearch_Research1_idx` (`ResearchID` ASC) ,
  CONSTRAINT `fk_ParameterResearch_Parameter1`
    FOREIGN KEY (`ParameterID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_ParameterResearch_Research1`
    FOREIGN KEY (`ResearchID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Research` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`Patient`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`Patient` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `LastName` VARCHAR(45) NOT NULL ,
  `FirstName` VARCHAR(45) NOT NULL ,
  `MiddleName` VARCHAR(45) NOT NULL ,
  `BirthDate` DATE NULL ,
  `Sex` TINYINT(1) NULL ,
  `Addresses` TEXT NULL ,
  `Phones` TEXT NULL ,
  `Job` TINYTEXT NULL ,
  `Post` TINYTEXT NULL ,
  `PassportSeries` VARCHAR(45) NULL ,
  `PassportNumber` INT UNSIGNED NULL ,
  `PassportIssueDate` DATE NULL ,
  `PassportIssuingAuthority` TINYTEXT NULL ,
  `PassportPersonalNumber` BIGINT UNSIGNED NULL ,
  `AdditionalInformation` MEDIUMTEXT NULL ,
  PRIMARY KEY (`ID`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`Prognosis`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`Prognosis` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `PatientID` INT NOT NULL ,
  `DateTime` DATETIME NOT NULL ,
  `DiagnosisName` TINYTEXT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_Prognosis_Patient1_idx` (`PatientID` ASC) ,
  CONSTRAINT `fk_Prognosis_Patient1`
    FOREIGN KEY (`PatientID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Patient` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`Verification`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`Verification` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `PatientID` INT NOT NULL ,
  `DateTime` DATETIME NOT NULL ,
  `Conclusion` TEXT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_Verification_Patient1_idx` (`PatientID` ASC) ,
  CONSTRAINT `fk_Verification_Patient1`
    FOREIGN KEY (`PatientID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Patient` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `PhaseParameterID` INT NOT NULL ,
  `RangeBegin` DOUBLE NOT NULL ,
  `RangeEnd` DOUBLE NOT NULL ,
  `BeginProbability` FLOAT NOT NULL ,
  `EndProbability` FLOAT NOT NULL ,
  `Description` TEXT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_Range_PhaseParameter1_idx` (`PhaseParameterID` ASC) ,
  CONSTRAINT `fk_Range_PhaseParameter1`
    FOREIGN KEY (`PhaseParameterID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`VerificationParameter`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`VerificationParameter` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  `Value` FLOAT NOT NULL ,
  `VerificationID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_VerificationParameter_Verification1_idx` (`VerificationID` ASC) ,
  CONSTRAINT `fk_VerificationParameter_Verification1`
    FOREIGN KEY (`VerificationID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Verification` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`PresumptiveVerificationDiagnosis`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`PresumptiveVerificationDiagnosis` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  `Probability` FLOAT NOT NULL ,
  `VerificationID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_PresumptiveVerificationDiagnosis_Verification1_idx` (`VerificationID` ASC) ,
  CONSTRAINT `fk_PresumptiveVerificationDiagnosis_Verification1`
    FOREIGN KEY (`VerificationID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Verification` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`UnknownDiagnosisParameter`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`UnknownDiagnosisParameter` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  `PresumptiveVerificationDiagnosisID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_UnknownDiagnosisParameter_PresumptiveVerificationDiagnos_idx` (`PresumptiveVerificationDiagnosisID` ASC) ,
  CONSTRAINT `fk_UnknownDiagnosisParameter_PresumptiveVerificationDiagnosis1`
    FOREIGN KEY (`PresumptiveVerificationDiagnosisID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`PresumptiveVerificationDiagnosis` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`NecessaryAdditionalParameterResearch`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`NecessaryAdditionalParameterResearch` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  `UnknownDiagnosisParameterID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_NecessaryAdditionalParameterResearch_UnknownDiagnosisPar_idx` (`UnknownDiagnosisParameterID` ASC) ,
  CONSTRAINT `fk_NecessaryAdditionalParameterResearch_UnknownDiagnosisParam1`
    FOREIGN KEY (`UnknownDiagnosisParameterID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`UnknownDiagnosisParameter` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`ParameterValue`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`ParameterValue` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  `Value` FLOAT NOT NULL ,
  `PrognosisID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_ParameterValue_Prognosis1_idx` (`PrognosisID` ASC) ,
  CONSTRAINT `fk_ParameterValue_Prognosis1`
    FOREIGN KEY (`PrognosisID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Prognosis` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`PhaseProbability`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`PhaseProbability` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  `Probability` FLOAT NOT NULL ,
  `PrognosisID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_PhaseProbability_Prognosis1_idx` (`PrognosisID` ASC) ,
  CONSTRAINT `fk_PhaseProbability_Prognosis1`
    FOREIGN KEY (`PrognosisID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`Prognosis` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`UnknownPhaseParameter`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`UnknownPhaseParameter` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  `PhaseProbabilityID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_UnknownPhaseParameter_PhaseProbability1_idx` (`PhaseProbabilityID` ASC) ,
  CONSTRAINT `fk_UnknownPhaseParameter_PhaseProbability1`
    FOREIGN KEY (`PhaseProbabilityID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`PhaseProbability` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`NecessaryResearch`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`NecessaryResearch` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Name` TINYTEXT NOT NULL ,
  `UnknownPhaseParameterID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_NecessaryResearch_UnknownPhaseParameter1_idx` (`UnknownPhaseParameterID` ASC) ,
  CONSTRAINT `fk_NecessaryResearch_UnknownPhaseParameter1`
    FOREIGN KEY (`UnknownPhaseParameterID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`UnknownPhaseParameter` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `LaboratoryAlgorithmDatabaseSchema`.`VerificationDescription`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `LaboratoryAlgorithmDatabaseSchema`.`VerificationDescription` (
  `ID` INT NOT NULL AUTO_INCREMENT ,
  `Text` TEXT NULL ,
  `VerificationParameterID` INT NOT NULL ,
  `PresumptiveVerificationDiagnosisID` INT NOT NULL ,
  PRIMARY KEY (`ID`) ,
  INDEX `fk_Description_PresumptiveVerificationDiagnosis1_idx` (`PresumptiveVerificationDiagnosisID` ASC) ,
  INDEX `fk_Description_VerificationParameter1_idx` (`VerificationParameterID` ASC) ,
  CONSTRAINT `fk_Description_PresumptiveVerificationDiagnosis1`
    FOREIGN KEY (`PresumptiveVerificationDiagnosisID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`PresumptiveVerificationDiagnosis` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Description_VerificationParameter1`
    FOREIGN KEY (`VerificationParameterID` )
    REFERENCES `LaboratoryAlgorithmDatabaseSchema`.`VerificationParameter` (`ID` )
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

-- -----------------------------------------------------
-- Data for table `LaboratoryAlgorithmDatabaseSchema`.`Diagnosis`
-- -----------------------------------------------------
START TRANSACTION;
USE `LaboratoryAlgorithmDatabaseSchema`;
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Diagnosis` (`ID`, `Name`) VALUES (1, 'Сахарный диабет');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Diagnosis` (`ID`, `Name`) VALUES (2, 'Сахарный диабет 1-ого типа');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Diagnosis` (`ID`, `Name`) VALUES (3, 'Бронхит хронический');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Diagnosis` (`ID`, `Name`) VALUES (4, 'Острый панкреатит');

COMMIT;

-- -----------------------------------------------------
-- Data for table `LaboratoryAlgorithmDatabaseSchema`.`Phase`
-- -----------------------------------------------------
START TRANSACTION;
USE `LaboratoryAlgorithmDatabaseSchema`;
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Phase` (`ID`, `DiagnosisID`, `Name`) VALUES (1, 3, 'Обострение');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Phase` (`ID`, `DiagnosisID`, `Name`) VALUES (2, 3, 'Критическая');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Phase` (`ID`, `DiagnosisID`, `Name`) VALUES (3, 2, 'Компенсация');

COMMIT;

-- -----------------------------------------------------
-- Data for table `LaboratoryAlgorithmDatabaseSchema`.`Parameter`
-- -----------------------------------------------------
START TRANSACTION;
USE `LaboratoryAlgorithmDatabaseSchema`;
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (1, 'Гликемия натощак (ммоль/л)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (2, 'Возраст пациента');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (3, 'Появление одышки при физической нагрузке');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (4, 'Количество лейкоцитов (10^9/л)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (5, 'CD25 (N 4 - 11%)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (6, 'Каталаза (34 - 48 мкмоль/мл*Нв)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (7, 'Гликированный гемоглобин');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (8, 'Общий холестерин (ммоль/л)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (9, 'Кортизол (83 - 580 нмоль/л)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (10, 'Малоновый диальдегид (0,95 - 1,05 мкмоль/л)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (11, 'Амилаза (28 - 100 Ед/л)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (12, 'Кашель с мокротой не менее 3 мес в году в течение 2 лет');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (13, 'Температура');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Parameter` (`ID`, `Name`) VALUES (14, 'Тестовый параметр');

COMMIT;

-- -----------------------------------------------------
-- Data for table `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter`
-- -----------------------------------------------------
START TRANSACTION;
USE `LaboratoryAlgorithmDatabaseSchema`;
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (1, 1, 3, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (2, 1, 12, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (3, 1, 13, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (4, 2, 2, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (5, 2, 3, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (6, 2, 12, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (7, 2, 13, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (8, 3, 7, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` (`ID`, `PhaseID`, `ParameterID`, `Weight`) VALUES (9, 3, 2, 1);

COMMIT;

-- -----------------------------------------------------
-- Data for table `LaboratoryAlgorithmDatabaseSchema`.`Research`
-- -----------------------------------------------------
START TRANSACTION;
USE `LaboratoryAlgorithmDatabaseSchema`;
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Research` (`ID`, `Name`) VALUES (1, 'Исследование температуры. Способ №1');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Research` (`ID`, `Name`) VALUES (2, 'Исследование температуры. Способ №2');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Research` (`ID`, `Name`) VALUES (3, 'Исследование легких.');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Research` (`ID`, `Name`) VALUES (4, 'Электрофорез');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Research` (`ID`, `Name`) VALUES (5, 'Жидкостная хроматография');

COMMIT;

-- -----------------------------------------------------
-- Data for table `LaboratoryAlgorithmDatabaseSchema`.`ParameterResearch`
-- -----------------------------------------------------
START TRANSACTION;
USE `LaboratoryAlgorithmDatabaseSchema`;
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`ParameterResearch` (`ID`, `ParameterID`, `ResearchID`) VALUES (1, 13, 1);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`ParameterResearch` (`ID`, `ParameterID`, `ResearchID`) VALUES (2, 13, 2);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`ParameterResearch` (`ID`, `ParameterID`, `ResearchID`) VALUES (3, 12, 3);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`ParameterResearch` (`ID`, `ParameterID`, `ResearchID`) VALUES (4, 7, 4);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`ParameterResearch` (`ID`, `ParameterID`, `ResearchID`) VALUES (5, 7, 5);

COMMIT;

-- -----------------------------------------------------
-- Data for table `LaboratoryAlgorithmDatabaseSchema`.`Patient`
-- -----------------------------------------------------
START TRANSACTION;
USE `LaboratoryAlgorithmDatabaseSchema`;
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Patient` (`ID`, `LastName`, `FirstName`, `MiddleName`, `BirthDate`, `Sex`, `Addresses`, `Phones`, `Job`, `Post`, `PassportSeries`, `PassportNumber`, `PassportIssueDate`, `PassportIssuingAuthority`, `PassportPersonalNumber`, `AdditionalInformation`) VALUES (1, 'Андрейченко', 'Людмила', 'Сергеевна', NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Patient` (`ID`, `LastName`, `FirstName`, `MiddleName`, `BirthDate`, `Sex`, `Addresses`, `Phones`, `Job`, `Post`, `PassportSeries`, `PassportNumber`, `PassportIssueDate`, `PassportIssuingAuthority`, `PassportPersonalNumber`, `AdditionalInformation`) VALUES (2, 'Андреев', 'Николай', 'Игнатьевич', NULL, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Patient` (`ID`, `LastName`, `FirstName`, `MiddleName`, `BirthDate`, `Sex`, `Addresses`, `Phones`, `Job`, `Post`, `PassportSeries`, `PassportNumber`, `PassportIssueDate`, `PassportIssuingAuthority`, `PassportPersonalNumber`, `AdditionalInformation`) VALUES (3, 'Горбоносов', 'Никодим', 'Феофанович', NULL, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Patient` (`ID`, `LastName`, `FirstName`, `MiddleName`, `BirthDate`, `Sex`, `Addresses`, `Phones`, `Job`, `Post`, `PassportSeries`, `PassportNumber`, `PassportIssueDate`, `PassportIssuingAuthority`, `PassportPersonalNumber`, `AdditionalInformation`) VALUES (4, 'Грибоедов', 'Иван', 'Иванович', NULL, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`Patient` (`ID`, `LastName`, `FirstName`, `MiddleName`, `BirthDate`, `Sex`, `Addresses`, `Phones`, `Job`, `Post`, `PassportSeries`, `PassportNumber`, `PassportIssueDate`, `PassportIssuingAuthority`, `PassportPersonalNumber`, `AdditionalInformation`) VALUES (5, 'Змитрович', 'Тамара', 'Сергеевна', NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

COMMIT;

-- -----------------------------------------------------
-- Data for table `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange`
-- -----------------------------------------------------
START TRANSACTION;
USE `LaboratoryAlgorithmDatabaseSchema`;
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (1, 1, 0, 1, 0, 1, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (2, 2, 0, 1, 0.1, 0.9, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (3, 3, 36.5, 39, 0, 1, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (4, 3, 39, 40, 1, 1, 'Высокое значение. Это может...');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (5, 4, 50, 70, 0, 1, 'Доп. описание значения параметра возраста (50 - 70) для диагноза Бронхит');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (6, 4, 70, 150, 1, 1, 'Доп. описание значения параметра возраста (70 - 150) для диагноза Бронхит');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (7, 5, 0.7, 1, 0, 1, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (8, 6, 0.7, 1, 0, 1, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (9, 7, 40, 43, 0, 1, NULL);
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (10, 8, 3, 7, 0, 1, 'Параметр эффективной терапии заболевания (при снижении)');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (11, 8, 7, 100, 1, 1, 'Увеличение значения свидетельствует о...');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (12, 9, 25, 50, 0, 1, 'Доп. описание значения параметра возраста (25 - 50) для диагноза Сах. диабет 1-ого типа');
INSERT INTO `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameterRange` (`ID`, `PhaseParameterID`, `RangeBegin`, `RangeEnd`, `BeginProbability`, `EndProbability`, `Description`) VALUES (13, 9, 50, 150, 1, 1, 'Доп. описание значения параметра возраста (50 - 150) для диагноза Сах. диабет 1-ого типа');

COMMIT;
