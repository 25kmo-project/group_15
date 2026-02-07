-- ============================================
-- BANK DATABASE - USER SCENARIOS TEST DATA
-- ============================================
-- ============================================
-- BANK DATABASE - USER SCENARIOS TEST DATA
-- ============================================
-- PIN CODES (bcrypt hashed with saltRounds=10):
-- Plain: 1111 → Hash: $2b$10$6444dXdtyJD4mRwi4c3TieNyFkA1FOu2.mHggnS35cnVSbQt7UQj2
-- Plain: 2222 → Hash: $2b$10$M3D5iLA/i9TNiMhJszTuB.7S00D9vXiNX./laY1xA4/rTVw2ANYdm
-- Plain: 3333 → Hash: $2b$10$OCzcdvZdp4UVqlWWDGh.oexqbnOVGM4fLZr1rKBfTihseLMzcOLgW
-- Plain: 4444 → Hash: $2b$10$XgyGucjIVpMW5Cr8FEGpuOGjFdjo2VxFWqAOffEGJo1ogQHXLM8L6
-- Plain: 5555 → Hash: $2b$10$.LJrr0Rtfp4VZpoyDRQ1EOQqLyp9ibJMl4bAoqxf1KNyp0jZlNLGK
-- Plain: 6666 → Hash: $2b$10$3wbrqTpkBXQa1f/tbl41LeSuYPeZj7cF4b5WdeWPp2Z8pfPNPvGPu
-- Plain: 7777 → Hash: $2b$10$gJb9yT/qQI8lWPN4UtlJxefuhVuYtRKatiTRj5VEvDz4UZcpk08KS
-- Plain: 8888 → Hash: $2b$10$.hHYS.qoG5owdLCrfFl6fOZEAKwSJIO6I2tdJbcEt3PilIeT39HYC
-- Plain: 9999 → Hash: $2b$10$9rGPGXKTPueXivSFDP/4p.5rcnkpNgMqL9x1zoQFs8ViLfaRfqJyC
-- Plain: 1010 → Hash: $2b$10$CcAAUUCaIU5qvv1kZ3Pnv.m3nxPWDZaSTNDTgBlZNmvGLrNjDAH52
-- Plain: 0011 → Hash: $2b$10$T3Od/3IjB/JRgFGpMzKL3OrEdkqfB1t6qYUOALQRiXxCkZI6rBQ8C
-- Plain: 1212 → Hash: $2b$10$tJGHFtYkndykxN44oNlKeuC3os7ZwdWfUpOzql7Gw17hDS2wxW9HO
-- Plain: 1313 → Hash: $2b$10$GiPfLpIVTnyP95zqY0N2qe0fwUwND3ozBomLOZkhxFfWFrKYSyU4G
-- Plain: 1414 → Hash: $2b$10$YitrgtxSCzHPn05TjZaI/eETm8oED5WpfkMrdtdpRh8VBtXnEz11S
-- Plain: 1515 → Hash: $2b$10$67jVW5qX/68ScQiGmc6AHeaBZEaelb130qzkDX6a11ZQ7xaq9DJ0i
-- Plain: 1616 → Hash: $2b$10$AdtsKPUb7vjfn7gEWkt/JuqPTndhiBc7wcaZQdT113EvoKstyfms6
-- Plain: 1717 → Hash: $2b$10$92I/nSR90368AT97yvYo0eLxsDam8Oi.5OO2GszTRq7dxgmrCYJpe
-- ============================================

USE bank;

-- ============================================
-- Skenaario #1: Perus DEBIT-kortin haltija
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Matti', 'Virtanen', 'Esimerkkikatu 1, 90100 Oulu', 'user01@email.fi', '04511111111');
SET @user1_id = LAST_INSERT_ID();

