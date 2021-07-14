-- phpMyAdmin SQL Dump
-- version 4.6.6deb4+deb9u2
-- https://www.phpmyadmin.net/
--
-- Servidor: localhost:3306
-- Tiempo de generación: 12-07-2021 a las 17:26:35
-- Versión del servidor: 10.1.23-MariaDB-9+deb9u1
-- Versión de PHP: 7.0.33-0+deb9u10

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de datos: `Domotica`
--
DROP DATABASE IF EXISTS Domotica;
CREATE DATABASE Domotica;

USE Domotica;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Alarm`
--

CREATE TABLE `Alarm` (
  `code` int(11) NOT NULL,
  `status` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Alarma`
--

CREATE TABLE `Alarma` (
  `ind` int(11) NOT NULL,
  `maxVal` int(11) NOT NULL,
  `fuego` tinyint(1) NOT NULL,
  `gas` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Botones`
--

CREATE TABLE `Botones` (
  `Nombre` tinytext NOT NULL,
  `Ejecutar` tinytext NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `config`
--

CREATE TABLE `config` (
  `Name` tinytext NOT NULL,
  `Valor` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Volcado de datos para la tabla `config`
--

INSERT INTO `config` (`Name`, `Valor`) VALUES
('refreshTime', 65);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Enchufes`
--

CREATE TABLE `Enchufes` (
  `ind` int(11) NOT NULL,
  `Status` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `LED`
--

CREATE TABLE `LED` (
  `ind` int(11) NOT NULL,
  `Status` tinytext NOT NULL,
  `R` smallint(6) NOT NULL,
  `G` smallint(6) NOT NULL,
  `B` smallint(6) NOT NULL,
  `W` smallint(6) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Riego`
--

CREATE TABLE `Riego` (
  `ID` text NOT NULL,
  `checkRain` tinyint(1) NOT NULL,
  `rangoCheck` int(11) NOT NULL,
  `probCheck` int(11) NOT NULL,
  `checkHumidity` tinyint(1) NOT NULL,
  `humedad` int(11) NOT NULL,
  `forzarRiego` tinyint(1) NOT NULL,
  `vecesPorDia` int(11) NOT NULL,
  `duracionRiego` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `RiegoHora`
--

CREATE TABLE `RiegoHora` (
  `ID` text NOT NULL,
  `Hora` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Tipos`
--
DROP TABLE IF EXISTS Valor;
DROP TABLE IF EXISTS Nombres;

DROP TABLE IF EXISTS Tipos;
CREATE TABLE Tipos (
  ind INTEGER NOT NULL AUTO_INCREMENT,
  ID VARCHAR(255) NOT NULL,
  Tipo VARCHAR(45) NOT NULL,
  RoA ENUM('r', 'a') NOT NULL,
  PRIMARY KEY (ind)
);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Nombres`
--

CREATE TABLE Nombres (
  ind INTEGER NOT NULL,
  Nombre VARCHAR(45) NOT NULL,
  Tiempo TINYINT NOT NULL,
  PRIMARY KEY (ind),
  FOREIGN KEY (ind) REFERENCES Tipos(ind)
);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Valor`
--

CREATE TABLE Valor (
  `ind` INTEGER NOT NULL,
  `Tiempo` ENUM('s', 'm', 'h', 'd') NOT NULL,
  `Time` tinyint(4) NOT NULL,
  `Val` INTEGER,
  `max` INTEGER,
  `min` INTEGER,
  PRIMARY KEY (ind,Tiempo,`Time`),
  FOREIGN KEY (ind) REFERENCES Tipos(ind)
);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Usuarios`
--

DROP TABLE IF EXISTS Usuarios;
CREATE TABLE `Usuarios` (
  `Nombre` VARCHAR(25) NOT NULL,
  `Pass` CHAR(32) NOT NULL,
  PRIMARY KEY (Nombre)
);

--
-- Volcado de datos para la tabla `Usuarios`
--

INSERT INTO `Usuarios` (`Nombre`, `Pass`) VALUES
('admin', MD5('admin')); -- usuario admin, contraseña admin

-- --------------------------------------------------------



-- Procedures
DROP PROCEDURE IF EXISTS updateGeneric;
CREATE PROCEDURE updateGeneric (IN unit ENUM('s', 'm', 'h', 'd'), IN time TINYINT)
BEGIN
    INSERT INTO Valor(ind, Tiempo, Time, Val, max, min)
        SELECT ind, (unit+1), time, AVG(Val) AS mean, GREATEST(MAX(Val), COALESCE(max,-2147483648)), LEAST(Min(Val), COALESCE(min,2147483647))
        FROM Valor
        WHERE Tiempo=unit
        GROUP BY ind;

    DELETE FROM Valor
    WHERE Tiempo=unit;
END;

DROP PROCEDURE IF EXISTS updateSeconds;
CREATE PROCEDURE updateSeconds ()
BEGIN
    CALL updateGeneric('s', MINUTE( DATE_ADD(NOW(), INTERVAL -10 SECOND) )); -- the data it's from the previous minute (not the current one)
END;

DROP PROCEDURE IF EXISTS updateMinutes;
CREATE PROCEDURE updateMinutes ()
BEGIN
    CALL updateGeneric('m', HOUR( DATE_ADD(NOW(), INTERVAL -10 SECOND) )); -- the data it's from the previous hour (not the current one)
END;

DROP PROCEDURE IF EXISTS updateHours;
CREATE PROCEDURE updateHours ()
BEGIN
    CALL updateGeneric('h', DAY( DATE_ADD(NOW(), INTERVAL -10 SECOND) )); -- the data it's from the previous day (not the current one)
END;

DROP PROCEDURE IF EXISTS updateDays;
CREATE PROCEDURE updateDays ()
BEGIN
    -- la base de datos guarda hasta 1 mes; directamente eliminamos los datos
    DELETE FROM Valor
    WHERE Tiempo='d';
END;


-- Schedulers
DROP EVENT IF EXISTS updater;
CREATE EVENT updater
ON SCHEDULE EVERY 1 MINUTE STARTS '2021-07-13 00:00:00'
DO BEGIN
    CALL updateSeconds();

    IF MINUTE(NOW()) = 0 THEN
        CALL updateMinutes();

        IF HOUR(NOW()) = 0 THEN
            CALL updateHours();

            IF DAY(NOW()) = 1 THEN
                CALL updateDays();
            END IF;
        END IF;
    END IF;
END;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
