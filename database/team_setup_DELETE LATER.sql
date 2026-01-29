CREATE DATABASE IF NOT EXISTS bank;

CREATE USER 'bank_admin'@'localhost' IDENTIFIED BY 'Jaksamista';

GRANT ALL PRIVILEGES ON `bank`.* TO 'bank_admin'@'localhost';

FLUSH PRIVILEGES;