INSERT INTO account (user_id, account_type, balance, credit_limit, account_number)
VALUES (@user1_id, 'DEBIT', 1000.00, NULL, 'FI1111111111111111');
SET @account1_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1111111111111111', '$2b$10$6444dXdtyJD4mRwi4c3TieNyFkA1FOu2.mHggnS35cnVSbQt7UQj2', 'PARENT', @user1_id, 'ACTIVE', 0);
SET @card1_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card1_id, @account1_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account1_id, @card1_id, 'DEPOSIT', 500.00, '2026-01-15 10:00:00'),
(@account1_id, @card1_id, 'WITHDRAWAL', 50.00, '2026-01-20 14:30:00'),
(@account1_id, @card1_id, 'INQUIRY', 0.00, '2026-01-25 09:15:00'),
(@account1_id, @card1_id, 'TRANSFER', 100.00, '2026-01-27 11:00:00');

-- ============================================
-- Skenaario #2: Perus CREDIT-kortin haltija
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Liisa', 'Korhonen', 'Esimerkkikatu 2, 90100 Oulu', 'user02@email.fi', '04522222222');
SET @user2_id = LAST_INSERT_ID();

INSERT INTO account (user_id, account_type, balance, credit_limit, account_number)
VALUES (@user2_id, 'CREDIT', -1200.00, 10000.00, 'FI2222222222222222');
SET @account2_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('2222222222222222', '$2b$10$M3D5iLA/i9TNiMhJszTuB.7S00D9vXiNX./laY1xA4/rTVw2ANYdm', 'PARENT', @user2_id, 'ACTIVE', 0);
SET @card2_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card2_id, @account2_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account2_id, @card2_id, 'WITHDRAWAL', 1200.00, '2026-01-10 11:00:00'),
(@account2_id, @card2_id, 'DEPOSIT', 500.00, '2026-01-22 16:45:00'),
(@account2_id, @card2_id, 'INQUIRY', 0.00, '2026-01-24 10:00:00'),
(@account2_id, @card2_id, 'TRANSFER', 200.00, '2026-01-26 14:00:00');

-- ============================================
-- Skenaario #3: Kaksoiskortti (DEBIT + CREDIT)
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Jukka', 'Nieminen', 'Esimerkkikatu 3, 90100 Oulu', 'user03@email.fi', '04533333333');
SET @user3_id = LAST_INSERT_ID();

INSERT INTO account (user_id, account_type, balance, credit_limit, account_number)
VALUES 
(@user3_id, 'DEBIT', 3500.00, NULL, 'FI3333333333333333'),
(@user3_id, 'CREDIT', -800.00, 5000.00, 'FI3333333333333334');
SET @account3_debit_id = LAST_INSERT_ID() - 1;
SET @account3_credit_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('3333333333333333', '$2b$10$OCzcdvZdp4UVqlWWDGh.oexqbnOVGM4fLZr1rKBfTihseLMzcOLgW', 'PARENT', @user3_id, 'ACTIVE', 0);
SET @card3_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES 
(@card3_id, @account3_debit_id, 'FULL'),
(@card3_id, @account3_credit_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account3_debit_id, @card3_id, 'DEPOSIT', 3500.00, '2026-01-05 09:00:00'),
(@account3_debit_id, @card3_id, 'WITHDRAWAL', 150.00, '2026-01-18 12:30:00'),
(@account3_debit_id, @card3_id, 'INQUIRY', 0.00, '2026-01-20 10:00:00'),
(@account3_credit_id, @card3_id, 'WITHDRAWAL', 800.00, '2026-01-12 15:20:00'),
(@account3_credit_id, @card3_id, 'DEPOSIT', 200.00, '2026-01-22 11:00:00'),
(@account3_credit_id, @card3_id, 'INQUIRY', 0.00, '2026-01-23 16:00:00'),
(@account3_debit_id, @card3_id, 'TRANSFER', 300.00, '2026-01-25 14:00:00');

-- ============================================
-- Skenaario #4: Vanhempi (PARENT) - DEBIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Kaisa', 'Mäkinen', 'Esimerkkikatu 4, 90100 Oulu', 'user04@email.fi', '04544444444');
SET @user4_id = LAST_INSERT_ID();

