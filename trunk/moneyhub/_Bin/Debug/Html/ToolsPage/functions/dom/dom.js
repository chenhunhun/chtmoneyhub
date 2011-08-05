/*
 *    version 1.0 Tesy
 *
 *	  	author Qbaty.qi
 *    2010-9-02
 */
var Tesy = (typeof Tesy == "undefined") ? {}: Tesy;

Tesy.dom = {
	$: function(id){
		return document.getElementById(id);
	},
    _: function(tagName){
		return document.getElementsByTagName(tagName);
	},
	cE: function(tagName){
		return document.createElement(tagName);
	},
	fNode : function(pNode,tagname,classname){
	   if(pNode.nodeType !== 9 && pNode.nodeType !== 1 ){
	      throw "method need a right parameter";
	   }
	   
	   var list = [];
	   var resultList = [];
	   
	   list = pNode.getElementsByTagName(tagname);
	   
	   if(typeof classname === 'undefined'){
	      return list;
	   }else{
	      if(list.length > 0){
            for(var i = 0,l = list.length;i<l;++i){
               if(list[i].className == classname){
                  resultList.push(list[i]);
               }
            }
	      }else{
	         return false;
	      }
	   }
	   
      return resultList;
	},
	getElemsByClass : function(parentDom,className){
      if(!parentDom){
         throw "parameter missed"
      }
		var target = parentDom;
		var aArray = target.getElementsByTagName("*");
		var resultArray = [];
		var classArray = [];
		for(var i = 0,l = aArray.length;i<l;++i){
			classArray = aArray[i].className.split(' ');

			for(var j=0,cl = classArray.length;j<cl;++j){
				if(classArray[j] == className){
					resultArray.push(aArray[i]);
					continue;
				}
			}
		}
		return resultArray;
	},
	addClass: function(element, className){
		var originClassName = element.className;
		var classArray = originClassName.split(" ");
		for (var i = 0; i < classArray.length; i++){
			if (className == classArray[i]){
				return true;
			}else{
				element.className += ' ' + className;
			}
		}
	},
	removeClass: function(element, className){
		var originClassName = element.className;
		var classArray = originClassName.split(" ");
		var tempClass = '';
		for (var i = 0; i < classArray.length; i++){
			if (className != classArray[i]){
				if (tempClass == ''){
					tempClass += classArray[i];
				}
				else{
					tempClass += " " + classArray[i];
				}
			}
		}
		element.className = tempClass;
	},
	setOpacity: function(obj, n){
		obj.style.opacity = n / 100;
		obj.style.filter = "alpha(opacity = " + n + ")";
	},
	getOpacity: function(obj){
		if (obj.style.opacity){
			return obj.style.opacity * 100;
		}
		else{
			return obj.style.filter.exec(/d+/);
		}
	},
	getHeight: function(element){
		return element.offsetHeight;
	},
	getWidth: function(element){
		return element.offsetWidth;
	},
	getY: function(element){
		var y = 0;
		var ele = element;
		while (ele){
			y += ele.offsetTop;
			ele = ele.offsetParent;
		}
		return y;
	},
	getX: function(element){
		var x = 0;
		var ele = element;
		while (ele){
			x += ele.offsetLeft;
			ele = ele.offsetParent;
		}
		return x;
	},
	getPosition: function(theElement){
		var positionX = 0;
		var positionY = 0;
		while (theElement != null){
			positionX += theElement.offsetLeft;
			positionY += theElement.offsetTop;
			theElement = theElement.offsetParent;
		}
		return [positionX, positionY];
	},
	isChild: function(child, parent){
		var p = child;
		while (p){
			p = p.parentNode;
			if (p == parent){
				return true;
			}
		}
		return false;
	},
	clearTag: function(element, target){
		if (typeof target == "undefined"){
			var elements = document.getElementsByTagName(element);
		} else{
			var elements = target.getElementsByTagName(element);
		}
		var num = elements.length;
		for (var k = 0; k < num; k++){
			elements[0].parentNode.removeChild(elements[0]);
		}
	},
	getInternalText: function(target){
		var elementChildren = target.childNodes;
		var internalText = "";
	
		for (var i = 0; i < elementChildren.length; i++){
			if (elementChildren[i].NodeType == 3){
				if (!/^\s*$/.test(elementChildren[i].nodeValue)){
					internalText += elementChildren[i].nodeValue;
				}
			}
			else{
				internalText += getInternalText(elementChildren[i]);
			}
		}
		return internalText;
	}
}
