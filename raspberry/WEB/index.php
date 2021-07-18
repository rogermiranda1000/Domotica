<!DOCTYPE html>
<html lang="es">
<head>
	<?php include 'files/php/index.php'; ?>
	<link rel="stylesheet" href="files/css/misestilos.css">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.js" type="text/javascript"></script>
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
		$sql = "SELECT Valor FROM config WHERE Name='refreshTime';";
		$result = $conn->query($sql);
		if ($result->num_rows > 0) {
			while($row = $result->fetch_assoc()) {
				echo "<META HTTP-EQUIV=Refresh CONTENT='". $row["Valor"] . "'>";
			}
		}
	?>
</head>
<body>
	<header>
		<?php include 'files/php/arriba.php'; ?>
	</header>
	
	<?php
		$x = 0;
		
		$modulos = $conn->query("SELECT Nombres.ID, Nombres.Nombre FROM Nombres;");
		if (($modulos->num_rows > 0)) {
			while($row_modulos = $modulos->fetch_assoc()) {
				if($x % 2 == 0) echo "<div id='exterior'>";
				echo "<div id='modulos'>";
				echo "<b><u>".htmlspecialchars($row_modulos["Nombre"])."</u></b>";
				echo "<br>";
				
				$data = $conn->prepare("SELECT Tipos.ind, Tipos.Tipo, Tipos.RoA, (SELECT Valor.Val FROM Valor WHERE Valor.ind = Tipos.ind ORDER BY Valor.Tiempo, Valor.Time DESC LIMIT 1) AS LastValue FROM Tipos WHERE Tipos.ID = ?");
				$data->bind_param("s", $row_modulos["ID"]);
				$data->execute();
				$data_result = $data->get_result();
				while ($row_data = $data_result->fetch_assoc()) {
					if($row_data["RoA"]=='r' && ($row_data["Tipo"]!="humedadPlanta1" && $row_data["Tipo"]!="alarma")) {
						// sensor
						$unidades = "";
						if($row_data["Tipo"]==="temperatura") $unidades = '°C';
						else if($row_data["Tipo"]==="humedad") $unidades = '%';
						else if($row_data["Tipo"]==="presion") $unidades = 'hPa';
						else if($row_data["Tipo"]==="luz") $unidades = '%';
						else if($row_data["Tipo"]==="agua") $unidades = 'mm/h';
						else if($row_data["Tipo"]==="viento") $unidades = 'km/h';
						// direccion no tiene unidades
						
						$value = number_format((float)$row_data["LastValue"], 2, '.', '');
						if ($row_data["Tipo"]==="direccion") {
							// ADC to direction
							if ($value < 380) $value = 'ESE'; //113;
							else if ($value < 393) $value = 'ENE'; //68;
							else if ($value < 414) $value = 'E'; //90;
							else if ($value < 456) $value = 'SSE'; //158;
							else if ($value < 508) $value = 'SE'; //135;
							else if ($value < 551) $value = 'SSO'; //203;
							else if ($value < 615) $value = 'S'; //180;
							else if ($value < 680) $value = 'NNE'; //23;
							else if ($value < 746) $value = 'NE'; //45;
							else if ($value < 801) $value = 'OSO'; //248;
							else if ($value < 833) $value = 'SO'; //225;
							else if ($value < 878) $value = 'NNO'; //338;
							else if ($value < 913) $value = 'N'; //0;
							else if ($value < 940) $value = 'ONO'; //293;
							else if ($value < 967) $value = 'NO'; //315;
							else if ($value < 990) $value = 'O'; //270;
							// en principio nunca saldrá de este rango (se comproba en el código de Arduino)
						}
						
						echo ucfirst($row_data["Tipo"]).": ".$value.$unidades."<br>";
					}
					else {
						if($row_data["Tipo"]=="enchufe") {
							$sql3 = "SELECT Status FROM Enchufes WHERE ind=".$row_data["ind"].";";
							$result3 = $conn->query($sql3);
							$status = "";
							if ($result3->num_rows > 0) {
								while($row3 = $result3->fetch_assoc()) {
									if($row3["Status"]==1) $status = "Apagar";
									else $status = "Encender";
								}
								echo "<button onclick='inte(\"".$row_data["ID"]."\")'>".$status."</button>";
							}
						}
						else if($row_data["Tipo"]=="alarma") {
							$sql3 = "SELECT status FROM Alarm WHERE 1;";
							$result3 = $conn->query($sql3);
							$status = "";
							if ($result3->num_rows > 0) {
								while($row3 = $result3->fetch_assoc()) {
									if($row3["status"]==1) $status = "Apagar";
									else $status = "Encender";
									echo "<button onclick='alarm(\"".$row_data["ID"]."\",".$row3["status"].")'>".$status." alarma</button>";
								}
							}
						}
						else if($row_data["Tipo"]=="gas") {
							$sql3 = "SELECT status,fuego,gas FROM Alarma WHERE ind=".$row_data["ind"].";";
							$result3 = $conn->query($sql3);
							$status = False;
							if ($result3->num_rows > 0) {
								while($row3 = $result3->fetch_assoc()) {
									if(($row3["fuego"]==1 || $row3["gas"]==1) && !($_GET["ID"]==$row_modulos["ID"] && $_GET["stat"]=="apagarAlarma")) $status = True;
									if(!($status)) echo 'Niveles correctos.';
									else echo "<button onclick='alarma(\"".$row_modulos["ID"]."\",".$row_data["ind"].")'>Apagar alarma</button>";
									echo "<br>";
									$v = $row3["maxVal"];
									if($_GET["ID"]==$row_modulos["ID"] && $_GET["stat"]=="setVal") $v = $_GET["val"];
									echo "Límite: <input type='range' id='lim".$row_modulos["ID"]."' min='5' max='1023' value='".$v."' onchange='alChange(\"".$row_modulos['ID']."\",".$row_data["ind"].")'>";
								}
							}
						}
						//Riego
						else if($row_data["Tipo"]=="humedadPlanta1") {
							$sql2 = "SELECT * FROM Riego WHERE ID=\"".$row_modulos["ID"]."\";";
							$result2 = $conn->query($sql2);
							if ($result2->num_rows > 0) {
								while($row2 = $result2->fetch_assoc()) {
									//Lluvia
									echo '<input id="'.$row_modulos["ID"].'R" type="checkbox" onchange="riego(\''.$row_modulos["ID"].'\', \'checkRain\')"';
									if(($row2["checkRain"]==1 || ($_GET["stat"]=="checkRain" && $_GET["val"]=="true")) && (($_GET["stat"]=="checkRain" && $_GET["val"]!="false") || $_GET["stat"]!="checkRain")) {
										//True
										echo ' checked';
										echo '>Considerar la lluvia';
										echo '<br>';
										
										echo 'En un rango de ';
										
										$seleccionado = $row2["rangoCheck"];
										if($_GET["rangoCheck"]!="" && $_GET["ID"]==$row_modulos['ID']) $seleccionado = $_GET["rangoCheck"];
										echo '<select id="rangoCheck'.$row_modulos['ID'].'" onchange="riego(\''.$row_modulos['ID'].'\', \'rangoCheck\')">';
										echo '<option value="" disabled="disabled">Rango:';
										for($i=0; $i<48; $i++) {
											echo '<option value="'.($i+1).'"';
											if($seleccionado==$i+1) echo ' selected="selected"';
											echo '>'.($i+1);
										}
										echo '</select>';
										
										echo ' horas,';
										echo '<br>';
										echo 'con una provabilidad del ';
										
										$seleccionado = $row2["probCheck"];
										if($_GET["probCheck"]!="" && $_GET["ID"]==$row_modulos['ID']) $seleccionado = $_GET["probCheck"];
										echo '<select id="probCheck'.$row_modulos['ID'].'" onchange="riego(\''.$row_modulos['ID'].'\', \'probCheck\')">';
										echo '<option value="" disabled="disabled">%:';
										for($i=1; $i<=9; $i++) {
											$a=$i*10;
											echo '<option value="'.$a.'"';
											if($seleccionado==$a) echo ' selected="selected"';
											echo '>'.$a;
										}
										echo '</select>';
										
										echo '%.';
									}
									else {
										echo '>Considerar la lluvia';
									}
									echo '<br>';
									
									//Humedad
									echo '<input id="'.$row_modulos["ID"].'H" type="checkbox" onchange="riego(\''.$row_modulos["ID"].'\', \'checkHumidity\')"';
									if(($row2["checkHumidity"]==1 || ($_GET["stat"]=="checkHumidity" && $_GET["val"]=="true"))&&(($_GET["stat"]=="checkHumidity"&&$_GET["val"]!="false") || $_GET["stat"]!="checkHumidity")) {
										//True
										echo ' checked';
										echo '>Considerar la humedad de la tierra';
												echo '<br>';
												echo 'Regar si la humedad es inferior al ';
												
												$seleccionado = "";
												$sql3 = "SELECT humedad FROM Riego WHERE ID=\"".$row_modulos['ID']."\";";
												$result3 = $conn->query($sql3);
												if ($result3->num_rows > 0) {
													while($row3 = $result3->fetch_assoc()) {
														$seleccionado = $row3["humedad"];
													}
												}
												if($_GET["humedad"]!="" && $_GET["ID"]==$row_modulos['ID']) $seleccionado = $_GET["humedad"];
												echo '<select id="humedad'.$row_modulos['ID'].'" onchange="riego(\''.$row_modulos['ID'].'\', \'humedad\')">';
												echo '<option value="" disabled="disabled">%:';
												for($i=1; $i<=9; $i++) {
													$a=$i*10;
													echo '<option value="'.$a.'"';
													if($seleccionado==$a) echo ' selected="selected"';
													echo '>'.$a;
												}
												echo '</select>';
												
												echo '%.';
											}
											else {
												echo '>Considerar la humedad de la tierra';
											}
											echo '<br>';
											echo "<button onclick='regar(\"".$row_modulos["ID"]."\")'>Regar durante 2 minutos</button>";
											echo '<br>';
											
											//Forzar riego
											/*echo '<input id="'.$row_modulos["ID"].'W" type="checkbox" onchange="riego(\''.$row_modulos["ID"].'\', \'checkWater\')"';
											if(($row2["forzarRiego"]==1 || ($_GET["stat"]=="checkWater" && $_GET["val"]=="true"))&&(($_GET["stat"]=="checkWater"&&$_GET["val"]!="false") || $_GET["stat"]!="checkWater")) {*/
												//True
												/*echo ' checked';
												echo '>Forzar riego';
												echo '<br>';
												
												$seleccionado = "";
												$sql3 = "SELECT vecesPorDia FROM Riego WHERE ID=\"".$row_modulos['ID']."\";";
												$result3 = $conn->query($sql3);
												if ($result3->num_rows > 0) {
													while($row3 = $result3->fetch_assoc()) {
														$seleccionado = $row3["vecesPorDia"];
													}
												}
												if($_GET["vecesPorDia"]!="" && $_GET["ID"]==$row_modulos['ID']) $seleccionado = $_GET["vecesPorDia"];
												echo '<select id="vecesPorDia'.$row_modulos['ID'].'" onchange="riego(\''.$row_modulos['ID'].'\', \'vecesPorDia\')">';
												echo '<option value="" disabled="disabled">Veces/día:';
												for($i=1; $i<=4; $i++) {
													echo '<option value="'.$i.'"';
													if($seleccionado==$i) echo ' selected="selected"';
													echo '>'.$i;
												}
												echo '</select>';
												
												echo ' veces al día,';
												echo '<br>';
												echo 'durante ';
												
												$seleccionado = "";
												$sql3 = "SELECT duracionRiego FROM Riego WHERE ID=\"".$row_modulos['ID']."\";";
												$result3 = $conn->query($sql3);
												if ($result3->num_rows > 0) {
													while($row3 = $result3->fetch_assoc()) {
														$seleccionado = $row3["duracionRiego"];
													}
												}
												if($_GET["duracionRiego"]!="" && $_GET["ID"]==$row_modulos['ID']) $seleccionado = $_GET["duracionRiego"];
												echo '<select id="duracionRiego'.$row_modulos['ID'].'" onchange="riego(\''.$row_modulos['ID'].'\', \'duracionRiego\')">';
												echo '<option value="" disabled="disabled">Minutos:';
												for($i=1; $i<=6; $i++) {
													$o = $i*5;
													echo '<option value="'.$o.'"';
													if($seleccionado==$o) echo ' selected="selected"';
													echo '>'.$o;
												}
												echo '</select>';
												
												echo ' minutos,';
												echo '<br>';
												echo 'A las:';
												echo '<br>';
												
												$seleccionado = "";
												$sql3 = "SELECT vecesPorDia FROM Riego WHERE ID=\"".$row_modulos['ID']."\";";
												$result3 = $conn->query($sql3);
												if ($result3->num_rows > 0) {
													while($row3 = $result3->fetch_assoc()) {
														$seleccionado = $row3["vecesPorDia"];
													}
												}
												if($_GET["vecesPorDia"]!="" && $_GET["ID"]==$row_modulos['ID']) $seleccionado = $_GET["vecesPorDia"];
												for($i=0; $i<intval($seleccionado); $i++) {
													echo "> ";
													echo "<br>";
												}*/
											/*}
											else echo '>Forzar riego';
											echo '<br>';*/
										}
									}
						}
						else if($row_data["Tipo"]=="led") {
							echo '<select id="LEDStatus '.$row_modulos['ID'].'" onchange="led(\''.$row_modulos['ID'].'\', \''.$row_data["ind"].'\')">';
							
							$seleccionado = "";
							$sql3 = "SELECT Status FROM LED WHERE ind=".$row_data["ind"].";";
							$result3 = $conn->query($sql3);
							if ($result3->num_rows > 0) {
								while($row3 = $result3->fetch_assoc()) {
									$seleccionado = $row3["Status"];
								}
							}
							if($_GET["modo"]!="" && $_GET["ind"]==$row_data["ind"]) $seleccionado = $_GET["modo"];
							
							echo '<option value="" disabled="disabled">Modo:';
							if($seleccionado=="simp") echo '<option value="simp" selected="selected">Simple';
							else echo '<option value="simp">Simple';
							if($seleccionado=="anim") echo '<option value="anim" selected="selected">Animación';
							else echo '<option value="anim">Animación';
							if($seleccionado=="avan") echo '<option value="avan" selected="selected">Avanzado';
							else echo '<option value="avan">Avanzado';
							echo '</select>';
							
							
							if($seleccionado=="simp") echo '<div style="display: inline;" id="simp">';
							else echo '<div style="display: none;" id="simp">';
							echo "<br><br>";
							echo "<button onclick='ledB(\"".$row_modulos["ID"]."\",\"on\", \"\", 0)'>Encender</button>";
							echo "<br>";
							echo "<button onclick='ledB(\"".$row_modulos["ID"]."\",\"off\", \"\", 0)'>Apagar</button>";
							echo "</div>";
							if($seleccionado=="anim") echo '<div style="display: inline;" id="anim">';
							else echo '<div style="display: none;" id="anim">';
							echo "<br><br>";
							echo "<button onclick='ledB(\"".$row_modulos["ID"]."\",\"anim\", \"\", 0)'>Iniciar</button>";
							echo "<br>";
							echo "<button onclick='ledB(\"".$row_modulos["ID"]."\",\"animS\", \"\", 0)'>Detener</button>";
							echo "</div>";
							if($seleccionado=="avan") echo '<div style="display: inline;" id="avan">';
							else echo '<div style="display: none;" id="avan">';
							echo "<br>";
							
							$r=0;
							$g=0;
							$b=0;
							$w=0;
							$sql3 = "SELECT R,G,B,W FROM LED WHERE ind=".$row_data["ind"].";";
							$result3 = $conn->query($sql3);
							if ($result3->num_rows > 0) {
								while($row3 = $result3->fetch_assoc()) {
									if($_GET["color"]!="R") $r = $row3["R"];
									else $r = $_GET["valor"];
									if($_GET["color"]!="G") $g = $row3["G"];
									else $g = $_GET["valor"];
									if($_GET["color"]!="B") $b = $row3["B"];
									else $b = $_GET["valor"];
									if($_GET["color"]!="W") $w = $row3["W"];
									else $w = $_GET["valor"];
								}
							}
							
							echo "R: <input type='range' id='R' min='0' max='255' value='".$r."' onchange='ledB(\"".$row_modulos['ID']."\",\"set\",\"red\",".$row_data["ind"].")'>";
							echo "<br>";
							echo "G: <input type='range' id='G' min='0' max='255' value='".$g."' onchange='ledB(\"".$row_modulos['ID']."\",\"set\",\"green\",".$row_data["ind"].")'>";
							echo "<br>";
							echo "B: <input type='range' id='B' min='0' max='255' value='".$b."' onchange='ledB(\"".$row_modulos['ID']."\",\"set\",\"blue\",".$row_data["ind"].")'>";
							echo "<br>";
							echo "W: <input type='range' id='W' min='0' max='255' value='".$w."' onchange='ledB(\"".$row_modulos['ID']."\",\"set\",\"white\",".$row_data["ind"].")'>";
							echo "</div>";
						}
					}
				}
			
				echo "</div>";
				if($x % 2 == 1) echo "</div>";
				$x++;
			}
		}
		
		$sql2 = "SELECT * FROM Botones;";
		$result2 = $conn->query($sql2);
		if ($result2->num_rows > 0) {
			while($row = $result2->fetch_assoc()) {
				if($x % 2 == 0) echo "<div id='exterior'>";
				echo "<div id='modulos'>";
				echo "<b><u>".htmlspecialchars($row["Nombre"])."</u></b>";
				echo "<br>";
				echo "<button onclick='exe(\"".htmlspecialchars($row["Nombre"])."\")'>Ejecutar '".htmlspecialchars($row["Ejecutar"])."'</button>";
				
				echo "</div>";
				if($x % 2 == 1) echo "</div>";
				$x++;
			}
		}
		if($x % 2 == 1) echo "</div>";
		
		$conn->close();
	?>
	
	<script type="text/javascript">
		var clientID = "ID-" + Math.round(Math.random() * 1000);
		var client = new Paho.MQTT.Client(window.location.host, 9001, clientID);
		console.log(window.location.host);

		client.connect({onSuccess:onConnect});

		function onConnect() {
			console.log("connected");
			client.subscribe(clientID);
		}
		function exe(txt) {
			var message = new Paho.MQTT.Message(clientID+" btn "+txt);
			message.destinationName = "central";
			client.send(message);
		}
		function inte(txt) {
			var message = new Paho.MQTT.Message(clientID+" int "+txt);
			message.destinationName = "central";
			client.send(message);
			
			location.reload();
		}
		function regar(ID) {
			var message = new Paho.MQTT.Message(clientID+" reg "+ID);
			message.destinationName = "central";
			client.send(message);
		}
		
		function alarma(ID, ind) {
			var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + '?ID='+ID+'&stat=apagarAlarma&ind='+ind;
			window.history.pushState({path:newurl},'',newurl);
			
			<?php
					// Create connection
					$conn = new mysqli($servername, $username, $password, $dbname);
					// Check connection
					if ($conn->connect_error) {
						die("Connection failed: " . $conn->connect_error);
					}
					
					$stt = $_GET["stat"];
					if($stt=="checkWater") $stt = "forzarRiego";
					$sql = "UPDATE Alarma SET fuego=0,gas=0 WHERE ind=".$_GET["ind"].";";
					if(mysqli_query($conn, $sql)) echo "/*Sí*/";
					else echo "/*No*/";
					
					$conn->close();
				?>
				
				var message = new Paho.MQTT.Message("deact");
				message.destinationName = ID;
				client.send(message);
				
				location.reload();
		}
		
		function alChange(ID, ind) {
			var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + '?ID='+ID+'&stat=setVal&val='+document.getElementById("lim"+ID).value+'&ind='+ind;
			window.history.pushState({path:newurl},'',newurl);
			
			<?php
				// Create connection
				$conn = new mysqli($servername, $username, $password, $dbname);
				// Check connection
				if ($conn->connect_error) {
					die("Connection failed: " . $conn->connect_error);
				}
				
				$stt = $_GET["stat"];
				if($stt=="checkWater") $stt = "forzarRiego";
				$sql = "UPDATE Alarma SET maxVal=".$_GET["val"]." WHERE ind=".$_GET["ind"].";";
				if(mysqli_query($conn, $sql)) echo "/*Sí*/";
				else echo "/*No*/";
				
				$conn->close();
			?>
			
			location.reload();
		}
		
		function riego(ID, btn) {
			if(btn=="checkRain" || btn=="checkHumidity" || btn=="checkWater") {
				var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + '?ID='+ID+'&stat='+btn+'&val='+document.getElementById(ID+btn[5]).checked;
				window.history.pushState({path:newurl},'',newurl);
				<?php
					// Create connection
					$conn = new mysqli($servername, $username, $password, $dbname);
					// Check connection
					if ($conn->connect_error) {
						die("Connection failed: " . $conn->connect_error);
					}
					
					$stt = $_GET["stat"];
					if($stt=="checkWater") $stt = "forzarRiego";
					$sql = "UPDATE Riego SET ".$stt."=".$_GET["val"]." WHERE ID=\"".$_GET["ID"]."\";";
					if(mysqli_query($conn, $sql)) echo "/*Sí*/";
					else echo "/*No*/";
					
					$conn->close();
				?>
				
				location.reload();
			}
			else if(btn=="probCheck"  || btn=="rangoCheck" || btn=="humedad" || btn=="vecesPorDia" || btn=="duracionRiego") {
				var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + '?ID='+ID+'&btn='+btn+'&'+btn+'='+document.getElementById(btn+ID).value;
				window.history.pushState({path:newurl},'',newurl);
				<?php
					// Create connection
					$conn = new mysqli($servername, $username, $password, $dbname);
					// Check connection
					if ($conn->connect_error) {
						die("Connection failed: " . $conn->connect_error);
					}
					
					$sql = "UPDATE Riego SET ".$_GET["btn"]."=".$_GET[$_GET["btn"]]." WHERE ID=\"".$_GET["ID"]."\";";
					if(mysqli_query($conn, $sql)) echo "/*Sí*/";
					else echo "/*No*/";
					
					$conn->close();
				?>
				
				location.reload();
			}
		}
		
		function alarm(ID, stat) {
			var msg = "deact";
			if(stat==0) msg = "act";
			var message = new Paho.MQTT.Message(ID+" "+msg);
			message.destinationName = "central";
			client.send(message);
			
			location.reload();
		}
		
		function led(ID, ind) {
			var g = document.getElementById("LEDStatus "+ID).value;
			var d1 = document.getElementById('simp');
			var d2 = document.getElementById('anim');
			var d3 = document.getElementById('avan');
			ledB(ID, "animS", "", 0);
			
			if(g=="simp") {
				d1.style.display = 'inline';
				d2.style.display = 'none';
				d3.style.display = 'none';
			}
			else if(g=="anim") {
				d1.style.display = 'none';
				d2.style.display = 'inline';
				d3.style.display = 'none';
			}
			else if(g=="avan") {
				d1.style.display = 'none';
				d2.style.display = 'none';
				d3.style.display = 'inline';
			}
			
			
			var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + '?ind='+ind+'&modo='+g;
			window.history.pushState({path:newurl},'',newurl);
			<?php
				// Create connection
				$conn = new mysqli($servername, $username, $password, $dbname);
				// Check connection
				if ($conn->connect_error) {
					die("Connection failed: " . $conn->connect_error);
				}
				
				$sql = "UPDATE LED SET Status=\"".$_GET["modo"]."\" WHERE ind=".$_GET["ind"].";";
				if(mysqli_query($conn, $sql)) echo "/*Sí*/";
				else echo "/*No*/";
				
				$conn->close();
			?>
			location.reload();
		}
		
		function ledB(ID, sel, color, ind) {
			var msg = sel;
			if(sel=="set") {
				var valor = 0;
				if(color=="red") valor = document.getElementById('R').value;
				else if(color=="green") valor = document.getElementById('G').value;
				else if(color=="blue") valor = document.getElementById('B').value;
				else if(color=="white") valor = document.getElementById('W').value;
				
				msg += (" " + color + " " + valor);
			}
			
			var message = new Paho.MQTT.Message(msg);
			message.destinationName = ID;
			client.send(message);
			
			if(sel=="set") {
				var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + '?ind='+ind+'&color='+color[0].toUpperCase()+'&valor='+valor;
				window.history.pushState({path:newurl},'',newurl);
				<?php
					if($_GET["color"]!="") {
						// Create connection
						$conn = new mysqli($servername, $username, $password, $dbname);
						// Check connection
						if ($conn->connect_error) {
							die("Connection failed: " . $conn->connect_error);
						}
						
						$sql = "UPDATE LED SET Status=\"avan\",".$_GET["color"]."=\"".$_GET["valor"]."\" WHERE ind=".$_GET["ind"].";";
						if(mysqli_query($conn, $sql)) echo "/*Sí*/";
						else echo "/*No*/";
						
						$conn->close();
					}
				?>
				location.reload();
			}
		}
    </script>
	
	<footer>
		Última actualización: <?php echo date("h:i:sa");?>
	</footer>
</body>
</html>