INSERT INTO account (user_id, account_type, balance, credit_limit, account_number)
VALUES (@user4_id, 'DEBIT', 6000.00, NULL, 'FI4444444444444444');
SET @account4_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('4444444444444444', '$2b$10$XgyGucjIVpMW5Cr8FEGpuOGjFdjo2VxFWqAOffEGJo1ogQHXLM8L6', 'PARENT', @user4_id, 'ACTIVE', 0);
SET @card4_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card4_id, @account4_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account4_id, @card4_id, 'DEPOSIT', 6000.00, '2026-01-01 09:00:00'),
(@account4_id, @card4_id, 'WITHDRAWAL', 400.00, '2026-01-10 11:30:00'),
(@account4_id, @card4_id, 'INQUIRY', 0.00, '2026-01-15 14:00:00'),
(@account4_id, @card4_id, 'TRANSFER', 250.00, '2026-01-18 10:30:00');

-- ============================================
-- Skenaario #5: Lapsi (CHILD) - VIEW_ONLY - DEBIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Emma', 'Mäkinen', 'Esimerkkikatu 4, 90100 Oulu', 'user05@email.fi', '04555555555');
SET @user5_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('5555555555555555', '$2b$10$.LJrr0Rtfp4VZpoyDRQ1EOQqLyp9ibJMl4bAoqxf1KNyp0jZlNLGK', 'CHILD', @user5_id, 'ACTIVE', 0);
SET @card5_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card5_id, @account4_id, 'VIEW_ONLY');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account4_id, @card5_id, 'INQUIRY', 0.00, '2026-01-16 17:00:00'),
(@account4_id, @card5_id, 'INQUIRY', 0.00, '2026-01-19 19:00:00');

-- ============================================
-- Skenaario #6: Lapsi (CHILD) - FULL - DEBIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Ville', 'Mäkinen', 'Esimerkkikatu 4, 90100 Oulu', 'user06@email.fi', '04566666666');
SET @user6_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('6666666666666666', '$2b$10$3wbrqTpkBXQa1f/tbl41LeSuYPeZj7cF4b5WdeWPp2Z8pfPNPvGPu', 'CHILD', @user6_id, 'ACTIVE', 0);
SET @card6_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card6_id, @account4_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account4_id, @card6_id, 'WITHDRAWAL', 50.00, '2026-01-18 14:00:00'),
(@account4_id, @card6_id, 'DEPOSIT', 100.00, '2026-01-20 10:00:00'),
(@account4_id, @card6_id, 'INQUIRY', 0.00, '2026-01-22 10:30:00'),
(@account4_id, @card6_id, 'TRANSFER', 75.00, '2026-01-24 15:00:00');

-- ============================================
-- Skenaario #7: Vanhempi (PARENT) - CREDIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Mikko', 'Heikkinen', 'Esimerkkikatu 7, 90100 Oulu', 'user07@email.fi', '04577777777');
SET @user7_id = LAST_INSERT_ID();

INSERT INTO account (user_id, account_type, balance, credit_limit, account_number)
VALUES (@user7_id, 'CREDIT', -2500.00, 15000.00, 'FI7777777777777777');
SET @account7_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('7777777777777777', '$2b$10$gJb9yT/qQI8lWPN4UtlJxefuhVuYtRKatiTRj5VEvDz4UZcpk08KS', 'PARENT', @user7_id, 'ACTIVE', 0);
SET @card7_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card7_id, @account7_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account7_id, @card7_id, 'WITHDRAWAL', 2500.00, '2026-01-08 13:00:00'),
(@account7_id, @card7_id, 'DEPOSIT', 1000.00, '2026-01-25 10:00:00'),
(@account7_id, @card7_id, 'INQUIRY', 0.00, '2026-01-26 11:00:00'),
(@account7_id, @card7_id, 'TRANSFER', 500.00, '2026-01-27 14:30:00');

