<!DOCTYPE html>
<html lang="es">
<head>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.js" type="text/javascript"></script>
</head>
<body>

	<script type="text/javascript">
		var clientID = "ID-" + Math.round(Math.random() * 1000);
		var client = new Paho.MQTT.Client(window.location.host, 9001, clientID);
		console.log(window.location.host);

		client.connect({onSuccess:onConnect});

		function onConnect() {
			console.log("connected");
			client.subscribe(clientID);
			
			<?php 
				//if($_GET['info'] != "" && $_GET['clase'] != "") {
					echo "var message = new Paho.MQTT.Message(clientID+"."' ".$_POST["clase"]/*$_GET['clase']*/." ".$_POST["info"]/*$_GET['info']*/."');";
					echo 'message.destinationName = "prueba";';
					echo "client.send(message);";
				/*}
				else echo "console.log('Sin post.');";*/
			?>
		}
    </script>
</body>
</html>