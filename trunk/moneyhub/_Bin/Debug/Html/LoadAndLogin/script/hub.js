var Hub = {};

/*get dom*/
Hub.dom = (function(){

    function nextE(element){
	    if (element != null){
		    var nextElement = element.nextSibling;
			if (nextElement != null){
				return nextElement.nodeType == 1 ? nextElement : nextE(nextElement);
			}else{
			    return null;
			}
		}else{
		    return null;
		}
	}
	
	function nextAllE(element){
	    var nextAll = [],
		    next = nextE(element);
		while (next != null){
		    nextAll.push(next);
			next = nextE(next);
		}
		return nextAll;
	}
	
	function prevE(element){
		if (element != null){
		    var prevElement = element.previousSibling;
			if (prevElement != null){
			   return prevElement.nodeType == 1 ? prevElement : prevE(prevElement);
			}else{
			   return null;
			}
		}else{
		    return null;
		}
	}
	
	function prevAllE(element){
	    var prevAll = [],
		    prev = prevE(element);
		while (prev != null){
		    prevAll.push(prev);
			prev = prevE(prev);
		}
		return prevAll;
	}
	
	function lastE(element){
	   element = element.lastChild;
	   return element && element.nodeType != 1 ? prevE(element) : element;
	}
	
	function firstE(element){
	   element = element.firstChild;
	   return element && element.nodeType != 1 ? nextE(element) : element;
	
	}

	function parent(element){
	    var parentElement = element.parentNode;
        if (parentElement.nodeType == 1){
		    return parentElement;
		}else{
		    return parent(parentElement);
		}		
	}
	
	function offset(element){
	    var left = 0, top = 0,
		    offsetParent = element;
		while(offsetParent != null && offsetParent != document.body){
		    left += offsetParent.offsetLeft;
			top += offsetParent.offsetTop;
			offsetParent = offsetParent.offsetParent;
		}
		return {"left": left, "top": top};
	}
	
	function getByClass(className, tag){
	    var el = [],
		    regTest = new RegExp("(?:^|\\s)" + className + "(?:\\s|$)"),
	        elements = (tag == undefined) ? document.getElementsByTagName("*") : document.getElementsByTagName(tag),
		    current;
		for (var i in elements){
			current = elements[i];
			if (typeof current != 'object') continue;
			if (current.className != undefined && current.className != "" && current.className != null && regTest.test(current.className)){
				el.push(current);
			}
		}
	    return el;
	}
	
	function hasClass(element, className){
	    var regTest = new RegExp("(?:^|\\s)" + className + "(?:\\s|$)"),
		        result = false;
			if (typeof element === 'object' && element.className != undefined && element.className != "" 
			          && element.className != null && regTest.test(element.className)){
				result = true;
			}
	    return result;
	}
	
	function notClass(className, tag){
	    var hasClass = getByClass(className, tag),
				elements = (tag == undefined) ? document.getElementsByTagName("*") : document.getElementsByTagName(tag),
				notClassEl = [],
				current;
			for (var i in hasClass){
				for(var j in elements){
				   current = elements[j];
				   if (typeof current != 'object') continue;
				   if (hasClass[i].className != current.className){
					   notClassEl.push(current);
				   }
				}
			} 
		return notClassEl;
	}
	
    return {
	    getById: function(id){
			try{
				return document.getElementById(id);
			}catch(e){
				alert(e);
			}
		},
		
		getByClass: function(className, tag){
		    return getByClass(className, tag);
		},
		
		getByName: function(name){
		    var element = "";
			try{
			    element = document.getElementsByName(name);
			}catch(e){
			    alert(e);
			}
		    return element;
		},
		
		notClass: function(className, tag){
			return notClass(className, tag)
	    },
		
		hasClass: function(element, className){
		    return hasClass(element, className);
	    },
		
		addClass: function (element, className){
			if (typeof element === 'object'){
				var all = [];
				if (element.className != undefined && element.className != "" && element.className != null){
					all = element.className.split("/\s+/");
				}
				all.push(className);
				element.className = all.join(" ");			
			}
	    },
		
		removeClass: function(element, className){
			if (typeof element === 'object' && element){ 
				if (className != undefined){
				   var regTest = new RegExp("(?:^|\\s)" + className + "(?:\\s|$)");
				  if (element.className != undefined && element.className != "" && element.className != null 
				      && regTest.test(element.className)){
					 var newClass = element.className.replace(regTest, function(){
						return " ";
					 });
					 element.className = newClass;
				  }			
				}else{
				  element.className = "";      
				}
			}
	    },
		
		createE: function(tag){
		    try{
		       return document.createElement(tag);
			}catch(e){
			   alert(e);
			}
		},
		
		nextE: function(element){
            return nextE(element);		         
		},
		
		prevE: function(element){
		    return prevE(element);
		},
		
		nextAllE: function(element, className){
		    return nextAllE(element, className);
		},
		
		prevAllE: function(element, className){
		    return prevAllE(element, className);
		},
		
		firstE: function(element){
		    return firstE(element);
		},
		
		parent: function(element){
		    return parent(element);
		},
		
		offset: function(element){
            return offset(element);		
		},
		
		loadJS: function(src, element){
		   if (typeof element === 'object'){
				var script = document.createElement("script");
				script.type = "text/javascript";
				script.src = src;
				element.appendChild(script);
			}
		}
	}
})();


