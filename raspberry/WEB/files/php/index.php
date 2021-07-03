<title>Casa domótica</title>
<meta charset="utf-8">
<!--Header-->
<link rel="stylesheet" href="files/css/barra.css">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
<link rel="icon" type="image/png" href="domotica-ico.png" />

<?php
	function formulario() {
		echo "<center><br>";
		echo "<form action='login.php' method='post'>";
		echo "<b>Usuario: </b> <input type='text' name='nombre' maxlength='20' required><br>";
		echo "<b>Contraseña: </b> <input type='password' name='pass' maxlength='20' required><br><br>";
		echo "<input type='submit' value='Entrar'>";
		echo "</form>";
		echo "</center>";
		die();
	}
	
	session_start();
	if(!isset($_SESSION["Nombre"])) formulario();
	
	$servername = "localhost";
	$username = "phpmyadmin";
	$password = "pass";
	$dbname = "Domotica";

	// Create connection
	$conn = new mysqli($servername, $username, $password, $dbname);
	// Check connection
	if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
	}
	
	//$done = false;
	
	$sql = $conn->prepare("SELECT Pass FROM Usuarios WHERE Nombre = ?;");
    $sql->bind_param("s",$_SESSION["Nombre"]);
	$sql->execute();
	$sql->bind_result($Pass);
	while ($sql->fetch()) {
		$sql->close();
		$conn->close();
		if(md5($_SESSION["Pass"])!=$Pass) {
			echo "<center><br>Usuario o contraseña errónea.</center>";
			formulario();
		}
		return;
	}
	$sql->close();
	$conn->close();
	
	echo "<center><br>Usuario o contraseña errónea.</center>";
	formulario();
?>