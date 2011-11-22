Hub.option = (function(){
    //selected class
	var selectedClass = "option-item-selected";
	    selectorClass = "option-item-selector";
		
	var optionItem = {
	    "set-mail"     : "htmlFragment/set-mail.html",
	    "set-password" : "htmlFragment/set-password.html",
	    "set-login"    : "htmlFragment/set-login.html",
	    "set-manage"   : "htmlFragment/set-manage.html"  
	};
	
	/**账户设置信息
	[{"userid"   : "", 
      "mail"     : "",
	  "autoload" : "",
      "autoinfo" : "",
	  "autoremb" : ""}]
	**/
	var accountList = Hub.request.getAccountMsg();
	
	/**
	<tr class = 'option-content-table-th'>
	  <td width = "110px">账户</td>
	  <td width = "90px">允许消息提示</td>
	  <td width = "80px">记录账户名</td>
	  <td></td>
    </tr>
    <tr>
	  <td>访客</td>
	  <td><span class = "checkBox option-content-table-checkbox"></span></td>
	  <td></td>
	  <td></td>
    </tr>
   **/
    var thString = "<table id = 'manage-table' class = 'option-content-table'><tr><th width = '110px'>账户</th><th width = '90px'>允许消息提示</th><th width = '80px'>记录账户名</th><th></th></tr>";
	//把表头放进来主要是为了处理IE6.0中tbody标签为只读的问题
	
	var firstTrString = "<tr><td id = 0>访客</td><td><span class = 'checkBox option-content-table-checkbox'></span></td><td></td><td></td></tr>";
	
	/**
	<tr>
	  <td>huwence@gmail.com</td>
	  <td><span class = "checkBox option-content-table-checkbox message"></span></td>
	  <td><span class = "checkBox option-content-table-checkbox account"></span></td>
	  <td><span class = "option-content-table-garbage"></span></td>
	</tr>
	**/
	var trArray = [
	    "<tr><td id = "     ,
        ""                  , //index 1
		" userid = '"       ,   
		""                  , //index 3
		"' >"               ,
		""                  , //index 5
	    "</td><td><span class = 'checkBox option-content-table-checkbox message ",
		""                  , //index 7
		"'></span></td>"    ,
	    "<td><span class = 'checkBox option-content-table-checkbox account ",
		""                  , //index 10
		"'></span></td><td><span class = 'option-content-table-garbage'></span></td></tr>"
	];
	
	//记录应用状态
	var appFlag = {
	    "manage" : false,
		"login"  : false
	};
	
	//记录tab状态
	var tabFlag = "set-mail";
	//当前账户
	var currentMail = Hub.request.getCurrentMail();
	
	//记录应用时的状态
	var mailStatus = {
	    "new-mail"              : "",
	    "psw"                   : "",
	    "new-mail-validate"     : "", 
	    "psw-validate"          : ""
	}

	var passWordStatus = {
	    "current-psw"           : "",
		"new-psw"               : "",
		"sure-psw"              : "",
		"current-psw-validate"  : "",
		"new-psw-validate"      : "",
		"sure-psw-validate"     : ""
	}
	
	//更新状态
	/*
	@record 是否更新input状态
	*/
	function updateStatus(status, record){
        for (var key in status){
		    if (status.hasOwnProperty(key)){
                if (record != undefined && record == 1){
                   if (String(key).indexOf("validate") != -1){				
                      Hub.dom.getById(key).innerHTML = status[key];
				   }else{
				      Hub.dom.getById(key).value = status[key];
				   }
                }else if(record != undefined && record == 2){
				   status[key] = "";
				}else{
				   status[key] = Hub.dom.getById(key).value !== undefined ? Hub.dom.getById(key).value : Hub.dom.getById(key).innerHTML;
				}
			}
		}       
	}
	
	//Tab切换事件
    function optionItemClick(event){
	    var target = event.srcElement ? event.srcElement : event.target;
		if (target.id === "set-blank") return false;
		if (!Hub.dom.hasClass(target, selectedClass)){
			Hub.dom.addClass(target, selectedClass); 
			otherChange(target);
			

			
			var contorlSet = Hub.dom.getById("contorl-set");
			Hub.ajax({
			    url     : optionItem[target.id],
				get     : "",
				success : function(result){
				    Hub.dom.getById("content").innerHTML = result;
					ajaxEvent[target.id](contorlSet, target.id);
				}
			});
		}else{
		    return false;
		}
	}
	
	/*先移除再添加事件*/
	function remAddEvent(element, event){
	    Hub.event.removeEvent(element, "click", event);
		Hub.event.addEvent(element, "click", event);
		Hub.dom.getById("app").style.background = "url(images/app.gif) no-repeat";
	}
	
    /*改变其它TAB的状态*/
	function otherChange(element){
	    var prev = Hub.dom.prevAllE(element);
		for (var i = 0; i < prev.length; i ++){
		    if (Hub.dom.hasClass(prev[i], selectedClass)){
			    Hub.dom.removeClass(prev[i], selectedClass);
			}
		}
		
		var next = Hub.dom.nextAllE(element);
		for (var i = 0; i < next.length; i ++){
		    if (Hub.dom.hasClass(next[i], selectedClass)){
			    Hub.dom.removeClass(next[i], selectedClass);
			}	
		}
	}
	
	/*验证*/
	function validteSetInput(id){
	    var validateId = id + "-validate";
	    Hub.dom.getById(id).onblur = function(){
		    var msg = "";
			if (id.indexOf("sure") != -1){
				msg = Hub.effect.validatePsAg("new-psw", id);
			}else if (id.indexOf("new-mail") != -1){
			    msg = Hub.effect.validateMail(id);
			}else{
			    msg = Hub.effect.validatePs(id);
			}
		    Hub.dom.getById(validateId).innerHTML = msg;
		}
	}
	
	
	var ajaxEvent = {
	
	    /*重置密码*/
	    "set-password" : function(contorlSet, targetid){
		    validteSetInput("current-psw");
			validteSetInput("new-psw");
			validteSetInput("sure-psw");//验证
			updateStatus(passWordStatus, 1);
			
			tabFlag = "set-password";
			Hub.event.addEvent(Hub.dom.getById("option-set-password"), "click", submitEvent[targetid]);
			remAddEvent(contorlSet, submitOrApp);
		},
		
		/*重置邮箱*/
		"set-mail": function(contorlSet, targetid){
		    Hub.dom.firstE(Hub.dom.getById("set-item")).style.borderTop = "1px solid #F7FCFF";
		    validteSetInput("new-mail");
			validteSetInput("psw");//验证
			updateStatus(mailStatus, 1);
			
			tabFlag = "set-mail";
			Hub.event.addEvent(Hub.dom.getById("option-set-mail"), "click", submitEvent[targetid]);
			remAddEvent(contorlSet, submitOrApp);
			Hub.dom.getById("old-mail").innerHTML = currentMail;
		},
		
		/*设置泡泡提示信息&&设置是否记录账户名*/
		"set-manage": function(contorlSet, targetid){
		    tabFlag = "set-manage";	
			var trString = [thString, firstTrString];
			if (targetid === "set-manage"){//点击设置后获取用户列表
				for (var i in accountList){
				   if (accountList[i].userid != "Guest"){
						trArray[1]  = i;
						trArray[3]  = accountList[i].userid;
						trArray[5]  = accountList[i].mail;
						trArray[7]  = accountList[i].autoinfo == 1 ? "checked" : "";
						trArray[10] = accountList[i].autoremb == 1 ? "checked" : "";
						trString.push(trArray.join("")); 
					}
				}
			}
			var manage = Hub.dom.getById("manage");
			trString.push("</table>");
			manage.innerHTML = trString.join("");
			Hub.event.addEvent(Hub.dom.getById("manage-table"), "click", submitEvent[targetid]);
			remAddEvent(contorlSet, submitOrApp);
		},
		
        "set-login": function(contorlSet, targetid){
		    tabFlag = "set-login";
			Hub.event.addEvent(Hub.dom.getById("auto-login"), "click", submitEvent[targetid]);
			remAddEvent(contorlSet, submitOrApp);
		}
	}
	
	/*提交事件*/
	var submitEvent = {
	
	    /*重置密码*/
	    "set-password" : function(){
		    var currentPsw = Hub.dom.getById("current-psw").value,
                newPsw     = Hub.dom.getById("new-psw").value,
                surePsw    = Hub.dom.getById("sure-psw").value,
				currentMsg = Hub.effect.validatePs("current-psw"),
				newPswMsg  = Hub.effect.validatePs("new-psw"),
				pswAgMsg   = Hub.effect.validatePsAg("new-psw", "sure-psw"),
				UID        = Hub.request.getUID();
			Hub.dom.getById("current-psw-validate").innerHTML = (currentPsw == "") ? "密码不能为空" : 
				        ((currentMsg != "") ? currentMsg : "");
			Hub.dom.getById("new-psw-validate").innerHTML = (newPsw == "") ? "新密码不能为空" : 
				        ((newPswMsg != "") ? newPswMsg : "");
		    Hub.dom.getById("sure-psw-validate").innerHTML = (surePsw == "") ? "确认密码不能为空" : 
				        ((pswAgMsg != "") ? pswAgMsg : "");
			
			if (currentMsg == "" && newPswMsg == "" && pswAgMsg == ""){
			   try {
			       var result = window.external.ChangeMailOrPwd("password", UID, surePsw, currentPsw),
				       msg    = result.replace(/^\s+([0-9]{1,3})[0-9a-zA-Z\<\>]?$/, "$1");
                   if (msg == "71"){//修改密码成功
				       alert("密码修改成功");
				   }else if (msg == "73"){
				       Hub.dom.getById("current-psw-validate").innerHTML = "当前密码错误";
				   }else{
				       alert("操作失败，请检查网络设置");
				   }
			   }catch(e){
			       //TODO
				   alert(e);
			   }
			}
			updateStatus(passWordStatus, 2);//更新input状态,提交后置空
		},
		
		/*重置邮箱*/
		"set-mail": function(){
			var newMail        = Hub.dom.getById("new-mail").value,
				newMailMsg     = Hub.effect.validateMail("new-mail"),
				validatePsw    = Hub.dom.getById("psw").value,
				validatePswMsg = Hub.effect.validatePs("psw"),
				UID            = Hub.request.getUID();
				
			Hub.dom.getById("new-mail-validate").innerHTML = (newMail == "") ? "请输入新邮箱" : 
		        ((newMailMsg != "") ? newMailMsg : "");
			Hub.dom.getById("psw-validate").innerHTML = (validatePsw == "") ? "请输入密码" : 
		        ((validatePswMsg != "") ? validatePswMsg : "");
			
             			
		    if (newMailMsg == "" && validatePswMsg == "" && currentMail != ""){
			    try {
			       var result = window.external.ChangeMailOrPwd("mail", UID, newMail, validatePsw),
                       msg    = result.replace(/^\s+([0-9]{1,3})[0-9a-zA-Z\<\>]?$/, "$1");
                   if (msg == "72"){//邮箱修改成功
				       alert("登录邮箱修改成功！已向您的邮箱发送了验证邮件，请收取邮件并完成验证操作");
				   }else{
				       alert("操作失败，请检查网络设置");
				   }				   
			    }catch(e){
			       //TODO
				   alert(e);
			   }
			}
			updateStatus(mailStatus, 2);//更新input状态,提交后置空
		},
		
		/*设置泡泡提示信息&&设置是否记录账户名*/
		"set-manage": function(event){
		    var target = event.srcElement ? event.srcElement : event.target,
			    parent = Hub.dom.parent(Hub.dom.parent(target)),
				targetId = Hub.dom.firstE(parent).getAttribute("id");//通过tr中的id属性维护账户数组
		    if (target.tagName != "SPAN") return false;
			if (Hub.dom.hasClass(target,"checked")){
				Hub.dom.removeClass(target, "checked");
			    if (targetId != "" && target.className.indexOf("message") > 0)	accountList[targetId].autoInfo = 0;
				if (targetId != "" && target.className.indexOf("account") > 0)  accountList[targetId].autoremb = 0;
			}else{
				Hub.dom.addClass(target, "checked");
			    if (targetId != "" && target.className.indexOf("message") > 0)  accountList[targetId].autoInfo = 1;
				if (targetId != "" && target.className.indexOf("account") > 0)  accountList[targetId].autoremb = 1;
			}
			accountList[targetId].mail += ">"//标记修改
            
            if (targetId != "" && target.className.indexOf("garbage") > 0){//删除
				if (window.confirm("是否清除该账号保存在本机上的数据？")){
					try{
						 Hub.request.deleteLocalAccount();
						 parent.style.display = "none";//改变状态
						 accountList[targetId].mail = accountList[targetId].mail.replace(/\>/g, "%");//%标记删除
					}catch(e){
						 parent.style.display = "none";//改变状态
						 accountList[targetId].mail = accountList[targetId].mail.replace(/\>/g, "%");
					} 							 
				}
			}
            
            if (appFlag["manage"] == false){//改变状态
			    Hub.dom.getById("app").style.background = "url(images/app.gif) no-repeat";
				appFlag["manage"] = true;
			}			
		},
		
		
        "set-login": function(){
		    var chekbox = Hub.dom.getById("auto-login"),
			    UID     = Hub.request.getUID();
			if (Hub.dom.hasClass(chekbox,"checked")){
				Hub.dom.removeClass(chekbox, "checked");
				try{
				    window.external.ExecuteSQL("update datUserInfo set autoremb = 0" + 
					                              " where userid = " + UID, "DataDB");
				}catch(e){}
			}else{
				Hub.dom.addClass(chekbox, "checked");
				try{
				    window.external.ExecuteSQL("update datUserInfo set autoremb = 1" + 
					                              " where userid = " + UID, "DataDB");
				}catch(e){}
			}
			
			if (appFlag["login"] == false){//改变状态
			    Hub.dom.getById("app").style.background = "url(images/app.gif) no-repeat";
				appFlag["login"] = true;
			}
		}
	}
	
	/*确定和应用点击事件*/	
	function submitOrApp(event){
	    var target = event.srcElement ? event.srcElement : event.target;
		//如果应用按钮为灰，则点击无效
	    if (target.style.background == "url(images/app1.gif) no-repeat") return false;
		if (target.id == "ok" || target.id == "app"){//点击确定，提交数据
		    if (tabFlag == "set-manage"){
				for (var i in accountList){
					if (/(?:\>)$/.test(accountList[i].mail)){
					   try{
						   window.external.ExecuteSQL("update datUserInfo set autoinfo = " + accountList[i].autoinfo + ", autoremb = " + accountList[i].autoremb +
													  " where userid = " + accountList[i].userid, "DataDB");
					   }catch(e){
						   //TODO
						   alert(e);
					   }
					}
				}
			}else if (tabFlag == "set-mail"){//当更改邮箱为点击提交时
			    updateStatus(mailStatus);
			}else if (tabFlag == "set-password"){
			    updateStatus(passWordStatus);
			}
			
			if (target.id == "app" && (appFlag["manage"] == true || appFlag["login"] == true) 
			        && (tabFlag == "set-manage" || tabFlag == "set-login")){
			    target.style.background = "url(images/app1.gif) no-repeat";
                if (appFlag["manage"] == true) appFlag["manage"] = false;	
                if (appFlag["login"] == true) appFlag["login"] = false;						
			}
			
			if (target.id == "ok"){
			    //TODO 关闭窗口?
				try{
				    window.external.AutoDialog("setting", "false");
				}catch(e){
				   alert(e);
				}
			}
		}else if (target.id == "cancel"){
		    //TODO 关闭窗口? 
			try{
				window.external.AutoDialog("setting", "false");
			}catch(e){
			    alert(e);
			}
		}
	}
	
    return {
	    init: function(){
		    //Hub.dom.firstE(Hub.dom.getById("set-item")).style.borderTop = "1px solid #F7FCFF";
	        Hub.event.addEvent(Hub.dom.getById("set-item"), "click", optionItemClick);
			var optionId = Hub.request.getOptionId();
			if (optionId === "Guest"){
			    Hub.dom.getById("set-mail").style.display     = "none";
				Hub.dom.getById("set-password").style.display = "none";
				Hub.dom.getById("set-login").style.display    = "none";
				Hub.dom.addClass(Hub.dom.getById("set-manage"), selectedClass);
				Hub.dom.getById("set-blank").style.height = "202px";
				optionId = "set-manage";
				Hub.dom.getById("set-manage").style.borderTop = "1px solid #F7FCFF";
			}
			//初态
			Hub.dom.addClass(Hub.dom.getById(optionId), selectedClass);
			Hub.ajax({
				url     : optionItem[optionId],
				get     : "",
				success : function(result){
					Hub.dom.getById("content").innerHTML = result;
					var contorlSet = Hub.dom.getById("contorl-set");
					ajaxEvent[optionId](contorlSet, optionId);
				}
			});		
	    }
	}	
})();


function init(){
   Hub.option.init();
}

window.onload = init;