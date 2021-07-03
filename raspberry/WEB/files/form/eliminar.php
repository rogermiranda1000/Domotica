<html>
<head>
	<?php include '../php/index.php'; ?>
	<style>
		.button {
			background-color: #4CAF50;
			border: none;
			color: white;
			padding: 15px 32px;
			text-align: center;
			text-decoration: none;
			display: inline-block;
			font-size: 16px;
			margin: 4px 2px;
			cursor: pointer;
		}
	</style>
</head>
<body>
	<center>
		<?php
			$servername = "localhost";
			$username = "phpmyadmin";
			$password = "pass";
			$dbname = "Domotica";

			// Create connection
			$conn = new mysqli($servername, $username, $password, $dbname);
			// Check connection
			if ($conn->connect_error) die("Connection failed: " . $conn->connect_error);
			if($_POST["tabla"]!="Botones" and $_POST["tabla"]!="Usuarios") die("Tabla no aceptada");
			
			$stmt = $conn->prepare("DELETE FROM ".$_POST["tabla"]." WHERE Nombre=?;");
			$stmt->bind_param("s",$_POST["nombre"]);
			$stmt->execute();
			echo "Consulta ejecutada correctamente.";
	
			$conn->close();
		?>
		<br>
		<a href=" ../../editar.php" class="button">OK</a>
	</center>
</body>
</html>