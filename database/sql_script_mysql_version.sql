-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
-- -----------------------------------------------------
-- Schema bank
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema bank
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `bank` DEFAULT CHARACTER SET utf8mb4 ;
USE `bank` ;

-- -----------------------------------------------------
-- Table `bank`.`user`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank`.`user` (
  `user_id` INT NOT NULL AUTO_INCREMENT,
  `user_name` VARCHAR(45) NOT NULL,
  `user_lastname` VARCHAR(45) NOT NULL,
  `user_address` VARCHAR(100) NOT NULL,
  `user_email` VARCHAR(100) NOT NULL,
  `user_phonenumber` VARCHAR(20) NOT NULL,
  PRIMARY KEY (`user_id`),
  UNIQUE INDEX `email_UNIQUE` (`user_email` ASC) VISIBLE,
  UNIQUE INDEX `phone_UNIQUE` (`user_phonenumber` ASC) VISIBLE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4;


-- -----------------------------------------------------
-- Table `bank`.`account`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank`.`account` (
  `account_id` INT NOT NULL AUTO_INCREMENT,
  `user_id` INT NOT NULL,
  `account_type` ENUM('DEBIT', 'CREDIT') NOT NULL,
  `balance` DECIMAL(15,2) NOT NULL DEFAULT 0.00,
  `credit_limit` DECIMAL(15,2) NULL DEFAULT NULL,
  `account_number` CHAR(18) NOT NULL,
  PRIMARY KEY (`account_id`),
  INDEX `user_id_idx` (`user_id` ASC) VISIBLE,
  UNIQUE INDEX `account_number_UNIQUE` (`account_number` ASC) VISIBLE,
  CONSTRAINT `fk_account_user`
    FOREIGN KEY (`user_id`)
    REFERENCES `bank`.`user` (`user_id`)
    ON DELETE RESTRICT
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4;


-- -----------------------------------------------------
-- Table `bank`.`card`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank`.`card` (
  `card_id` INT NOT NULL AUTO_INCREMENT,
  `card_number` CHAR(16) NOT NULL,
  `pin_code` VARCHAR(60) NOT NULL,
  `card_type` ENUM('PARENT', 'CHILD') NOT NULL DEFAULT 'PARENT',
  `user_id` INT NOT NULL,
  `status` ENUM('ACTIVE', 'CLOSED') NOT NULL,
  `failed_pin_attempts` INT NOT NULL DEFAULT 0,
  PRIMARY KEY (`card_id`),
  UNIQUE INDEX `card_number_UNIQUE` (`card_number` ASC) VISIBLE,
  INDEX `card_user_idx` (`user_id` ASC) VISIBLE,
  CONSTRAINT `fk_card_user`
    FOREIGN KEY (`user_id`)
    REFERENCES `bank`.`user` (`user_id`)
    ON DELETE RESTRICT
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4;


-- -----------------------------------------------------
-- Table `bank`.`account_access`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank`.`account_access` (
  `card_id` INT NOT NULL,
  `account_id` INT NOT NULL,
  `access_type` ENUM('FULL', 'VIEW_ONLY') NOT NULL,
  PRIMARY KEY (`card_id`, `account_id`),
  INDEX `account_id_idx` (`account_id` ASC) VISIBLE,
  CONSTRAINT `fk_access_account`
    FOREIGN KEY (`account_id`)
    REFERENCES `bank`.`account` (`account_id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_access_card`
    FOREIGN KEY (`card_id`)
    REFERENCES `bank`.`card` (`card_id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4;


-- -----------------------------------------------------
-- Table `bank`.`transaction`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `bank`.`transaction` (
  `transaction_id` INT NOT NULL AUTO_INCREMENT,
  `account_id` INT NOT NULL,
  `card_id` INT NULL,
  `transaction_type` ENUM('WITHDRAWAL', 'DEPOSIT', 'TRANSFER', 'INQUIRY') NOT NULL,
  `amount` DECIMAL(15,2) NOT NULL,
  `transaction_date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP(),
  PRIMARY KEY (`transaction_id`),
  INDEX `account_id_idx` (`account_id` ASC) VISIBLE,
  INDEX `card_id_idx` (`card_id` ASC) VISIBLE,
  CONSTRAINT `fk_transaction_account`
    FOREIGN KEY (`account_id`)
    REFERENCES `bank`.`account` (`account_id`)
    ON DELETE RESTRICT
    ON UPDATE CASCADE,
  CONSTRAINT `fk_transaction_card`
    FOREIGN KEY (`card_id`)
    REFERENCES `bank`.`card` (`card_id`)
    ON DELETE SET NULL
    ON UPDATE CASCADE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
