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
-- Estructura de tabla para la tabla `Nombres`
--

CREATE TABLE `Nombres` (
  `ID` tinytext NOT NULL,
  `Nombre` tinytext NOT NULL,
  `Tiempo` tinyint(4) NOT NULL
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

CREATE TABLE `Tipos` (
  `ind` int(11) NOT NULL,
  `ID` tinytext NOT NULL,
  `Tipo` tinytext NOT NULL,
  `RoA` varchar(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Usuarios`
--

CREATE TABLE `Usuarios` (
  `Nombre` tinytext NOT NULL,
  `Pass` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Volcado de datos para la tabla `Usuarios`
--

INSERT INTO `Usuarios` (`Nombre`, `Pass`) VALUES
('admin', '21232f297a57a5a743894a0e4a801fc3'); -- usuario admin, contraseña admin

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `Valor`
--

CREATE TABLE `Valor` (
  `ind` int(11) NOT NULL,
  `Tiempo` varchar(1) NOT NULL,
  `Time` tinyint(4) NOT NULL,
  `Val` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Índices para tablas volcadas
--

--
-- Indices de la tabla `Tipos`
--
ALTER TABLE `Tipos`
  ADD PRIMARY KEY (`ind`),
  ADD UNIQUE KEY `ind` (`ind`),
  ADD KEY `ind_2` (`ind`);

--
-- Indices de la tabla `Valor`
--
ALTER TABLE `Valor`
  ADD KEY `ind` (`ind`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
