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
			if($_POST["tabla"]!="Nombres" and $_POST["tabla"]!="config") die("Tabla no aceptada");
			
			$tmp = "Nombre";
			$tmp2 = "ID";
			if(isset($_POST["send"])) {
				$tmp = "Tiempo";
				if($_POST["cond"]=="todos")$tmp2 = "1 or '-'";
			}
			else if($_POST["tabla"]=="config") {
				$tmp = "Valor";
				$tmp2 = "Name";
			}
			
			$stmt = $conn->prepare("UPDATE ".$_POST["tabla"]." SET ".$tmp."=? WHERE ".$tmp2."=?;");
			$stmt->bind_param("ss",$_POST["arg"],$_POST["cond"]);
			$stmt->execute();
			echo "Consulta ejecutada correctamente.";
			
	
			/*$sql = "UPDATE Nombres SET Nombre='".$_POST["nombre"]."' WHERE ID='".$_POST["modulo"]."';";
	
			if(mysqli_query($conn, $sql)){
				echo "Se ha cambiado el nombre de ".$_POST["modulo"]." a ".$_POST["nombre"].".";
			} else {
				echo "Error: " . mysqli_error($conn);
			}*/
	
			$conn->close();
		?>
		<br>
		<a href=" ../../editar.php" class="button">OK</a>
	</center>
</body>
</html>