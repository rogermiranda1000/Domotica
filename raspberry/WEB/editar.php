<!DOCTYPE html>
<html lang="es">
<head>
	<?php include 'files/php/index.php'; ?>
	<link rel="stylesheet" href="files/css/misestilos.css">
</head>
<body>
	<header>
		<?php include 'files/php/arriba.php'; ?>
	</header>
	
	<div id="exterior">
		<div id="modulos">
			<form action="files/form/cambiar.php" method="post">
				<b>Tiempo de envío</b><br><br>
				<select name="cond" required>
					<option value="todos">Todos</option>
					<?php
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
						$sql = "SELECT N.* FROM Nombres as N, Tipos as T WHERE T.ID=N.ID and T.RoA='r' GROUP BY N.Nombre";
						$result = $conn->query($sql);
						if ($result->num_rows > 0) {
							while($row = $result->fetch_assoc()) {
								echo "<option value='".$row["ID"]."'>".$row["Nombre"]." (".$row["Tiempo"]."s)"."</option>";
							}
						}
					?>
				</select> > <input type="number" name="arg" min="1" max="50" required> [s]
				<input type="hidden" name="send" value="true">
				<input type="hidden" name="tabla" value="Nombres">
				<br><br>
				<input type="submit" value="Cambiar">
			</form>
		</div>
		<div id="modulos">
			<form action="files/form/cambiar.php" method="post">
				<b>Tiempo de refresco</b><br><br>
				<?php					
					$sql = "SELECT Valor FROM config WHERE Name='refreshTime';";
					$result = $conn->query($sql);
					if ($result->num_rows > 0) {
						while($row = $result->fetch_assoc()) {
							echo "<input type='number' name='arg' min='1' max='300' value='".$row["Valor"]."' required>";
						}
					}
				?>
				[s]
				<input type="hidden" name="tabla" value="config">
				<input type="hidden" name="cond" value="refreshTime">
				<br><br>
				<input type="submit" value="Cambiar">
			</form>
		</div>
	</div>
	
	<div id="exterior">
		<div id="modulos">
			<form action="files/form/cambiar.php" method="post">
				<b>Cambiar nombre</b><br><br>
				<select name="cond" required>
					<?php
						$sql = "SELECT N.ID,N.Nombre FROM Nombres AS N, Tipos as T WHERE N.ID=T.ID and (T.Roa='r' or T.Tipo='led') GROUP BY N.Nombre;";
						$result = $conn->query($sql);
						if ($result->num_rows > 0) {
							while($row = $result->fetch_assoc()) {
								echo "<option value='".$row["ID"]."'>".$row["Nombre"]." (".$row["ID"].")</option>";
							}
						}
					?>
				</select> > <input type="text" name="arg" maxlength="20" required>
				<input type="hidden" name="tabla" value="Nombres">
				<br><br>
				<input type="submit" value="Cambiar">
			</form>
		</div>
	</div>
		
	<div id="exterior">
		<div id="modulos">
			<form action="files/form/add.php" method="post">
				<b>Añadir botón</b><br><br>
				Nombre: <input type="text" name="nombre" maxlength="20" required><br>
				Comando a ejecutar: <input type="text" name="arg" maxlength="80" required>
				<input type="hidden" name="tabla" value="Botones">
				<br><br>
				<input type="submit" value="Añadir">
			</form>
		</div>
		<div id="modulos">
			<form action="files/form/eliminar.php" method="post">
				<b>Eliminar botón</b><br><br>
					<?php
						$sql = "SELECT Nombre FROM Botones;";
						$result = $conn->query($sql);
						if ($result->num_rows > 0) {
							echo "<select name='nombre' required>";
							while($row = $result->fetch_assoc()) {
								echo "<option value='".$row["Nombre"]."'>".$row["Nombre"]."</option>";
							}
							echo "</select>";
						}
						else echo "No hay botones creados.";
					?>
				<input type="hidden" name="tabla" value="Botones">
				<br><br>
				<input type="submit" value="Eliminar">
			</form>
		</div>
	</div>
	<div id="exterior">
		<div id="modulos">
			<form action="files/form/add.php" method="post">
				<b>Añadir usuario</b><br><br>
				Nombre: <input type="text" name="nombre" maxlength="20" autocomplete="off" required><br>
				Contraseña: <input type="password" name="arg" maxlength="80" autocomplete="off" required>
				<input type="hidden" name="tabla" value="Usuarios">
				<br><br>
				<input type="submit" value="Añadir">
			</form>
		</div>
		<div id="modulos">
			<form action="files/form/eliminar.php" method="post">
				<b>Eliminar usuario</b><br><br>
				<?php
					$sql = "SELECT Nombre FROM Usuarios;";
					$result = $conn->query($sql);
					if ($result->num_rows > 1) {
						echo "<select name='nombre' required>";
						while($row = $result->fetch_assoc()) {
							echo "<option value='".$row["Nombre"]."'>".$row["Nombre"]."</option>";
						}
						echo "</select>";
						echo "<input type='hidden' name='tabla' value='Usuarios'>";
						echo "<br><br>";
						echo "<input type='submit' value='Eliminar'>";
					}
					else echo "Ha de existir mínimo 1 usuario.";
					$conn->close();
				?>
			</form>
		</div>
	</div>
</body>
</html>