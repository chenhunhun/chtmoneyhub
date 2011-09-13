function fsinit(){
	var couponhost = "";
	try {
		couponhost = window.external.GetHostName("benefit");
	} catch (e) {
		couponhost = "http://benefit.caijinhui.com/";
	}

	var iframeDiv = document.getElementById("ifdiv");
	if(!iframeDiv.innerHTML){
		iframeDiv.innerHTML = '<iframe src="'+couponhost+'coupon_list.php" frameborder="no" border="0" marginwidth="0" marginheight="0" class="mainw"></iframe>';
	}
	window.onresize = fsinit;
}