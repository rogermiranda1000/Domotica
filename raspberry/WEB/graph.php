<!DOCTYPE html>
<html lang="es">
<head>
    <?php include 'files/php/index.php'; ?>
    <script src="https://code.highcharts.com/highcharts.js"></script>
    <script src="https://code.highcharts.com/modules/exporting.js"></script>
    <script src="https://code.highcharts.com/modules/export-data.js"></script>
    <style>
        .highcharts-data-table {
            display: none;
        }
        .highcharts-credits {
            display: none;
        }
        
        select {
            margin: auto;
            margin-left: auto;
            margin-right: auto;
            padding: 4px;
            width: 45%;
            margin: 1%;
        }
        @media (max-width: 600px) {
            select {
                width: 95%;
                margin-top: 2%;
            }
        }
    </style>
</head>
<body>
    <header>
        <?php include 'files/php/arriba.php'; ?>
    </header>
    
    <select id="mySelect" onchange="myFunction()">
        <option value="" selected="selected" disabled="disabled">--Seleccione una gráfica--</option>
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
            $sql = "SELECT Tipo FROM Tipos WHERE RoA='r' GROUP BY Tipo;";
            $result = $conn->query($sql);
            if ($result->num_rows > 0) {
                while($row = $result->fetch_assoc()) {
					if($row["Tipo"] == "humedadPlanta1") {
						echo "<option value='Humedad'";
						if ($_GET['g'] === 'Humedad') echo " selected";
						echo ">Humedad</option>";
					}
					else if($row["Tipo"] != "humedadPlanta2" && $row["Tipo"] != "humedadPlanta3" && $row["Tipo"] != "humedadPlanta4") {
						echo "<option value='".ucfirst($row["Tipo"])."'";
						if ($_GET['g'] === ucfirst($row["Tipo"])) echo " selected";
						echo ">".ucfirst($row["Tipo"])."</option>";
					}
                }
            }
        ?>
    </select>

    <select id="mySelectT" onchange="myFunction()">
        <option value="" <?php if ($_GET['t'] !== 's' && $_GET['t'] !== 'm' && $_GET['t'] !== 'h' && $_GET['t'] !== 'd') echo 'selected'; ?> disabled="disabled">--Seleccione un periodo de tiempo--
        <option value="s" <?php if ($_GET['t'] === 's') echo 'selected'; ?>>60 s
        <option value="m" <?php if ($_GET['t'] === 'm') echo 'selected'; ?>>60 min
        <option value="h" <?php if ($_GET['t'] === 'h') echo 'selected'; ?>>24 h
        <option value="d" <?php if ($_GET['t'] === 'd') echo 'selected'; ?>>30 d
    </select>
    
    <div id="container" style="min-width: 310px; height: 400px; margin: 0 auto; display: none;"></div>
    <?php include 'files/php/GG.php'; ?>
    <script>
        <?php if($_GET['g']=="" && $_GET['t']=="") {echo"</script></body></html>"; die;} ?>
        document.getElementById("container").style.display = "block";
        var unidades = "?";
        if(<?php echo "'".$_GET['g']."'"; ?>=="Temperatura") unidades = '°C';
        else if(<?php echo "'".$_GET['g']."'"; ?>=="Humedad") unidades = '%';
        else if(<?php echo "'".$_GET['g']."'"; ?>=="Presion") unidades = 'hPa';
        else if(<?php echo "'".$_GET['g']."'"; ?>=="Luz") unidades = '%';
        else if(<?php echo "'".$_GET['g']."'"; ?>=="Agua") unidades = 'mm/h';
        
        <?php
            $sql = "SELECT ind,ID FROM Tipos WHERE Tipo=\"".strtolower($_GET['g'])."\";";
			if($_GET['g']=="Humedad") $sql = "SELECT ind,ID FROM Tipos WHERE Tipo='humedadPlanta1' OR Tipo='humedadPlanta2' OR Tipo='humedadPlanta3' OR Tipo='humedadPlanta4' OR Tipo='humedad';";
            $result = $conn->query($sql);
            $ind = array();
            $IDs = array();
            if ($result->num_rows > 0) {
                while($row = $result->fetch_assoc()) {
                    array_push($ind, $row["ind"]);
                    array_push($IDs, $row["ID"]);
                }
            }
            
            $nombre = array();
            foreach($IDs as $x) {
                $sql = "SELECT Nombre FROM Nombres WHERE ID=\"".$x."\";";
                $result = $conn->query($sql);
                while($row = $result->fetch_assoc()) {
                    array_push($nombre, $row["Nombre"]);
                }
            }
            
            $TEXTO = array();
			foreach($ind as $x) {
				foreach(array('Val','max','min') as $checking) {
					$valores = "";
					$index = array_search($x, $ind);
					$sql = "SELECT Time,".$checking." FROM Valor WHERE ind=\"".$x."\" AND Tiempo=\"".$_GET['t']."\" ORDER BY Valor.Time ASC;";
					$result = $conn->query($sql);
					if ($result->num_rows > 0) {
						while($row = $result->fetch_assoc()) {
							if ($row[$checking] !== NULL) $valores .= "[".$row["Time"].",".$row[$checking]."], ";
						}
					}
					
					if (strlen($valores) > 0) {
						$valores = substr($valores, 0, -2); // subtract last ', '
						$json = "{ name: '".$nombre[$index];
						if ($checking !== 'Val') $json .= " (".$checking.")";
						$json .= "', data: [".$valores."] }";
						array_push($TEXTO, $json);
					}
				}
			}
            $conn->close();
        ?>

Highcharts.chart('container', {
  chart: {
    type: 'line'
  },
  title: {
    text: <?php echo "'".$_GET['g']."'"; ?>
  },
  yAxis: {
    title: {
      text: <?php echo "'".$_GET['g']."'"; ?>+' ('+unidades+')'
    }
  },
  xAxis: {
	min: <?php echo (($_GET['t']=="d") ? 1 : 0); ?>,
	max: <?php 
		$tmp = null;
		if($_GET['t']=="s" || $_GET['t']=="m") $tmp = 59;
		else if($_GET['t']=="h") $tmp = 23;
		else if($_GET['t']=="d") $tmp = 31;
		
		echo $tmp;
	?>,
    title: {
      text: 'Tiempo ['+<?php echo "'".$_GET['t']."'"; ?>+']'
    }
  },
  plotOptions: { 
    line: {
      dataLabels: {
        enabled: true
      },
	  //allowPointSelect: true,
      enableMouseTracking: true
    }
  },
  series: [
  <?php foreach($TEXTO as $t) {
    echo $t;
    if(array_search($t, $TEXTO)!=(count($TEXTO)-1)) echo ", ";
  }?>
  ]
});
    </script>
</body>
</html>
