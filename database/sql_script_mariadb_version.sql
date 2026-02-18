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
  UNIQUE INDEX `email_UNIQUE` (`user_email`),
  UNIQUE INDEX `phone_UNIQUE` (`user_phonenumber`))
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
  INDEX `user_id_idx` (`user_id`),
  UNIQUE INDEX `account_number_UNIQUE` (`account_number`),
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
  UNIQUE INDEX `card_number_UNIQUE` (`card_number`),
  INDEX `card_user_idx` (`user_id`),
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
  INDEX `account_id_idx` (`account_id`),
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
  `transaction_date` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP(),
  PRIMARY KEY (`transaction_id`),
  INDEX `account_id_idx` (`account_id`),
  INDEX `card_id_idx` (`card_id`),
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


-- =====================================================
-- STORED PROCEDURE: DEPOSIT
-- =====================================================

DELIMITER $$

DROP PROCEDURE IF EXISTS deposit_money $$

CREATE PROCEDURE deposit_money(
    IN p_account_id INT,
    IN p_card_id INT,
    IN p_amount DECIMAL(15,2)
)
BEGIN
    DECLARE v_balance DECIMAL(15,2);
    DECLARE v_card_status VARCHAR(20);
    DECLARE v_access_type VARCHAR(20);

    IF p_amount IS NULL OR p_amount <= 0 THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'INVALID_AMOUNT';
    END IF;

    START TRANSACTION;

    SELECT a.balance, c.status, aa.access_type
      INTO v_balance, v_card_status, v_access_type
    FROM account a
    JOIN account_access aa ON aa.account_id = a.account_id
    JOIN card c ON c.card_id = aa.card_id
    WHERE a.account_id = p_account_id
      AND c.card_id = p_card_id
    FOR UPDATE;

    IF v_balance IS NULL THEN
        ROLLBACK;
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'NOT_FOUND';
    END IF;

    IF v_access_type <> 'FULL' THEN
        ROLLBACK;
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'UNAUTHORIZED';
    END IF;

    IF v_card_status <> 'ACTIVE' THEN
        ROLLBACK;
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'LOCKED';
    END IF;

    UPDATE account
    SET balance = balance + p_amount
    WHERE account_id = p_account_id;

    INSERT INTO `transaction`
        (account_id, card_id, transaction_type, amount)
    VALUES
        (p_account_id, p_card_id, 'DEPOSIT', p_amount);

    COMMIT;

    SELECT (v_balance + p_amount) AS new_balance;

END $$

DELIMITER ;

-- =====================================================
-- STORED PROCEDURE: TRANSFER
-- =====================================================
DELIMITER $$

DROP PROCEDURE IF EXISTS transfer_money $$

CREATE PROCEDURE transfer_money(
    IN p_sender_acc_id INT,
    IN p_receiver_acc_num CHAR(18), 
    IN p_card_id INT,
    IN p_amount DECIMAL(15,2)
)
BEGIN
    DECLARE v_sender_balance DECIMAL(15,2);
    DECLARE v_sender_type ENUM('DEBIT', 'CREDIT');
    DECLARE v_sender_limit DECIMAL(15,2);
    DECLARE v_receiver_acc_id INT;
    DECLARE v_card_status VARCHAR(20);
    DECLARE v_access_type VARCHAR(20);

    -- 1. check amount
    IF p_amount <= 0 THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'INVALID_AMOUNT';
    END IF;

    START TRANSACTION;

    -- 2. check sender info and lock the row
    SELECT a.balance, a.account_type, a.credit_limit, c.status, aa.access_type
      INTO v_sender_balance, v_sender_type, v_sender_limit, v_card_status, v_access_type
    FROM account a
    JOIN account_access aa ON aa.account_id = a.account_id
    JOIN card c ON c.card_id = aa.card_id
    WHERE a.account_id = p_sender_acc_id AND c.card_id = p_card_id
    FOR UPDATE;

    -- 3. sender account validation
    IF v_access_type <> 'FULL' THEN
        ROLLBACK; SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'UNAUTHORIZED';
    END IF;

    IF v_card_status <> 'ACTIVE' THEN
        ROLLBACK; SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'CARD_LOCKED';
    END IF;

    -- 4. balance check based on account type
    IF v_sender_type = 'DEBIT' AND (v_sender_balance < p_amount) THEN
        ROLLBACK; SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'INSUFFICIENT_FUNDS';
    ELSEIF v_sender_type = 'CREDIT' AND (v_sender_balance - p_amount < -IFNULL(v_sender_limit, 0)) THEN
        ROLLBACK; SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'EXCEEDS_CREDIT_LIMIT';
    END IF;

    -- 5. check receiver account 
    SELECT account_id INTO v_receiver_acc_id FROM account 
    WHERE account_number = p_receiver_acc_num;

    IF v_receiver_acc_id IS NULL THEN
        ROLLBACK; SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'RECEIVER_NOT_FOUND';
    END IF;

    -- 6. amount transfer
    UPDATE account SET balance = balance - p_amount WHERE account_id = p_sender_acc_id;
    UPDATE account SET balance = balance + p_amount WHERE account_id = v_receiver_acc_id;

    -- 7. record for both sender and receiver
    INSERT INTO `transaction` (account_id, card_id, transaction_type, amount)
    VALUES (p_sender_acc_id, p_card_id, 'TRANSFER', -p_amount);
    
    INSERT INTO `transaction` (account_id, card_id, transaction_type, amount)
    VALUES (v_receiver_acc_id, NULL, 'TRANSFER', p_amount);

    COMMIT;
    
    SELECT (v_sender_balance - p_amount) AS new_balance;

END $$

DELIMITER ;

-- =====================================================
-- STORED PROCEDURE: WITHDRAW
-- =====================================================
DELIMITER $$

DROP PROCEDURE IF EXISTS withdraw_money $$

CREATE PROCEDURE withdraw_money(
    IN p_account_id INT,
    IN p_card_id INT,
    IN p_amount DECIMAL(15,2)
)
BEGIN
    DECLARE v_balance DECIMAL(15,2);
    DECLARE v_card_status VARCHAR(20);
    DECLARE v_access_type VARCHAR(20);

    IF p_amount IS NULL OR p_amount <= 0 THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'INVALID_AMOUNT';
    END IF;

    START TRANSACTION;

    SELECT a.balance, c.status, aa.access_type
      INTO v_balance, v_card_status, v_access_type
    FROM account a
    JOIN account_access aa ON aa.account_id = a.account_id
    JOIN card c ON c.card_id = aa.card_id
    WHERE a.account_id = p_account_id
      AND c.card_id = p_card_id
    FOR UPDATE;

    IF v_balance IS NULL THEN
        ROLLBACK;
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'NOT_FOUND';
    END IF;

    IF v_access_type <> 'FULL' THEN
        ROLLBACK;
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'UNAUTHORIZED';
    END IF;

    IF v_card_status <> 'ACTIVE' THEN
        ROLLBACK;
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'LOCKED';
    END IF;

    IF v_balance < p_amount THEN
        ROLLBACK;
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'INSUFFICIENT_FUNDS';
    END IF;

    UPDATE account
    SET balance = balance - p_amount
    WHERE account_id = p_account_id;

    INSERT INTO `transaction`
        (account_id, card_id, transaction_type, amount)
    VALUES
        (p_account_id, p_card_id, 'WITHDRAWAL', p_amount);

    COMMIT;

    SELECT (v_balance - p_amount) AS new_balance;

END $$

DELIMITER ;




SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