-- ============================================
-- Skenaario #8: Lapsi (CHILD) - VIEW_ONLY - CREDIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Aino', 'Heikkinen', 'Esimerkkikatu 7, 90100 Oulu', 'user08@email.fi', '04588888888');
SET @user8_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('8888888888888888', '$2b$10$.hHYS.qoG5owdLCrfFl6fOZEAKwSJIO6I2tdJbcEt3PilIeT39HYC', 'CHILD', @user8_id, 'ACTIVE', 0);
SET @card8_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card8_id, @account7_id, 'VIEW_ONLY');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account7_id, @card8_id, 'INQUIRY', 0.00, '2026-01-20 16:45:00'),
(@account7_id, @card8_id, 'INQUIRY', 0.00, '2026-01-23 18:00:00');

-- ============================================
-- Skenaario #9: Lapsi (CHILD) - FULL - CREDIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Oskar', 'Heikkinen', 'Esimerkkikatu 7, 90100 Oulu', 'user09@email.fi', '04599999999');
SET @user9_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('9999999999999999', '$2b$10$9rGPGXKTPueXivSFDP/4p.5rcnkpNgMqL9x1zoQFs8ViLfaRfqJyC', 'CHILD', @user9_id, 'ACTIVE', 0);
SET @card9_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card9_id, @account7_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account7_id, @card9_id, 'WITHDRAWAL', 200.00, '2026-01-17 13:20:00'),
(@account7_id, @card9_id, 'DEPOSIT', 100.00, '2026-01-19 11:00:00'),
(@account7_id, @card9_id, 'INQUIRY', 0.00, '2026-01-24 11:00:00'),
(@account7_id, @card9_id, 'TRANSFER', 150.00, '2026-01-26 15:30:00');

-- ============================================
-- Skenaario #10: Vanhempi kaksoiskortti (DEBIT+CREDIT)
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Sari', 'Rantanen', 'Esimerkkikatu 10, 90100 Oulu', 'user10@email.fi', '04510101010');
SET @user10_id = LAST_INSERT_ID();

INSERT INTO account (user_id, account_type, balance, credit_limit, account_number)
VALUES
(@user10_id, 'DEBIT', 4500.00, NULL, 'FI1010101010101010'),
(@user10_id, 'CREDIT', -1500.00, 8000.00, 'FI1010101010101011');
SET @account10_debit_id = LAST_INSERT_ID() - 1;
SET @account10_credit_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1010101010101010', '$2b$10$CcAAUUCaIU5qvv1kZ3Pnv.m3nxPWDZaSTNDTgBlZNmvGLrNjDAH52', 'PARENT', @user10_id, 'ACTIVE', 0);
SET @card10_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES
(@card10_id, @account10_debit_id, 'FULL'),
(@card10_id, @account10_credit_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account10_debit_id, @card10_id, 'DEPOSIT', 4500.00, '2026-01-03 09:00:00'),
(@account10_debit_id, @card10_id, 'WITHDRAWAL', 200.00, '2026-01-15 12:00:00'),
(@account10_debit_id, @card10_id, 'INQUIRY', 0.00, '2026-01-18 10:00:00'),
(@account10_credit_id, @card10_id, 'WITHDRAWAL', 1500.00, '2026-01-14 15:30:00'),
(@account10_credit_id, @card10_id, 'DEPOSIT', 300.00, '2026-01-22 11:00:00'),
(@account10_credit_id, @card10_id, 'INQUIRY', 0.00, '2026-01-24 14:00:00'),
(@account10_debit_id, @card10_id, 'TRANSFER', 150.00, '2026-01-26 13:00:00');

