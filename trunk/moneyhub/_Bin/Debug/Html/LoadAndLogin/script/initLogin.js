function initLoginEffect(){
    var inputDom = [{"name"         : Hub.dom.getById("login-mail"), 
					 "color"        : "#C3C3C3", 
					 "over"         : "images/input2.png", 
					 "out"          : "images/input.png",
					 "deflautValue" : "请输入邮箱地址"},
	                {"name"         : Hub.dom.getById("login-password-name"), 
			         "color"        : "#C3C3C3", 
				     "over"         : "images/input2.png", 
				     "out"          : "images/input.png",
					 "deflautValue" : "请输入密码"},
					{"name"         : Hub.dom.getById("login-password"), 
			         "color"        : "#C3C3C3", 
				     "over"         : "images/input2.png", 
				     "out"          : "images/input.png",
					 "deflautValue" : ""}],
		iconDom = Hub.dom.getById("registered-icon"),
	    bgX = 3,
		bgY = 3;
	for (var i in inputDom){//input
	    Hub.effect.init(inputDom[i], bgX, bgY);
		Hub.effect.inputEffect(inputDom[i].name);
	}
	
	Hub.effect.loginCheck("login-checkbox");
	Hub.effect.iconInputEffect(iconDom);	
	var id = Hub.selectOption.initOption(getAccountMsg(), inputDom[0].name);
	Hub.selectOption.eventOption(iconDom, "click", id, "show");
	
	Hub.login.addLoginEvent();
}

/*取得本地用户信息*/
function getAccountMsg(){
     var result = [];
    try{
	    result = eval("(" + window.external.QuerySQL("select userid as id, mail as name from datUserInfo", "DataDB") + ")");
		if (!result instanceof Array) return false;
	}catch(e){
	    //TODO
	}
	return result;
}

function initRegisterEffect(){
    var inputDom = [{"name"         : Hub.dom.getById("register-mail"), 
					 "color"        : "#C3C3C3", 
					 "over"         : "images/input2.png", 
					 "out"          : "images/input.png",
					 "deflautValue" : "请输入您常用的邮箱地址"},
	                {"name"         : Hub.dom.getById("register-password-name"), 
			         "color"        : "#C3C3C3", 
				     "over"         : "images/input2.png", 
				     "out"          : "images/input.png",
					 "deflautValue" : "6-20字符（数字、字母、符号）"},
					{"name"         : Hub.dom.getById("register-password"), 
			         "color"        : "#C3C3C3", 
				     "over"         : "images/input2.png", 
				     "out"          : "images/input.png",
					 "deflautValue" : ""},
					{"name"         : Hub.dom.getById("register-password-agname"), 
			         "color"        : "#C3C3C3", 
				     "over"         : "images/input2.png", 
				     "out"          : "images/input.png",
					 "deflautValue" : "再输入一次密码以确认无误"},
					{"name"         : Hub.dom.getById("register-password-again"), 
			         "color"        : "#C3C3C3", 
				     "over"         : "images/input2.png", 
				     "out"          : "images/input.png",
					 "deflautValue" : ""}],
	    bgX = 3,
		bgY = 3;
		
	Hub.effect.loginCheck("register-checkbox");
	
	for (var i in inputDom){//input
	    Hub.effect.init(inputDom[i], bgX, bgY);
		Hub.effect.inputEffect(inputDom[i].name);
	}
	
	Hub.register.addRegisterEvent();
}

/*自动登录时，如果失败弹出的信息*/
function autoLoadMessage(param){
    if (param === "101"){
	    Hub.dom.getById("login-msg").innerHTML = "登录失败：网络连接超时。</br> 您首次在本机登录此账号，请确保网络畅通";
	}else if (param === "102"){
        Hub.dom.getById("login-msg").innerHTML = "密码验证失败，无法解密本机数据";	
	}else if (param === "65"){
	    Hub.dom.getById("login-msg").innerHTML = "自动登录失败，请手动登录";	
	}
}


function init(){
    Hub.dom.getById("login-to-register").onclick = function(){
	   Hub.dom.getById("login-wrap").style.display = "none";
	   Hub.dom.getById("register-wrap").style.display = "block";
	   initRegisterEffect();
	   Hub.register.changeTitle("注册财金汇");
	}
	Hub.dom.getById("login-mail-immediate").onclick = function(){
	    Hub.dom.getById("register-success").style.display = "none";
		Hub.dom.getById("login-wrap").style.display = "block";
		Hub.register.changeTitle("登录财金汇");
	}
	Hub.dom.getById("visitors").onclick = function(){//使用访客身份登录
	    try{
		   window.external.AutoDialog("load", "false");
		}catch(e){
		  //TODO
		}
	}
	Hub.dom.getById("ask-visitors").onclick = function(){
        Hub.dom.getById("register-wrap").style.display = "none";
	    Hub.dom.getById("login-wrap").style.display = "block";
    }
	
	initLoginEffect();
	
	// var version = window.navigator.userAgent.toLowerCase();
	// if (/msie\s[\d.]+/.test(version) && version.match(/msie\s([\d.]+)/)[1] == 6){//IE6
	   
	   // var elmBox = Hub.dom.getByClass("boxBg"),
	       // elmBoxButtom = Hub.dom.getByClass("boxBgButtom"),
		   // elemWrap = Hub.dom.getByClass("wrap"),
	       // n = elmBox.length,
		   // m = elmBoxButtom.length,
		   // k = elemWrap.length;
	   // for (var i = 0; i < n; i ++){
	       // elmBox[i].style.width = "470px";
	   // }
	   
	   // for (var i = 0; i < m; i ++){
	       // elmBoxButtom[i].style.width = "470px";
	   // }
	   
	   // for (var i = 0; i < k; i ++){
	       // elemWrap[i].style.width = "470px";
	   // }
	// }
}

window.onload = init;
