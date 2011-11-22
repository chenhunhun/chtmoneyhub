Hub.request = (function(){
    /*得到本地用户信息*/
	function getAccountMsg(){
	    var accountList = [];
	    try{
		    accountList = JSON.parse(window.external.QuerySQL("select userid, mail, autoload, autoinfo, autoremb from datUserInfo", "DataDB"));
		}catch(e){
            accountList = [{"userid"   : "Guest", 
							"mail"     : "访客",
							"autoload" : 0,
							"autoinfo" : 1,
							"autoremb" : 0},
			               {"userid"   : "sdfsdfds", 
							"mail"     : "ddback@gmail.com",
							"autoload" : 1,
							"autoinfo" : 1,
							"autoremb" : 0},
						   {"userid"   : "zdfdasfd", 
							"mail"     : "huwence@gmail.com",
							"autoload" : 1,
							"autoinfo" : 0,
							"autoremb" : 1},
						   {"userid"   : "fsdfsdfs", 
							"mail"     : "friend@gmail.com",
							"autoload" : 0,
							"autoinfo" : 1,
							"autoremb" : 1}]	
		}
	    return accountList;
	}

    /*得到当前用户UserID*/
	function getUID(){
	    var UID = "";
		try{
	       UID = window.external.GetCurrentUserID();	
		}catch(e){
		   //TODO
		}
		return UID;
	}
	
	/*
    @params 
	 "mail"     : 更改邮箱
	 "password" : 更改密码
	 "login"    : 设置登录
	*/
	function getOptionId(){
	    var optionId = "";
		try{
		    var status = window.external.GetCurrentSettingStatus();
			switch(status){
			    case "0":
			        optionId = "Guest";
					break;
				case "1":
                    optionId = "set-password";
                    break;					
			    case "2":
				    optionId = "set-mail";
					break;
				default:
				    break;
			}
		}catch(e){
            optionId = "Guest";	
            //alert(e);			
		}
		return optionId;
	}
	
	/*得到当前用户账号*/
	function getCurrentMail(){
	    var mail   = "",
		    UID    = getUID(),
			result = [];
	    try{
		    result = JSON.parse(window.external.QuerySQL("select mail as currentMail from datUserInfo where userid = '" + UID + "'" , "DataDB"));
			if (result.length > 0){
			    mail = result[0].currentMail;
			}			
		}catch(e){
		   //TODO
		   //mail = "gohuwence@finantech.com";
		   alert(e);
		}
		return mail;
	}	
	
	/*删除本地账户*/
	function deleteLocalAccount(id){
	   try{
	       window.external.ExecuteSQL("delete from datUserInfo where userid = " + id, "DataDB");
	   }catch(e){}
	}
	
	return{
       	getAccountMsg: function(){
		    return getAccountMsg();
		},
		
		getUID: function(){
		    return getUID();
		},
		
		getCurrentMail: function(){
		    return getCurrentMail();
		},
		
		deleteLocalAccount: function(){
		    deleteLocalAccount();
		},
		
		getOptionId: function(){
		    return getOptionId();
		}
	}
})();