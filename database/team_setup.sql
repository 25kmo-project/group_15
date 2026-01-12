CREATE DATABASE IF NOT EXISTS bank;

CREATE USER 'bank_admin'@'localhost' IDENTIFIED BY 'Jaksamista';

GRANT ALL PRIVILEGES ON `bank`.* TO 'bank_admin'@'localhost';

FLUSH PRIVILEGES;

INSERT INTO `user` (user_id, user_name, user_phonenumber) VALUES (1, 'Wilma Kerhonen', '050-1234567');

INSERT INTO `account` (account_id, account_type, balance, credit_limit) VALUES (1, 'Debit', 1000.00, 0.00);
INSERT INTO `account` (account_id, account_type, balance, credit_limit) VALUES (2, 'Credit', 0.00, 5000.00);

INSERT INTO `card` (card_id, card_number, pin_code, status, user_id) VALUES (1, '12345678', '1234', 'Active', 1);

INSERT INTO `account_access` (card_id, account_id, access_type) VALUES (1, 1, 'FULL');
INSERT INTO `account_access` (card_id, account_id, access_type) VALUES (1, 2, 'FULL');