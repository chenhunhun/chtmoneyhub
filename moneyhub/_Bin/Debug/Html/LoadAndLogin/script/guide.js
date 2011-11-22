Hub.guide = (function(){
    
	var clickElement = {
	    "checkBox" : Hub.dom.getById("guide-tip"),
		"sayLater" : Hub.dom.getById("guide-later"),
		"register" : Hub.dom.getById("immediate-register")
	}
	
	function setTip(param){
	    try{
		    if (/^[0-1]$/.test(param))
		      window.external.SetRegGuideInfoParam(param);		
		}catch(e){
            //TODO
            alert(e);			
		}
	}
	
    /*不在弹出提示事件*/
	function tipClick(){
	    if (Hub.dom.hasClass(clickElement.checkBox, "checked")){
		    Hub.dom.removeClass(clickElement.checkBox, "checked");  
            setTip("0");			
		}else{
		    Hub.dom.addClass(clickElement.checkBox, "checked");
			setTip("1");
		}   
	}	

    /*以后再说*/
	function laterClick(){
	    try{
		    window.external.AutoDialog("registerguide", "false");
		}catch(e){
		    //TODO
		}
	}
	
	function immediateRegister(){
	    try{
		    window.external.AutoDialog("registerguide", "false");
			window.external.AutoDialog("register", "true");
		}catch(e){
            //TODO				
		}
	}
	
	return {
	    init : function(){
		    Hub.event.addEvent(clickElement.checkBox, "click", tipClick);
	        Hub.event.addEvent(clickElement.sayLater, "click", laterClick);
			Hub.event.addEvent(clickElement.register, "click", immediateRegister);
		}
	}
})();

function init(){
    Hub.guide.init();
}

window.onload = init;