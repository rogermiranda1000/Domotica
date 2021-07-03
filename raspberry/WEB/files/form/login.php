<?php
	session_start();
	$_SESSION["Nombre"] = $_POST["nombre"];
	$_SESSION["Pass"] = $_POST["pass"];
	echo "<script> history.go(-1); </script>";
?>