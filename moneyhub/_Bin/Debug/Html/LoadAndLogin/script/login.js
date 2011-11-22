Hub.login = (function(){
	var status = false;
	
	function post(mail, password, autoFlag){
		    var msg;
		    try{
			    var result = window.external.UserLoad(mail, password, autoFlag);//登录接口
                msg = result.replace(/^\s+([0-9]{1,3})[0-9a-zA-Z\<\>]?$/, "$1");	
		    }catch(e){
				// var random = Math.sin(Math.random() * 10000);//测试用，产生随机数
				// if (random > -1 && random < -0.5){
					// msg = "101";
				// }else if (random >= -0.5 && random < 0){
					// msg = "102";
				// }else if (random >= 0 && random < 0.5){
					// msg = "3";
				// }else if (random >= 0.5 && random < 1){
					// msg = "4";
				// }
		    }	
		    return msg;
    }
	
	function login(){
			var mail = Hub.dom.getById("login-mail").value,
			password = Hub.dom.getById("login-password").value;
			var autoLogin = Hub.dom.hasClass(Hub.dom.getById("login-checkbox"), "checked");
			if (Hub.effect.validateMail("login-mail") == "") status = true;
			if (password != ""){
				Hub.dom.getById("login-password-validate").innerHTML = "";
				Hub.dom.getById("login-mail-validate").innerHTML = "";
				Hub.dom.getById("login-msg").innerHTML = "";
				if (status == true){
				    var msg;
				    try{
						if (autoLogin == true){
						   msg = post(mail, password, "true");//自动登录
						}else if (autoLogin == false){
						   msg = post(mail, password, "false");
						}
   					}catch(e){
	                    alert(e.message);				
					}
					
					if (msg == "51"){//成功
					   try{
					      window.external.AutoDialog("load", "false");
					   }catch(e){
					      alert(e);
					   }
					}else if (msg == "53"){//密码错误                             
					   Hub.dom.getById("login-password-validate").innerHTML = "密码错误，请重新输入";
					}else if(msg == "52"){//用户名错误
					   Hub.dom.getById("login-mail-validate").innerHTML = "用户名错误, 请重新输入";
					}else if(msg == "101"){//连接超时
					   Hub.dom.getById("login-msg").innerHTML = "登录失败：网络连接超时。</br> 您首次在本机登录此账号，请确保网络畅通";
					}else if(msg == "102"){//本地密码验证失败
					   Hub.dom.getById("login-msg").innerHTML = "密码验证失败，无法解密本机数据";
					}
					if (autoLogin == true){
					   try{
						  window.external.AutoLoad(mail);//选择自动登录后，在本地存储stoken
					   }catch(e){
						  //TODO
					   }
					}
				}else{
					if (Hub.effect.validateMail("login-mail") != ""){
					   Hub.dom.getById("login-mail-validate").innerHTML = "邮箱格式不正确";
					}
				}
			}else{
				if (Hub.effect.validateMail("login-mail") == ""){
					Hub.dom.getById("login-password-validate").innerHTML = "请输入登录密码";
				}
			}
			if (/请输入邮箱地址/.test(mail)){
					Hub.dom.getById("login-mail-validate").innerHTML = "请输入登录邮箱";
			}
	}
		
	return{
		addLoginEvent: function(){
		    Hub.dom.getById("login").onclick = login;
			document.onkeypress = function(e){
			   var e = e || window.event;
			   if (e.keyCode == 13){
			       login();
			   }
			}
		}
	}
})();