-- ============================================
-- Skenaario #11: Lapsi - VIEW_ONLY (DEBIT+CREDIT)
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Lumi', 'Rantanen', 'Esimerkkikatu 10, 90100 Oulu', 'user11@email.fi', '04511011011');
SET @user11_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1101101101101101', '$2b$10$T3Od/3IjB/JRgFGpMzKL3OrEdkqfB1t6qYUOALQRiXxCkZI6rBQ8C', 'CHILD', @user11_id, 'ACTIVE', 0);
SET @card11_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES
(@card11_id, @account10_debit_id, 'VIEW_ONLY'),
(@card11_id, @account10_credit_id, 'VIEW_ONLY');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account10_debit_id, @card11_id, 'INQUIRY', 0.00, '2026-01-19 09:30:00'),
(@account10_credit_id, @card11_id, 'INQUIRY', 0.00, '2026-01-19 09:31:00');

-- ============================================
-- Skenaario #12: Lapsi - VIEW_ONLY (DEBIT) - Vanhemman kaksoiskortti
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Eetu', 'Rantanen', 'Esimerkkikatu 10, 90100 Oulu', 'user12@email.fi', '04512121212');
SET @user12_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1212121212121212', '$2b$10$tJGHFtYkndykxN44oNlKeuC3os7ZwdWfUpOzql7Gw17hDS2wxW9HO', 'CHILD', @user12_id, 'ACTIVE', 0);
SET @card12_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card12_id, @account10_debit_id, 'VIEW_ONLY');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account10_debit_id, @card12_id, 'INQUIRY', 0.00, '2026-01-20 10:00:00'),
(@account10_debit_id, @card12_id, 'INQUIRY', 0.00, '2026-01-22 11:30:00');

-- ============================================
-- Skenaario #13: Lapsi - VIEW_ONLY (CREDIT) - Vanhemman kaksoiskortti
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Aada', 'Rantanen', 'Esimerkkikatu 10, 90100 Oulu', 'user13@email.fi', '04513131313');
SET @user13_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1313131313131313', '$2b$10$GiPfLpIVTnyP95zqY0N2qe0fwUwND3ozBomLOZkhxFfWFrKYSyU4G', 'CHILD', @user13_id, 'ACTIVE', 0);
SET @card13_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card13_id, @account10_credit_id, 'VIEW_ONLY');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account10_credit_id, @card13_id, 'INQUIRY', 0.00, '2026-01-21 14:00:00'),
(@account10_credit_id, @card13_id, 'INQUIRY', 0.00, '2026-01-23 16:00:00');

-- ============================================
-- Skenaario #14: Lapsi - FULL DEBIT + VIEW CREDIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Onni', 'Rantanen', 'Esimerkkikatu 10, 90100 Oulu', 'user14@email.fi', '04514141414');
SET @user14_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1414141414141414', '$2b$10$YitrgtxSCzHPn05TjZaI/eETm8oED5WpfkMrdtdpRh8VBtXnEz11S', 'CHILD', @user14_id, 'ACTIVE', 0);
SET @card14_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES
(@card14_id, @account10_debit_id, 'FULL'),
(@card14_id, @account10_credit_id, 'VIEW_ONLY');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account10_debit_id, @card14_id, 'WITHDRAWAL', 100.00, '2026-01-21 12:00:00'),
(@account10_debit_id, @card14_id, 'DEPOSIT', 50.00, '2026-01-22 10:00:00'),
(@account10_debit_id, @card14_id, 'INQUIRY', 0.00, '2026-01-23 15:30:00'),
(@account10_debit_id, @card14_id, 'TRANSFER', 80.00, '2026-01-24 11:00:00'),
(@account10_credit_id, @card14_id, 'INQUIRY', 0.00, '2026-01-23 15:31:00');

