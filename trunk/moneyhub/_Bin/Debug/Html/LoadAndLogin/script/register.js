Hub.register = (function(){
	var status = false;
	
	function post(mail, password){
	    var msg = "";
	    try{
		   Hub.dom.getById("register-msg").innerHTML = "注册中，请稍候......";
		   var result = window.external.UserRegedit(mail, password);//注册接口
           msg = result.replace(/^\s+([0-9]{1,3})[0-9a-zA-Z\<\>]?$/, "$1");
	    }catch(e){
		   //TODO
	    }	
	    return msg;
    }
	
	function changeTitle(name){
	    try{
			if (typeof name === 'string'){
				window.external.ChangeWindowName(name);
            }            				 
		   }catch(e){
				alert(e.message);
		}
	}
	
	function register(){
			var mail = Hub.dom.getById("register-mail").value,
				password = Hub.dom.getById("register-password").value,
				passwordAg = Hub.dom.getById("register-password-again").value;
			var mailMsg = Hub.effect.validateMail("register-mail"),
				passwordMsg = Hub.effect.validatePs("register-password"),
				passwordAgMsg = Hub.effect.validatePsAg("register-password", "register-password-again");	
			var agree = Hub.dom.hasClass(Hub.dom.getById("register-checkbox"), "checked"); 	
			
			if (mailMsg != ""){
				Hub.dom.getById("register-mail-validate").innerHTML = (/请输入您常用的邮箱地址/.test(mail)) ? "请输入您的邮箱地址" : mailMsg;
			}
			if (passwordMsg != ""){
				Hub.dom.getById("register-password-validate").innerHTML = (password == "") ? "请输入您的密码" : passwordMsg;
			}
			if (passwordAgMsg != ""){
				Hub.dom.getById("register-password-again-validate").innerHTML = (passwordAg == "") ? "请再次输入您的密码" : passwordAgMsg;
			}
			if (mailMsg == "" && passwordMsg == "" && passwordAgMsg == "" && agree == true) status = true;
			if (status == true){
				var msg = post(mail, password);
				if (msg == "41"){//成功
				   try{
					   Hub.dom.getById("register-success").style.display = "block";
					   Hub.dom.getById("register-wrap").style.display = "none";
					   Hub.dom.getById("success-mail").innerHTML = mail;
					   
				   }catch(e){
					  //TODO
				   }
				}else{
				   Hub.dom.getById("register-msg").innerHTML = "联网失败，请检查网络设置";
				}
			}else{
				
			}				
    }	
		
	return{
		addRegisterEvent: function(){
		   Hub.dom.getById("register").onclick = register;
		   document.onkeypress = function(e){
				var e = e || window.event;
				if (e.keyCode == 13){
				   register();
				}
		   }
		},
		
		changeTitle: function(name){
		    changeTitle(name);
		}
	}
})();