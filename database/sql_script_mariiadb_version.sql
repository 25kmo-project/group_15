-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema bank
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema bank
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `bank` DEFAULT CHARACTER SET utf8 ;
USE `bank` ;

-- -----------------------------------------------------
-- Table `bank`.`user`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bank`.`user` ;

CREATE TABLE IF NOT EXISTS `bank`.`user` (
  `user_id` INT NOT NULL AUTO_INCREMENT,
  `user_name` VARCHAR(45) NOT NULL,
  `user_lastname` VARCHAR(45) NOT NULL,
  `user_address` VARCHAR(50) NOT NULL,
  `user_email` VARCHAR(50) NOT NULL,
  `user_phonenumber` VARCHAR(13) NOT NULL,
  PRIMARY KEY (`user_id`),
  UNIQUE INDEX `user_id_UNIQUE` (`user_id` ),
  UNIQUE INDEX `user_email_UNIQUE` (`user_email` ) ,
  UNIQUE INDEX `user_phonenumber_UNIQUE` (`user_phonenumber` ) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank`.`account`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bank`.`account` ;

CREATE TABLE IF NOT EXISTS `bank`.`account` (
  `account_id` INT NOT NULL,
  `account_type` ENUM('DEBIT', 'CREDIT', 'SAVINGS') NOT NULL,
  `balance` DECIMAL(15,2) NOT NULL,
  `credit_limit` DECIMAL(15,2) NOT NULL,
  `status` ENUM('Active', 'Frozen', 'Closed') NULL,
  PRIMARY KEY (`account_id`),
  UNIQUE INDEX `account_id_UNIQUE` (`account_id` ))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank`.`card`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bank`.`card` ;

CREATE TABLE IF NOT EXISTS `bank`.`card` (
  `card_id` INT NOT NULL,
  `card_number` VARCHAR(20) NOT NULL,
  `pin_code` VARCHAR(255) NOT NULL,
  `status` ENUM('Active', 'Blocked', 'Expired') NOT NULL,
  `user_id` INT NOT NULL,
  UNIQUE INDEX `card_number_UNIQUE` (`card_number` ),
  PRIMARY KEY (`card_id`),
  INDEX `user_id_idx` (`user_id` ) ,
  CONSTRAINT `user_id`
    FOREIGN KEY (`user_id`)
    REFERENCES `bank`.`user` (`user_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank`.`transaction`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bank`.`transaction` ;

CREATE TABLE IF NOT EXISTS `bank`.`transaction` (
  `transaction_id` INT NOT NULL,
  `type` ENUM('Withdrawal', 'Deposit', 'Transfer', 'Inquiry') NOT NULL,
  `amount` DECIMAL(15,2) NOT NULL,
  `date` DATETIME NULL,
  `account_id` INT NULL,
  `card_id` INT NULL,
  PRIMARY KEY (`transaction_id`),
  UNIQUE INDEX `transaction_id_UNIQUE` (`transaction_id`) ,
  INDEX `account_id_idx` (`account_id`),
  CONSTRAINT `account_id`
    FOREIGN KEY (`account_id`)
    REFERENCES `bank`.`account` (`account_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bank`.`account_access`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bank`.`account_access` ;

CREATE TABLE IF NOT EXISTS `bank`.`account_access` (
  `card_id` INT NOT NULL,
  `account_id` INT NOT NULL,
  `access_type` ENUM('FULL', 'VIEW ONLY') NOT NULL,
  PRIMARY KEY (`card_id`, `account_id`),
  INDEX `account_id_idx` (`account_id`),
  INDEX `card_id-idx` (`card_id`),
  CONSTRAINT `fk_card_id`
    FOREIGN KEY (`card_id`)
    REFERENCES `bank`.`card` (`card_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_account_id`
    FOREIGN KEY (`account_id`)
    REFERENCES `bank`.`account` (`account_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
