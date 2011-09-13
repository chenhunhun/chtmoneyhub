
var MH=MH||{};

MH.dom={
	$:function(a){
		return document.getElementById(a);
	},
	
	addClass:function(element,className){
		element.className=element.className+" "+className;
	},
	
	removeClass:function(element,className){
		var originClassName=element.className;
		var classArray=originClassName.split(" ");
		var tempClass='';
		for(var i=0;i<classArray.length;i++){
			if(className!=classArray[i]){
				if(tempClass==''){
					tempClass+=classArray[i];
				}else{
					tempClass+=" "+classArray[i];
				}
			}
		}
		element.className=tempClass;
	},
	
	setOpacity:function(obj,n){
		obj.style.opacity=n/100;
		obj.style.filter="alpha(opacity = "+n+")";
	},
	
	getOpacity:function(obj){
		if(obj.style.opacity){
			return obj.style.opacity*100;
		}else{
			return obj.style.filter.exec(/d+/);
		}
	},
	isChild:function(child,parent){
		var p=child;
		while(p){
			p=p.parentNode;
			if(p==parent){
				return true;
			}
		}
		return false;
	},
	shiftL:function(a,b){
		if(typeof a=='object'){
			for(var i=0;i<a.length;i++){
				MH.dom.$(a[i]).style.display="none";
			}
		}else{
			MH.dom.$(a).style.display="none";
		}
		if(typeof b=='object'){
			for(var i=0;i<b.length;i++){
				MH.dom.$(b[i]).style.display="";
			}
		}else{
			MH.dom.$(b).style.display="";
		}
	}
};