Hub.event = (function(){

    function addEvent(element, type, handler){  
       if (window.addEventListener){  
           element.addEventListener(type, handler, false);  
       }else if (window.attachEvent){  
           element.attachEvent("on" + type, handler);  
       }else{  
           element["on" + type] = handler;  
       }
	}
	
	function removeEvent(element, type, handler){
	   if (window.removeEventListener){  
          element.removeEventListener(type, handler, false);  
       }else if (window.detachEvent){  
          element.detachEvent("on" + type, handler);  
       }else{  
          element["on" + type] = null;  
       } 
	}
	
    return{
	   addEvent: function(element, type, handler){
	        addEvent(element, type, handler);
	   },
	   
	   removeEvent: function(element, type, handler){
          	removeEvent(element, type, handler);   
	   },
	   
	   stopBubble: function(e){
	      if (e && e.stopPropagation){
		      e.stopPropagation();
		  }else{
		      window.event.cancelBubble = true;
		  }
	   }
	}
})();

/*ajax function*/
Hub.ajax = (function(){

    function createXHR(){
	     //return window.XMLHttpRequest ? new XMLHttpRequest() : new ActiveXObject("Microsoft.XMLHttp");
		 // var xhr = "";
		 // if (window.ActiveXObject){
		    // try{
			     // xhr = new ActiveXObject("Msxml2.XMLHTTP.4.0");
			// }catch(e){
			    // try{xhr = new ActiveXObject("Msxml2.XMLHTTP");}
                // catch(e){xhr = new ActiveXObject("Msxml2.XMLHTTP");}
			// }
		 // }else if (window.XMLHttpRequest){
		    // xhr = new XMLHttpRequest();
		 // }
		 // return xhr;
		 
		 /*先判断ActiveXObject是为了兼容IE7,8的native访问，因为IE7,8同样支持XMLHttpRequest，但不支持本地访问*/
		 return window.ActiveXObject ? new ActiveXObject("Microsoft.XMLHttp") : new XMLHttpRequest();
	}
	
	function change(callback){
	    if (xmlHttp.readyState == 4){//has loaded
		   if (xmlHttp.status == 200 || xmlHttp.status == 0){// success
			  (function(XMLText){
                 return callback(XMLText);			    
			  })(xmlHttp.responseText);
		   }
		}else{
		   return false;
		}
	}
	
	function get(url, callback){
         xmlHttp = createXHR();
		 xmlHttp.onreadystatechange = function(){
		     change(callback);
		 }
		 xmlHttp.open("GET", url, true);
		 //xmlHttp.setRequestHeader("Accept","text/html");
         //xmlHttp.setRequestHeader("Content-Type","application/x-www-form-urlencoded"); 
		 xmlHttp.send(null);
	}
	
	function post(url, data, sync, callback){
	     xmlHttp = createXHR();
		 xmlHttp.onreadystatechange = function(){
		    change(callback);
		 }
		 xmlHttp.open("POST", url, sync);
		 xmlHttp.setRequestHeader("Accept","text/html");
	     xmlHttp.setRequestHeader("Content-Type","application/x-www-form-urlencoded"); 
         xmlHttp.send(data);
	}
	
	return function(params){
	     if(typeof params === "object"){
             if (params.hasOwnProperty("url")){			  
			     if (params.hasOwnProperty("get") && 
				       params.hasOwnProperty("success")){
				      get(params.url, params.success);
				 }else if (params.hasOwnProperty("post") && 
				             params.hasOwnProperty("success") && 
							      params.hasOwnProperty("data") &&
								      params.hasOwnProperty("sync")){
				      post(params.url, params.data, params.sync, params.success);
				 }
			 }		 
		 }
	}
})();
