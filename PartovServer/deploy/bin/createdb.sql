CREATE DATABASE IF NOT EXISTS PartovDB DEFAULT CHARACTER SET 'utf8';
CREATE USER 'databaseusername'@'localhost' IDENTIFIED BY 'databasepassword';
GRANT ALL ON PartovDB.* TO 'databaseusername'@'localhost' IDENTIFIED BY 'databasepassword';

USE PartovDB;

CREATE TABLE IF NOT EXISTS User(firstName CHAR(20), lastName CHAR(20),
		userName CHAR(20) NOT NULL PRIMARY KEY, password CHAR(32) NOT NULL,
		active INT NOT NULL, groupName CHAR(20) NOT NULL, email CHAR(64) NOT NULL);

INSERT INTO User VALUES('Behnam', 'Momeni', 'momeni', md5('some-password'), 1, 'admin', 'b_momeni [AT] ce [Dot] sharif [Dot] edu');

