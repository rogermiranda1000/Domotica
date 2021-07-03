CREATE DATABASE Domotica;
USE Domotica;

CREATE TABLE Tipos (
    id INTEGER AUTO_INCREMENT,
    name VARCHAR(255),
    type VARCHAR(255),
    RoA ENUM('Receptor', 'Actuador'),
    PRIMARY KEY (id)
);

CREATE TABLE Alarm (
    id INTEGER,
    status BOOLEAN,
    PRIMARY KEY (id),
    FOREIGN KEY (id) REFERENCES Tipos(id)
);

CREATE TABLE Alarma (
    id INTEGER,
    maxVal INTEGER,
    fire BOOLEAN,
    gas BOOLEAN,
    PRIMARY KEY (id),
    FOREIGN KEY (id) REFERENCES Tipos(id)
);

CREATE TABLE Botones (
    name VARCHAR(255),
    execute VARCHAR(255),
    PRIMARY KEY (name)
);

CREATE TABLE Config (
    name VARCHAR(255),
    value INTEGER,
    PRIMARY KEY (name)
);

CREATE TABLE Enchufes (
    id INTEGER,
    status BOOLEAN,
    PRIMARY KEY (id)
);

CREATE TABLE LED (
    id INTEGER,
    status VARCHAR(255),
    R SMALLINT,
    G SMALLINT,
    B SMALLINT,
    W SMALLINT,
    PRIMARY KEY (id)
);

CREATE TABLE Nombres (
    id VARCHAR(255),
    name VARCHAR(255),
    time INTEGER,
    PRIMARY KEY (id)
);

CREATE TABLE Riego (
    id INTEGER,
    checkRain BOOLEAN,
    checkRange INTEGER,
    checkProbability INTEGER,
    checkHumidity BOOLEAN,
    humidity INTEGER,
    forceWater BOOLEAN,
    timesPerDay TINYINT,
    duration INTEGER,
    PRIMARY KEY (id)
);

CREATE TABLE RiegoHora (
    id INTEGER,
    hour TINYINT,
    PRIMARY KEY (id)
);

CREATE TABLE Valor (
    id INTEGER,
    tiempo VARCHAR(1),
    time SMALLINT,
    val INTEGER,
    PRIMARY KEY (id, tiempo, time),
    FOREIGN KEY (id) REFERENCES Tipos(id)
);

CREATE TABLE Users (
    name VARCHAR(255),
    password CHAR(32),
    PRIMARY KEY (name)
);