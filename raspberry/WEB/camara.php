<!DOCTYPE html>
<html lang="es">
<head>
	<?php include 'files/php/index.php'; ?>
	<link rel="stylesheet" href="files/css/misestilos.css">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.js" type="text/javascript"></script>
</head>
<body>
	<header>
		<?php include 'files/php/arriba.php'; ?>
	</header>
	
	<br>
	<div style="width: 70%; height: 100%; margin: 0 auto;">
		<img src='' id='img1' style="width: 100%; height: auto;">
	</div>
	
	<script type="text/javascript">
		var clientID = "ID-" + Math.round(Math.random() * 1000);
		var client = new Paho.MQTT.Client(window.location.host, 9001, clientID);
		var time = 0;
		console.log(window.location.host);

		client.onMessageArrived = onMessageArrived;
		client.connect({onSuccess:onConnect});
		
		function onConnect() {
			console.log("connected");
			client.subscribe(clientID);
			
			show_me();
		}
		function show_me() {
			var message = new Paho.MQTT.Message(clientID+" WEB foto");
			message.destinationName = "central";
			client.send(message);
		}
		function onMessageArrived(msg) {
			var mensaje = msg.payloadString
			console.log(mensaje);
			if(mensaje.split(".").length==2 && mensaje.split(".")[0]=="refresh" && document.getElementById("img1").src != 'data:image/jpg;base64,'+mensaje.split(".")[1])
				document.getElementById("img1").src = 'data:image/jpg;base64,'+mensaje.split(".")[1];//"/files/img/"+mensaje.split(".")[1]+".jpg";//?"+Math.ceil(Math.random()*1000000);
			
			time++;
			if(time>=8) {
				show_me();
				time = 0;
			}
		}
		/*window.setInterval(function(){
			if(time==0) {
				show_me();
				time++;
			}
		}, 100);*/
    </script>
</body>
</html>