-- ============================================
-- Skenaario #15: Lapsi - FULL CREDIT + VIEW DEBIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Helmi', 'Rantanen', 'Esimerkkikatu 10, 90100 Oulu', 'user15@email.fi', '04515151515');
SET @user15_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1515151515151515', '$2b$10$67jVW5qX/68ScQiGmc6AHeaBZEaelb130qzkDX6a11ZQ7xaq9DJ0i', 'CHILD', @user15_id, 'ACTIVE', 0);
SET @card15_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES
(@card15_id, @account10_credit_id, 'FULL'),
(@card15_id, @account10_debit_id, 'VIEW_ONLY');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account10_credit_id, @card15_id, 'WITHDRAWAL', 250.00, '2026-01-25 14:00:00'),
(@account10_credit_id, @card15_id, 'DEPOSIT', 150.00, '2026-01-26 10:00:00'),
(@account10_credit_id, @card15_id, 'INQUIRY', 0.00, '2026-01-27 12:00:00'),
(@account10_credit_id, @card15_id, 'TRANSFER', 100.00, '2026-01-28 15:00:00'),
(@account10_debit_id, @card15_id, 'INQUIRY', 0.00, '2026-01-27 12:01:00');

-- ============================================
-- Skenaario #16: Lapsi - FULL CREDIT + FULL DEBIT
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Alma', 'Rantanen', 'Esimerkkikatu 10, 90100 Oulu', 'user16@email.fi', '04516161616');
SET @user16_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1616161616161616', '$2b$10$AdtsKPUb7vjfn7gEWkt/JuqPTndhiBc7wcaZQdT113EvoKstyfms6', 'CHILD', @user16_id, 'ACTIVE', 0);
SET @card16_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES
(@card16_id, @account10_debit_id, 'FULL'),
(@card16_id, @account10_credit_id, 'FULL');

INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account10_debit_id, @card16_id, 'WITHDRAWAL', 200.00, '2026-01-24 14:00:00'),
(@account10_debit_id, @card16_id, 'DEPOSIT', 75.00, '2026-01-25 10:00:00'),
(@account10_debit_id, @card16_id, 'INQUIRY', 0.00, '2026-01-26 11:00:00'),
(@account10_debit_id, @card16_id, 'TRANSFER', 120.00, '2026-01-27 13:00:00'),
(@account10_credit_id, @card16_id, 'WITHDRAWAL', 300.00, '2026-01-26 16:30:00'),
(@account10_credit_id, @card16_id, 'DEPOSIT', 200.00, '2026-01-28 11:00:00'),
(@account10_credit_id, @card16_id, 'INQUIRY', 0.00, '2026-01-29 10:00:00'),
(@account10_credit_id, @card16_id, 'TRANSFER', 180.00, '2026-01-29 14:00:00');

-- ============================================
-- Skenaario #17: Suljettu kortti (CLOSED status)
-- ============================================
INSERT INTO user (user_name, user_lastname, user_address, user_email, user_phonenumber)
VALUES ('Antti', 'Laine', 'Esimerkkikatu 17, 90100 Oulu', 'user17@email.fi', '04517171717');
SET @user17_id = LAST_INSERT_ID();

INSERT INTO account (user_id, account_type, balance, credit_limit, account_number)
VALUES (@user17_id, 'DEBIT', 1500.00, NULL, 'FI1717171717171717');
SET @account17_id = LAST_INSERT_ID();

INSERT INTO card (card_number, pin_code, card_type, user_id, status, failed_pin_attempts)
VALUES ('1717171717171717', '$2b$10$92I/nSR90368AT97yvYo0eLxsDam8Oi.5OO2GszTRq7dxgmrCYJpe', 'PARENT', @user17_id, 'CLOSED', 3);
SET @card17_id = LAST_INSERT_ID();

INSERT INTO account_access (card_id, account_id, access_type)
VALUES (@card17_id, @account17_id, 'FULL');

-- Historiallisia transaktioita ennen kortin sulkemista
INSERT INTO transaction (account_id, card_id, transaction_type, amount, transaction_date)
VALUES
(@account17_id, @card17_id, 'DEPOSIT', 2000.00, '2025-12-01 10:00:00'),
(@account17_id, @card17_id, 'WITHDRAWAL', 500.00, '2025-12-15 14:00:00'),
(@account17_id, @card17_id, 'INQUIRY', 0.00, '2025-12-20 11:00:00');