MH.control={
	isIE:!+"\v1",
	oldScreenHeight:0,
	oldScreenWidth:0,
	favPageNum:5,
	appPageNum:5,
	totalPages:0,
	totalAppPages:0,
	currentPage:0,
	currentAppPage:0,
	appLiWeight:154,
	IEVersion:function(){
		var X,V,N;V=navigator.appVersion;N=navigator.appName;
		if(N=="Microsoft Internet Explorer"){
			X=parseFloat(V.substring(V.indexOf("MSIE")+5,V.lastIndexOf("Windows")));
		}else{
			X=parseFloat(V);
		}
		return X;
	},
	appContainerWidth:function(){
		return MH.control.appPageNum*MH.control.appLiWeight;
	},
	
	favContainerWidth:function(){
		return MH.control.favPageNum*196;
	},
	
	screenHeight:function(){
		return MH.control.isIE?document.documentElement.offsetHeight:document.documentElement.clientHeight;
	},
	
	screenWidth:function(){
		return MH.control.isIE?document.documentElement.offsetWidth-62:document.documentElement.clientWidth-62;
	},
	
	resizeRegister:function(type,func){
		if(typeof MH.control.rsEvent==='undefined'){
			MH.control.rsEvent={};
		}
		MH.control.rsEvent[type]=func;
	},
	
	resizefire:function(){
		if(typeof MH.control.rsEvent==='undefined'){
			return true;
		}else{
			for(var i in MH.control.reEvent){
				MH.control.reEvent[i]();
			}
		}
	},
	
	delArray:function(arr,n){
		if(n<0)
			return arr;
		else
			return arr.slice(0,n).concat(arr.slice(n+1,arr.length));
	},
	
	stopBubble:function(e){
		if(e&&e.stopPropagation&&e.preventDefault){
			e.stopPropagation();
			e.preventDefault();
		}else{
			window.event.cancelBubble=true;
			window.event.returnValue=false;
		}
	},
	
	checkType:function(obj){
		if(obj.style.display=='none'||MH.dom.getOpacity(obj)<100){
			return 0;
		}else{
			return 1;}
	},
	
	openLink:function(n,appid){
		var types=appid.slice(0,1);
		switch(types){
			case"a":appType=apptypes[1];
			break;
			case"b":appType=apptypes[2];
			break;
			case"c":appType=apptypes[3];
			break;
			case"d":appType=apptypes[4];
			break;
			case"e":appType=apptypes[5];
			break;
			case"f":appType=apptypes[6];
			break;
		}
		for(var j in appList[appType]){
			if(appList[appType][j].id==appid){
				var sublink=appList[appType][j]['sublink'];
				for(i in sublink){
					if(n==i){
						setTimeout((function(url){
							return function(){
								try{
									window.external.SendVisitRecord(url);
								}catch(err){
								}
								window.open(url);
							}
						})(sublink[i]),100)
					}
					else{
					}
				}
			}
		}
	},
	
	
	
	
	
	adjustDivHeight:function(){
		var target=MH.dom.$("appContainer");
		var navHeight=target.offsetTop;
		target.style.height="216px";
	},
	
	appViewResize:function(){
		try{
				var swidth=MH.control.screenWidth();
				var sheight=MH.control.screenHeight();
				if(typeof favScrollInterval!="undefined")clearInterval(favScrollInterval);
				MH.dom.$("Favcontainer").scrollLeft=0;
				MH.control.currentPage=0;
				MH.dom.$("appContainer").scrollLeft = 0;
				MH.control.currentAppPage = 0;
				
				
				var revent=MH.dom.$('r_event');
				var favdiv=MH.dom.$('favdiv');
				var appbox=MH.dom.$('app_box');
				var appdiv=MH.dom.$('app_div');
				var other=MH.dom.$('other');
				var finatech_ad=MH.dom.$('finatech_ad');
			
				var appBoxHeight=285;
				//最小高度为40（fav到top距离）+196（fav固定值）+214（other最小值）
				var minHeight=500;
				var maxHeight=595;
				//var maxHeight=595+appBoxHeight;
				var old_finatech_ad_height=90;
				
				var top=40;
				var favH=206;
				var otherH=214;
				if(appbox.style.display=="none"){
					//未显示时效果
					/*
					226为other背景图H最大值
					*/
					var top=40;
					var favH=206;
					var otherH=214;
					if(sheight<=minHeight){
						//finatech_ad.style.top="190px";
						//alert("st1");
						
						favdiv.style.top=top+"px";
						other.style.top=(minHeight-favH-top)+"px";
						//alert((minHeight-favH-top));
						revent.style.height=otherH+"px";
						rcoupon.style.height=otherH+"px";
						finatech_ad.style.height=otherH+"px";
						document.getElementById("textAd").style.height = otherH+"px";
						revent.style.backgroundPosition="0px 0px";
						rcoupon.style.backgroundPosition="0px 0px";
					} else if(maxHeight<sheight<=maxHeight){
						//alert(sheight);
						var selfMargin=((sheight-226-favH-top)/3);
						//alert(selfMargin);
						favdiv.style.top=selfMargin+top+"px";
						other.style.top=selfMargin*2+favH+top+"px";
						revent.style.height="226px";
						finatech_ad.style.height="226px";
						document.getElementById("textAd").style.height = "226px";
						rcoupon.style.height="226px";
						revent.style.backgroundPosition="0px 0px";
						rcoupon.style.backgroundPosition="0px 0px";
						
					}
					else{
						//alert("st3");
						//var oldSheight=sheight;
						//sheight=maxHeight;
						//alert(sheight);
						var selfMargin=((sheight-226-favH)/3);
						favdiv.style.top=selfMargin+"px";
						other.style.top=selfMargin*2+favH+"px";
						revent.style.height="226px";
						finatech_ad.style.height="226px";
						document.getElementById("textAd").style.height = "226px";
						rcoupon.style.height="226px";
						revent.style.backgroundPosition="0px 0px";
						rcoupon.style.backgroundPosition="0px 0px";
						//sheight=oldSheight;
					}
				} else {
					//app显示状态
				//最小高度为40（fav到top距离）+196（fav固定值）+214（other最小值）
					var floatHeight=20;
					minHeight=490+appBoxHeight+floatHeight;
					maxHeight=595+appBoxHeight+floatHeight;
					//alert(minHeight);
					if(sheight<=minHeight){
						//finatech_ad.style.top="190px";
						//sheight=minHeight;
						//alert("st1");
						appbox.style.top=top+"px";
						favdiv.style.top=top+appBoxHeight+floatHeight+"px";
						other.style.top=top+appBoxHeight+favH+"px";
						finatech_ad.style.height=otherH+"px";
						document.getElementById("textAd").style.height = otherH+"px";
						revent.style.height=otherH+"px";
						rcoupon.style.height=otherH+"px";
						revent.style.backgroundPosition="0px 0px";
						rcoupon.style.backgroundPosition="0px 0px";
					} else if(maxHeight<sheight<=maxHeight){
						//alert("st2");
						var selfMargin=((sheight-226-favH-appBoxHeight-floatHeight)/3);
						appbox.style.top=top+"px";
						if(selfMargin>0)
						{
							//alert(selfMargin);
							favdiv.style.top=selfMargin+appBoxHeight+floatHeight+top+"px";
							other.style.top=selfMargin*2+favH+appBoxHeight+floatHeight+"px";
						}else{
							//alert(selfMargin);
							favdiv.style.top=appBoxHeight+floatHeight+top+"px";
							other.style.top=selfMargin*2+favH+appBoxHeight+floatHeight+"px";
						}
						revent.style.height="226px";
						rcoupon.style.height="226px";
						document.getElementById("textAd").style.height = "226px";
						finatech_ad.style.height="226px";
						revent.style.backgroundPosition="0px 0px";
						rcoupon.style.backgroundPosition="0px 0px";
						
					}
					else{
						appbox.style.top=top+"px";
						sheight=maxHeight;
						//alert(sheight);
						var selfMargin=((sheight-226-favH-appBoxHeight)/3);
						favdiv.style.top=selfMargin+appBoxHeight+top+floatHeight+"px";
						other.style.top=selfMargin*2+appBoxHeight+favH+"px";
						revent.style.height="226px";
						rcoupon.style.height="226px";
						finatech_ad.style.height="226px";
						document.getElementById("textAd").style.height = "226px";
						revent.style.backgroundPosition="0px 0px";
						rcoupon.style.backgroundPosition="0px 0px";
					}
				}
			
		}catch(e){
		
		}
	},
	
	
	
	resizeFunction:function(){
		try{
			if((MH.control.screenHeight()!=MH.control.oldScreenHeight)||(MH.control.screenWidth()!=MH.control.oldScreenWidth)){
				var swidth=MH.control.screenWidth();
				var sheight=MH.control.screenHeight();
				MH.control.oldScreenHeight=sheight;
				MH.control.oldScreenWidth=swidth;
				if(typeof favScrollInterval!="undefined")clearInterval(favScrollInterval);
				MH.dom.$("Favcontainer").scrollLeft=0;
				MH.control.currentPage=0;
				MH.dom.$("appContainer").scrollLeft = 0;
				MH.control.currentAppPage = 0;
				
				
				var revent=MH.dom.$('r_event');
				var favdiv=MH.dom.$('favdiv');
				var appbox=MH.dom.$('app_box');
				var appdiv=MH.dom.$('app_div');
				var other=MH.dom.$('other');
				var finatech_ad=MH.dom.$('finatech_ad');
				
				var lo_width=250;
				var finatech_ad_width=300;
				var revent_width_min=235;
				var revent_width_max=410;
				var marginWidth=55;
				
				var minWidth=lo_width+finatech_ad_width+revent_width_min+marginWidth;
				var maxWidth=lo_width+finatech_ad_width+revent_width_max+marginWidth;
				if(swidth<=(minWidth)){
					//alert("st1");
					other.style.width=(minWidth)+"px";
					other.style.marginLeft="0px";
					other.style.left="0px";
					revent.style.width=revent_width_min+"px";
					finatech_ad.style.width=finatech_ad_width+"px";
					revent.style.margin="0 10px 0 20px";
					finatech_ad.style.margin="0 5px 0 10px";
				}else if(swidth>=maxWidth){
					//alert("st3");
					other.style.width=maxWidth+"px";
					other.style.marginLeft="-"+parseInt(maxWidth)/2+"px";
					other.style.left="50%";
					revent.style.width=revent_width_max+"px";
					finatech_ad.style.width=finatech_ad_width+"px";
					revent.style.margin="0 10px 0 20px";
					//rcoupon.style.margin="0 10px 0 10px";
					finatech_ad.style.margin="0 5px 0 10px";
				}else{ 
					/*
					alert("st2="+swidth);
					var finatech_ad_width_min=300;
					var finatech_ad_width_max=450;
					var revent_width_min=235;
					var revent_width_max=410;
					var marginWidth=55;
				
					other.style.width=swidth+"px";
					var omarginleft=-swidth/2;
					other.style.marginLeft=omarginleft+"px";
					other.style.left="50%";
					alert("剩余宽度="+(swidth-marginWidth-lo_width));
					if((swidth-marginWidth-lo_width)<(finatech_ad_width_max+revent_width_max)){
						wevent=parseInt(revent_width_min+(swidth-marginWidth-lo_width-finatech_ad_width_min-revent_width_min)/2);
						finatech_ad_width=parseInt(finatech_ad_width_min+(swidth-marginWidth-lo_width-finatech_ad_width_min-revent_width_min)/2);
					}else{
						alert("st2_else");
						wevent=revent_width_max;
						finatech_ad_width=finatech_ad_width_max;
					}
					var wevent=parseInt(swidth-lo_width-finatech_ad_width-marginWidth);
					revent.style.width=wevent+"px";				
					finatech_ad.style.width=finatech_ad_width+"px";
					revent.style.margin="0 10px 0 20px";
					finatech_ad.style.margin="0 5px 0 10px";
					*/
					//alert("st2="+swidth);
					var finatech_ad_width=300;
					var revent_width_min=235;
					var revent_width_max=410;
					var marginWidth=55;
				
					other.style.width=swidth+"px";
					var omarginleft=-swidth/2;
					other.style.marginLeft=omarginleft+"px";
					other.style.left="50%";
					//alert("剩余宽度="+(swidth-marginWidth-lo_width));
					if((swidth-marginWidth-lo_width-finatech_ad_width)<revent_width_max){
						wevent=parseInt(swidth-marginWidth-lo_width-finatech_ad_width);
					}else{
						//alert("st2_else");
						wevent=revent_width_max;
					}
					var wevent=parseInt(swidth-lo_width-finatech_ad_width-marginWidth);
					revent.style.width=wevent+"px";				
					finatech_ad.style.width=finatech_ad_width+"px";
					revent.style.margin="0 10px 0 20px";
					finatech_ad.style.margin="0 5px 0 10px";
					
				}
				var appBoxHeight=280;
				//调用调整高度方法
				MH.control.appViewResize();
				renderFav();
				//alert("更新");
				MH.control.currentAppPage=0;
				handle(currentAppArray);
				MH.control.adjustDivHeight();
			}
		}catch(e){
		}
	},
	
	resizeOk:function(){
		if((typeof resizetimer=='undefined')||resizetimer==null){
			resizetimer=setTimeout(function(){
				MH.control.resizeFunction();resizetimer=null;},200);
		}
	},
	
	initApp:function(){
		var trigger=MH.dom.$('aplusspan');
		var timer;
		window.onresize=function(){
			MH.control.resizeOk();
		}
		trigger.onclick=function(){
			var appBox=MH.dom.$('app_box');
			if(document.getElementById('app_box').style.display=="none")
			{
				changePic(1);
				appBox.style.display="";
				//调整高度事件
				MH.control.appViewResize();
				renderFav();
				
			} else{
				changePic(0);
				appBox.style.display="none";
				
				//调整高度事件
				MH.control.appViewResize();
				renderFav();
			}
		}
		
	},
	
	checkAlarm:function(){
		var resultArr=MH.interact.getAlarm();
		var cnt=0;
		if(resultArr){
			for(var i in resultArr){
				if(resultArr[i].status==1){
					cnt++;
				}
			}
		}
		if(cnt>0){
			MH.dom.$('alarm').style.display="block";
		}else{
			MH.dom.$('alarm').style.display="none";
		}
	}
};
MH.interact={
	//取数据
	
	getFav:function(){
		try{
			var favResult=window.external.GetFav();			
        	if (typeof favResult == 'string') {
            	var favData = JSON.parse(favResult);
				var j=favData.length;
				MH.favArr.length=0;
				MH.favStatusArr.length=0;
				if(j>0){
					for(i=0;i<j;i++){
						MH.favArr.push(favData[i].id);
						MH.favStatusArr.push(favData[i].status);
					}
				} else {
					MH.favArr=[];
					MH.favStatusArr=[];
				}
        	}
		}catch(e){
			if(typeof MH.favArr=='undefined'){
				MH.favArr=[];
				MH.favStatusArr=[];
			}
		}
	},
	
	//添加
	saveFav:function(appID){
		try{
			var res=window.external.SaveFav(appID);
		}catch(e){
		}
	},
	
	//删除
	deleteFav:function(appID){
		try{
			var res=window.external.DeleteFav(appID);
		}catch(e){
		}		
	},
	
	getAlarm:function(){
		var alarmStr,alarmArr;
		try{
			alarmStr=window.external.GetTodayAlarms();
			alarmArr=JSON.parse(alarmStr);
		}catch(e){
			return true;
		}
		return alarmArr;
	}
}