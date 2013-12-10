ALTER TABLE `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` ADD Weight FLOAT;
UPDATE `LaboratoryAlgorithmDatabaseSchema`.`PhaseParameter` SET Weight = 1;

COMMIT;