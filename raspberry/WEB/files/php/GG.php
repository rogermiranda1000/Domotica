<script>
function myFunction() {
	var g = document.getElementById("mySelect").value;
	var t = document.getElementById("mySelectT").value;
	
	if(g != "" && t != "") {
		var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + '?g='+g+'&t='+t;
		window.history.pushState({path:newurl},'',newurl);
		location.reload();
	}
}
</script>
