/*
 *    version 1.0 Tesy
 *
 *	  	author Qbaty.qi
 *    	2010-9-02
 */
var Tesy = (typeof Tesy == "undefined") ? {} : Tesy;

Tesy.event = {
	stopBubble : function (e) {  
		if (e && e.stopPropagation && e.preventDefault) {// 非IE  
			e.stopPropagation();// 标准W3C的取消冒泡  
			e.preventDefault();// 取消默认行为  
		} else {  
			window.event.cancelBubble = true;// IE的取消冒泡方式  
			window.event.returnValue = false;// IE的取消默认行为  
		}
	},
	preventDefault : function(e) {
		if (typeof(e) == "undefined" || !e) {
			e = window.event;
		}
		e.returnValue = false;
		if (typeof(e.preventDefault) != "undefined") {
			e.preventDefault();
		}
		return true;
	},
	getEventFrom: function(e) {
		var fromElement = null;
		if (typeof e.target != "undefined") {
			fromElement = e.target;
		}
		else {
			fromElement = e.srcElement;
		}
		while (fromElement.nodeType == 3 && fromElement.parentNode != null) {
			fromElement = fromElement.parentNode;
		}
		return fromElement;
	},
	getEventTo: function(e){
		var toEl = e.toElement?e.toElement:e.relatedTarget;
		return ((typeof toEl !== "undefined")?toEl:false);
	},
	/*
	 * setup browser compatibility of Event Listeners
	 * make compatible with those browser do not support W3C Event Listeners
	 */
	addEventListener: function(target, eventType, functionRef, capture) {
		//FF
		if (typeof target.addEventListener != "undefined") {
			target.addEventListener(eventType, functionRef, capture);
		}
		else //IE
			if (typeof target.attachEvent != "undefined") {
				target.attachEvent("on" + eventType, functionRef);
			}
	},
	removeEventListener: function(target, eventType, functionRef, capture) {
		//FF
		if (typeof target.removeEventListener != "undefined") {
			target.removeEventListener(eventType, functionRef, capture);
		}else //IE
			if (typeof target.detachEvent != "undefined") {
				target.detachEvent("on" + eventType, functionRef);
			}
	},
	getObjectType : function(obj){
		
	}
}
