<h1 style="padding-left:16px; margin-top:2px; margin-bottom:2px;">Casa domótica</h1>
<div class="topnav" id="myTopnav">
	<?php $url = $_SERVER['PHP_SELF']; ?>
	<a href="index.php" <?php if(($url=='/index.php') || $url=='') echo 'class="active"'; ?>>Información</a>
	<a href="graph.php" <?php if($url=='/graph.php') echo 'class="active"'; ?>>Gráfica</a>
	<a href="camara.php" <?php if($url=='/camara.php') echo 'class="active"'; ?>>Cámara</a>
	<a href="editar.php" <?php if($url=='/editar.php') echo 'class="active"'; ?>>Editar</a>
	<a href="phpmyadmin">phpMyAdmin</a>
	<a href="javascript:void(0);" class="icon" onclick="mFunction()">
		<i class="fa fa-bars"></i>
	</a>
</div>

<script>
	function mFunction() {
		var x = document.getElementById("myTopnav");
		if (x.className === "topnav") {
			x.className += " responsive";
		} else {
			x.className = "topnav";
		}
	}
</script>
