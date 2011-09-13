
var selector, showlist;
var bigStarInterval = new Array();
function init(){
    var wwwhost = "";
    try {
        wwwhost = window.external.GetHostName("web");
    } 
    catch (e) {
        wwwhost = "http://www.caijinhui.com/";
    }
    document.getElementById("amore").href = wwwhost;
    changePic(0);
    selector = document.getElementById("selector");
    showlist = document.getElementById("app_div");
    showlist_1 = document.getElementById("appContent");
    currentAppArray = "";
    renderFav();
    sortableFav();
    handle(currentAppArray);
    MH.control.resizeOk();
    MH.control.checkAlarm();
    MH.control.initApp();
    
    if (!window.location.hash) {
        window.location.hash = "#all";
    }
    shiftTab(window.location.hash);
    selector.onkeyup = function(){
        return findName(selector.value, "banks");
    };
    renderAd();
    DD_belatedPNG.fix('#app_box_bg_left,.apppagedoton,.apppagedotoff');
}

function showEnt(object, para){
    MH.dom.setOpacity(object, 100);
    object.style.display = "";
    if (object.hideTimer) {
        clearTimeout(object.hideTimer);
    }
    if (object.showTimer) {
        clearTimeout(object.showTimer);
    }
    var temp;
    var begin = para.begin;
    var change = para.change;
    var duration = para.duration;
    temp = Math.ceil(Tween.strongEaseOut(para.t, begin, change, duration));
    object.style.height = temp + "px";
    if (para.t < duration) {
        para.t++;
        object.showTimer = setTimeout((function(o, p){
            return function(){
                showEnt(o, p);
            }
        })(object, para), 10);
    }
}

function hideEnt(object, para){
    if (object.hideTimer) {
        clearTimeout(object.hideTimer);
    }
    if (object.showTimer) {
        clearTimeout(object.showTimer);
    }
    var temp;
    var begin = para.begin;
    var change = para.change;
    var duration = para.duration;
    temp = Math.ceil(Tween.strongEaseOut(para.t, begin, change, duration));
    MH.dom.setOpacity(object, temp);
    if (para.t < duration) {
        para.t++;
        object.hideTimer = setTimeout((function(o, p){
            return function(){
                hideEnt(o, p);
            }
        })(object, para), 20);
    }
    else {
        object.style.display = "none";
        MH.dom.setOpacity(object, 100);
    }
}

function findName(name, type){
    if (name.length > 0) {
        var resultArray = searchIndex(name, type);
        if (resultArray.length > 0) {
            handle(resultArray);
        }
        else {
            showlist.innerHTML = '<div class="no_search">' + '<div class="search_main">' + '<h1>对不起，没有搜索到你要寻找的应用</h1>' + '<strong>建议您检查：</strong>' + '<p>您确认填写的拼音正确</p>' + '<p>您是否输入了空格</p>' + '</div></div>';
        }
    }
    else 
        handle(appList[type]);
}


/*
 * 按照某一速度滚动某一距离
 * @param d 滚动距离
 * @param speed 滚动速度
 */
function favScrollRun(d, speed){
    var Favct = document.getElementById("Favcontainer");
    var Favcontent = document.getElementById("Favcontent");
    i = Favct.clientWidth;
    h = Favcontent.scrollWidth;
    a = Favct.scrollLeft;
    if (d > a) 
        if (h - d > i) 
            a += Math.ceil((d - a) / speed);
        else 
            a += Math.ceil((d - a - (h - d)) / speed);
    else 
        a += (d - a) / speed;
    Favct.scrollLeft = a;
    if (a == d || Favct.offsetTop == a) 
        clearInterval(favScrollInterval);
}

/*
 * 向某一方向滚动
 * @param direction 滚动方向
 */
function favScroll(direction){
    if (typeof favScrollInterval != "undefined") 
        clearInterval(favScrollInterval);
    var scrollTo = 0;
    if (direction == "right") {
        MH.control.currentPage++;
    }
    else 
        if (direction == "left") {
            MH.control.currentPage--;
        }
        else {
            MH.control.currentPage = direction;
        }
    scrollTo = MH.control.favContainerWidth() * MH.control.currentPage;
    favScrollInterval = setInterval('favScrollRun(' + scrollTo + ', 20)', 10);
    //决定是否隐藏左或右箭头
    if (MH.control.currentPage > 0) {
        document.getElementById('FavLeft').style.visibility = "visible";
    }
    else {
        document.getElementById('FavLeft').style.visibility = "hidden";
    }
    if (MH.control.currentPage < MH.control.totalPages - 1) {
        document.getElementById('FavRight').style.visibility = "visible";
    }
    else {
        document.getElementById('FavRight').style.visibility = "hidden";
    }
    //改变翻页圆点
    document.getElementById('favpage').innerHTML = "";
    for (i = 0; i < MH.control.totalPages; i++) {
        if (MH.control.currentPage == i) {
            document.getElementById('favpage').innerHTML += '<div class="pagedoton"></div>';
        }
        else {
            document.getElementById('favpage').innerHTML += '<div class="pagedotoff" onclick="favScroll(' + i + ');"></div>';
        }
    }
}


/*添加APP部分的滚动事件*/
function appScrollRun(d, speed){
    var appCt = document.getElementById("appContainer");
    var appContent = document.getElementById("appContent");
    i = appCt.clientWidth;
    h = appContent.scrollWidth;
    a = appCt.scrollLeft;
    
    if (d > a) 
        if (h - d > i) {
            a += Math.ceil((d - a) / speed);
        }
        else {
            a += Math.ceil((d - a - (h - d)) / speed);
        }
    else {
        a += (d - a) / speed;
    }
    appCt.scrollLeft = a;
    if (a == d || appCt.offsetTop == a) {
        clearInterval(appScrollInterval);
    }
}

function appScroll(direction){
    if (typeof appScrollInterval != "undefined") 
        clearInterval(appScrollInterval);
    var scrollTo = 0;
    if (direction == "right") {
        MH.control.currentAppPage++;
    }
    else 
        if (direction == "left") {
            MH.control.currentAppPage--;
        }
        else {
            MH.control.currentAppPage = direction;
        }
    
    scrollTo = MH.control.appContainerWidth() * MH.control.currentAppPage;
    appScrollInterval = setInterval('appScrollRun(' + scrollTo + ', 20)', 10);
    if (MH.control.currentAppPage > 0) {
        document.getElementById('appLeft').style.visibility = "visible";
    }
    else {
        document.getElementById('appLeft').style.visibility = "hidden";
    }
    if (MH.control.currentAppPage < MH.control.totalAppPages - 1) {
        document.getElementById('appRight').style.visibility = "visible";
    }
    else {
        document.getElementById('appRight').style.visibility = "hidden";
    }
    
    document.getElementById('appPage').innerHTML = "";
    for (i = 0; i < MH.control.totalAppPages; i++) {
        if (MH.control.currentAppPage == i) {
            document.getElementById('appPage').innerHTML += '<div class="apppagedoton"></div>';
        }
        else {
            document.getElementById('appPage').innerHTML += '<div class="apppagedotoff" onclick="appScroll(' + i + ');"></div>';
        }
    }
}


function favZoomRun(fi_id, speed, d, callback){
   try{
   		var lifav = document.getElementById("li_" + fi_id);
	    if (lifav.style.margin == "") {
	        a = 0;
	    }
	    else {
	        a = parseInt(lifav.style.margin);
	    }
	    if (d == 1) {
	        a -= parseInt(145 / speed);
	    }
	    else {
	        a += parseInt(145 / speed);
	    }
	    if (a <= 5) 
	        a = 0;
	    lifav.style.margin = a + "px 0 0 0";
	    if (a <= 0 || a > 145) {
	        clearInterval(favZoomInterval);
	        if (callback != null) 
	            callback(fi_id);
	    }
   }catch(e){
   	 //alert(e.description);
   }
    
}

function favZoomIn(fi_id){
   
    try{
		if (typeof favZoomInterval != "undefined"){
			
        	clearInterval(favZoomInterval);
			
		}
		document.getElementById("li_" + fi_id).style.margin = "145px 0 0 0";
		
		favZoomInterval = setInterval('favZoomRun("' + fi_id + '",15,1,null)', 10);
		
	}catch(e){
		
	}
	
}

function favZoomOut(fi_id){
    try{
		if (typeof favZoomInterval != "undefined") 
        	clearInterval(favZoomInterval);
    		favZoomInterval = setInterval('favZoomRun("' + fi_id + '",15,0,renderFav)', 10);	
	}catch(e){
		//alert(e.description);
	}
	
}

function renderApp(){
    MH.control.currentAppPage = 0;
}

//设置不可拖拽元素
function cancelFav(div,id){
	try{
		var divId="#"+div;
		var cancelId="#"+id;
		$(divId).sortable("option","cancel",cancelId);
		//alert("4");
	}catch(e){
		//alert(e.name+"    "+e.description);
	}
}


function renderFav(currentPage){
	//alert("渲染");
	MH.interact.getFav();
	var favUL = document.getElementById("favUL");
	var appType, imgUrl, l, appId, appName;
	var ebank_url, credit_url, personal_url, history_url;
	var l = MH.favArr.length;
	favUL.innerHTML = "";
	var content_length = l * 196;
	
	document.getElementById('Favcontent').style.width = content_length + "px";
	document.getElementById('Favcontainer').style.width = content_length + "px";
	MH.control.favPageNum = parseInt(MH.control.screenWidth() / 196);
	if (MH.control.favPageNum == 0) 
		MH.control.favPageNum = 1;
	MH.control.currentPage = 0;
	
	//alert("MH.control.currentPage+="+MH.control.currentPage);
	/*根据页数显示*/
	if (l > MH.control.favPageNum) {
		/*页数大于1*/
		document.getElementById("Favcontainer").style.marginLeft = -MH.control.favPageNum * 98 + "px";
		MH.control.totalPages = parseInt((l - 1) / MH.control.favPageNum) + 1;
		content_length = MH.control.totalPages * MH.control.favPageNum * 196 + 1;
		document.getElementById('Favcontent').style.width = content_length + "px";
		document.getElementById('Favcontainer').style.width = MH.control.favContainerWidth() + "px";
		document.getElementById('FavLeft').style.visibility = "hidden";
		document.getElementById('FavRight').style.visibility = "visible";
		document.getElementById('favpage').innerHTML = '<div class="pagedoton"></div>';
		for (i = 1; i < MH.control.totalPages; i++) {
			document.getElementById('favpage').innerHTML += '<div class="pagedotoff" onclick="favScroll(' + i + ');"></div>';
		}
		document.getElementById('favpage').style.width = 18 * MH.control.totalPages + "px";
		document.getElementById('favpage').style.marginLeft = -9 * MH.control.totalPages + "px";
		document.getElementById('favpage').style.display = "";
	}
	else {
		/*页数小于1*/
		document.getElementById('FavLeft').style.visibility = "hidden";
		document.getElementById('FavRight').style.visibility = "hidden";
		document.getElementById("Favcontainer").style.marginLeft = -l * 98 + "px";
		document.getElementById('favpage').style.display = "none";
	}
	
	if (l != 0) {
		for (i = 0; i < l; i++) {
			var tempStr = MH.favArr[i];
			var favStatus = MH.favStatusArr[i];
			var types = tempStr.slice(0, 1);
			var li = document.createElement("li");
			
			switch (types) {
				case "a":
					appType = apptypes[1];
					break;
				case "b":
					appType = apptypes[2];
					break;
				case "c":
					appType = apptypes[3];
					break;
				case "d":
					appType = apptypes[4];
					break;
				case "e":
					appType = apptypes[5];
					break;
				case "f":
					appType = apptypes[6];
					break;
			}
			var bFound = false;
			for (var j in appList[appType]) {
				if (appList[appType][j].id == MH.favArr[i]) {
					imgUrl = appList[appType][j].favUrl;
					appName = appList[appType][j].name;
					appId = appList[appType][j].id;
					ebank_url = appList[appType][j]['sublink'].ebank;
					ebank_txt = appList[appType][j]['sublink'].txt_ebank;
					vip_url = appList[appType][j]['sublink'].vip;
					vip_txt = appList[appType][j]['sublink'].txt_vip;
					bFound = true;
				}
			}
			if (!bFound) 
				continue;
			var strHTML = "";
			if (favStatus < 200) {
				strHTML = '<div class="favitem"><div class="textFav" id="pro_' + appId + '">';
				if (favStatus < 100) {
					strHTML += "正在下载";
				}
				else {
					strHTML += "正在安装";
				}
				strHTML += '</div>';
				//strHTML += '<div id="usb_"'+appId+' class="procUsb"><div class="nnn" id="usbPr_' + appId + '"></div></div>';
				//alert("usb_"+appId);
				strHTML += '<div class="favlogo" style="background-image:url(' + imgUrl + ');">' + '<div class="prow">';
				strHTML += '<div class="procCover" id="shadow_' + appId + '"><b class="rtop"><b class="r1"></b><b class="r2"></b><b class="r3"></b><b class="r4"></b></b><table height="132px"><tr><td><br></td></tr></table><b class="rbottom"><b class="r4"></b><b class="r3"></b><b class="r2"></b><b class="r1"></b></b></div></div>';
				strHTML += '<div class="proc" id="status_' + appId + '"><div class="nn" id="pr_' + appId + '"></div></div>';
			}
			else {
				//strHTML = '<span class="delFav" onclick =deleteFavAction('+appId+',"' + appId + '")>"' + appId + '"</span>';
				strHTML = '<span class="delFav" onclick =deleteFavAction("' + appId + '")></span>';
				//alert("usb_"+appId);
				//strHTML = '<div id="usb_"'+appId+' class="procUsb"><div class="nn" id="usbPr_' + appId + '"></div></div>';
				strHTML += '<div class="favitem">';
				strHTML += '<div class="textFav" id="pro_' + appId + '"></div>';
				strHTML += '<div id="usb_'+appId+'" class="procUsb"><div class="nnn" id="usbPr_' + appId + '"></div></div>';
				strHTML += '<div class="favlogo" style="background-image:url(' + imgUrl + ');">';
				strHTML += '<dl class="itemoption" style="display:none">';
				strHTML += '<dt id="drop_' + appId + '" class="dt" onmouseover="javascript:changeCss(\'drop_' + appId + '\',\'over\');" onmousedown="javascript:changeCss(\'drop_' + appId + '\',\'down\');" onmouseup="javascript:changeCss(\'drop_' + appId + '\',\'release\');" onmouseout="javascript:changeCss(\'drop_' + appId + '\',\'out\');">' + appName + '</dt>' + "<dd><a href=javascript:MH.control.openLink('url','" + appId + "')><span>首&nbsp;&nbsp;页</span></a></dd>";
				//strHTML += '<dt id="drop_' + appId + '" class="dt" onmouseup="javascript:alert(\'over2\');">' + appName + '</dt>' + "<dd><a href=javascript:MH.control.openLink('url','" + appId + "')><span>首&nbsp;&nbsp;页</span></a></dd>";
				//if(appId=="e001"){
					if (ebank_url == undefined) {
						strHTML += '<dd><a href="javascript:;"></a></dd>';
					}
					else {
						if (favStatus < 200) {
							strHTML += "<dd><a href=javascript:;><span>" + ebank_txt + "</span></a></dd>";
						}
						else {
							strHTML += "<dd><a href=javascript:MH.control.openLink('ebank','" + appId + "')><span>" + ebank_txt + "</span></a></dd>";
						}
					}
					
					if (vip_url == undefined) {
						strHTML += '<dd><a href="javascript:;"></a></dd>';
					}
					else {
						if (favStatus < 200) {
							strHTML += "<dd><a href='javascript:;'></dd>";
						}
						else {
							strHTML += "<dd><a href=javascript:MH.control.openLink('vip','" + appId + "')><span>" + vip_txt + "</span></a></dd>";
						}
					}
				/*	
				}else{
					if (ebank_url == undefined) {
						strHTML += '<dd><a href="javascript:;"></a></dd>';
					}
					else {
						if (favStatus < 200) {
							strHTML += "<dd><a href=javascript:;></a></dd>";
						}
						else {
							strHTML += "<dd><a href=javascript:;></a></dd>";
						}
					}
					
					if (vip_url == undefined) {
						strHTML += '<dd><a href="javascript:;"></a></dd>';
					}
					else {
						if (favStatus < 200) {
							strHTML += "<dd><a href='javascript:;'></a></dd>";
						}
						else {
							strHTML += "<dd><a href='javascript:;'></a></dd>";
						}
					}
					
				}
				*/
					
			}
			strHTML += '<dd class="laone"><a href="javascript:;"></a></dd>' + '</dl>' + '</div></div>';
			li.innerHTML = strHTML;
			li.id = "li_" + appId;
			//添加禁止拖动的效果
			try{
				if (favStatus < 200) {
				MH.dom.addClass(li, 'downloading');
				fav("favUL", li.id);
				//alert("ok="+li.id+" status=="+favStatus );
				}
				else {
					$('#favUL').sortable('option', 'handle', 'dt');
					//alert("error="+li.id);
				}	
			}catch(e){
				//alert(e.message);
			}
			
			//添加drop
			try {
				li['onmouseover'] = (function(liEl){
					return function(e){
					
						var e = window.event ? window.event : e;
						var fromEl = e.srcElement ? e.srcElement : e.target;
						var toEl = e.toElement ? e.toElement : e.relatedTarget;
						var target = liEl.getElementsByTagName("dl")[0];
						var _para;
						liEl.className = "hvfav";
						if (MH.control.checkType(target)) {
							return;
						}
						if (liEl == fromEl && !MH.dom.isChild(toEl, liEl) || MH.dom.isChild(fromEl, liEl)) {
							showEnt(target, _para = {
								t: 0,
								begin: 30,
								change: 112,
								duration: 50
							});
						}
					}
				})(li)
				var method = (window.event) ? 'onmouseleave' : 'onmouseout';
				li['onmouseout'] = (function(liEl){
					return function(e){
						var e = window.event ? window.event : e;
						var fromEl = e.srcElement ? e.srcElement : e.target;
						var toEl = e.toElement ? e.toElement : e.relatedTarget;
						var target = liEl.getElementsByTagName("dl")[0];
						var _para;
						if (toEl != liEl && !MH.dom.isChild(toEl, liEl) || fromEl.tagName.toLowerCase() == 'a' && !(toEl)) {
							liEl.className = "";
							hideEnt(target, _para = {
								t: 0,
								begin: 100,
								change: -100,
								duration: 50
							});
						}
						else 
							if (!MH.dom.isChild(toEl, liEl) && !MH.dom.isChild(fromEl, liEl) || !toEl) {
								return;
							}
					}
				})(li)
			} 
			catch (e) {
				alaer(e.message);			
			}
			favUL.appendChild(li);
			if (favStatus <= 100) {
				var t = '#pr_' + appId;
				$(t).width(favStatus);
			}
		}
	}
	else {
		displayAppNav();
	}
}


//下载参数方法
function setDownloadStatus(appId, spro, param3, param4){
    //param3 文字
	//param4  状态
	
	try {
		if(param4==0){
			var t = '#pr_' + appId;
			var status='#status_' + appId;
	        $(t).width(spro);
	        var divTextID = "#pro_" + appId;
	        if (spro < 100) {
	            $(divTextID).empty()
	            $(divTextID).append(param3);
				$(status).show();
	        }
	        if (100 <= spro < 200) {
	            $(divTextID).empty()
	            $(divTextID).append(param3);
				$(status).show();
	         }
	        if (spro == 200){
	            $(divTextID).empty();
				setTimeout(function(){
                var shadow = "#shadow_" + appId;
                $(shadow).hide();
            	}, 100);
				$(status).hide();
				renderFav();
	        }	
		}else{
			//strHTML += '<div id="usb_"'+appId+' class="procUsb" style="display:none;"><div class="nn" id="usbPr_' + appId + '"></div></div>';
			//alert(param4+"::"+param3+"::"+spro);
			var usb = "#usb_" + appId;
			//alert(usb);
			try{
				var t = '#usbPr_' + appId;
				$(usb).show();
				var divTextID = "#pro_" + appId;
			}
			catch(e){
				//alert("e:::::"+e.description);
			}
			
			if (spro < 100) {
	            try{
					$(divTextID).empty()
		            $(divTextID).append(param3);
					$(usb).show();
					$(t).width(spro);
				}catch(e){
					//alert("spro < 100"+e.description);
				}
				
	        }
	        if (100 <= spro < 200) {
	            $(divTextID).empty()
	            $(divTextID).append(param3);
				$(usb).show();
				
				$(status).show();
	         }
	        if (spro == 200){
	            $(divTextID).empty();
				setTimeout(function(){
                $(usb).hide();
            	}, 100);
				renderFav();
	        }
	        
        }
        
    } 
    catch (e) {
        //alert("errorReport:"+e.message+"   "+e.description);
    }
}



/*
 当fav区为空时，显示引导标志
 */
function displayAppNav(){
    try {
        var content_length = 196;
        document.getElementById("Favcontainer").style.marginLeft = -98 + "px";
        document.getElementById('Favcontent').style.width = content_length + "px";
        document.getElementById('Favcontainer').style.width = content_length + "px";
        var favUL = document.getElementById("favUL");
        var li = document.createElement("li");
        var strHTML = '<div class="favitem" id="favitem" onclick="javascript:appDisplay()"><div class="favNav" id="favNav"></div></div>';
        li.innerHTML = strHTML;
        favUL.appendChild(li);
        document.getElementById("favNav").style.backgroundImage = "url(images/cross.gif)";
        document.getElementById("favitem").style.cursor = "pointer";
    } 
    catch (e) {
        
    }
}

function appDisplay(){
    changePic(1);
    document.getElementById("app_box").style.display = "";
	//alert("test");
	//renderFav();
    MH.control.appViewResize();
}

function clone(myObj){
    if (typeof(myObj) != 'object') 
        return myObj;
    if (myObj == null) 
        return myObj;
    var myNewObj = new Object();
    for (var i in myObj) {
        myNewObj[i] = clone(myObj[i]);
    }
    return myNewObj;
}

function MergeRecursive(obj1, obj2){
    var destObj = clone(obj1);
    for (var p in obj2) {
        try {
            if (obj2[p].constructor == Object) {
                destObj[p] = MergeRecursive(destObj[p], obj2[p]);
            }
            else {
                destObj[p] = obj2[p];
            }
        } 
        catch (e) {
            destObj[p] = obj2[p];
        }
    }
    return destObj;
}

function shiftTab(target){
    var ul = document.getElementById("bank_tabs_title");
    var lis = ul.getElementsByTagName("li");
    var lilength = lis.length;
    
    var ori_index = 0;
    var tl = target.length;
    for (var i = 0; i < lilength; i++) {
        lis[i].setAttribute("apptype", apptypes[i]);
        var apptype = lis[i].getAttribute("apptype");
        if (apptype == target.substring(1, tl)) {
            MH.dom.addClass(lis[i], "click");
            if (apptype == "all") {
                currentAppArray = MergeRecursive(appList["banks"], appList["payments"]);
                currentAppArray = MergeRecursive(currentAppArray, appList["securities"]);
                currentAppArray = MergeRecursive(currentAppArray, appList["insurances"]);
                currentAppArray = MergeRecursive(currentAppArray, appList["funds"]);
                currentAppArray = MergeRecursive(currentAppArray, appList["shopping"]);
            }
            else {
                currentAppArray = appList[apptype];
               
            }
            handle(currentAppArray);
            ori_index = i;
        }
        lis[i].onclick = (function(n){
            return function(){
                if (lis[n].className == "cur") {
                    return false;
                }
                else {
                    MH.dom.removeClass(lis[ori_index], "click");
                    MH.dom.addClass(lis[n], "click");
                    selector.value = "";
                    ori_index = n;
                    selector.onkeyup = function(){
                        return findName(selector.value, apptypes[n]);
                    };
                    if (n == 0) {
                        currentAppArray = MergeRecursive(appList["banks"], appList["payments"]);
                        currentAppArray = MergeRecursive(currentAppArray, appList["securities"]);
                        currentAppArray = MergeRecursive(currentAppArray, appList["insurances"]);
                        currentAppArray = MergeRecursive(currentAppArray, appList["funds"]);
						currentAppArray = MergeRecursive(currentAppArray, appList["shopping"]);
                    }
                    else {
                        currentAppArray = appList[lis[n].getAttribute("apptype")];
                    }
                    MH.control.currentAppPage = 0;
                    MH.dom.$("appContainer").scrollLeft = 0;
                    
                    handle(currentAppArray);
                    window.location.hash = "#" + apptypes[n];
                }
            }
        })(i);
    }
}

function reduces(appId){
    /*
    
     var oNode=document.getElementById(appId);
    
     if(parseInt(oNode.childNodes[3].style.width)>20){
    
     oNode.childNodes[3].style.width=parseInt(oNode.childNodes[3].style.width)-1+"px";
    
     oNode.childNodes[3].style.height=parseInt(oNode.childNodes[3].style.height)-1+"px";
    
     oNode.childNodes[3].style.top=4-(36-parseInt(oNode.childNodes[3].style.width))/1.5+"px";
    
     oNode.childNodes[3].style.left=parseInt(oNode.childNodes[3].style.left)+4+"px";
    
     if(MH.control.IEVersion()>6){
    
     oNode.childNodes[4].style.width=parseInt(oNode.childNodes[4].style.width)-1+"px";
    
     oNode.childNodes[4].style.height=parseInt(oNode.childNodes[4].style.height)-1+"px";
    
     oNode.childNodes[4].style.top=4-(36-parseInt(oNode.childNodes[3].style.width))/1.5+"px";
    
     oNode.childNodes[4].style.left=parseInt(oNode.childNodes[4].style.left)+4+"px";
    
     }
    
     }else{
    
     clearInterval(bigStarInterval[appId]);
    
     oNode.childNodes[2].style.display="none";
    
     oNode.childNodes[3].style.display="none";
    
     oNode.childNodes[3].style.width="36px";
    
     oNode.childNodes[3].style.height="36px";
    
     oNode.childNodes[3].style.top="4px";
    
     oNode.childNodes[3].style.left="54px";
    
     if(MH.control.IEVersion()>6){
    
     oNode.childNodes[4].style.display="none";
    
     oNode.childNodes[4].style.width="36px";
    
     oNode.childNodes[4].style.height="36px";
    
     oNode.childNodes[4].style.top="4px";
    
     oNode.childNodes[4].style.left="54px";
    
     }
    
     }
    
     */
    
}

//取得长度
function getPropertyCount(o){
    var n, count = 0;
    for (n in o) {
        if (o.hasOwnProperty(n)) {
            count++;
        }
    }
    return count;
}

/*显示appbox时其他层高度调整*/
function adjustMargin(){
}





/*改写生成方法*/
function handle(jsonArray){
    
    MH.interact.getFav();
    
    //取得app总数
    appLength = getPropertyCount(jsonArray);
    var l = appLength;
    try {
        var content_length = appLength * MH.control.appLiWeight;
        
        try {
        
            document.getElementById('appContent').style.width = content_length + "px";
            
            MH.control.appPageNum = parseInt(MH.control.screenWidth() / MH.control.appLiWeight);
            document.getElementById('appContainer').style.width = (MH.control.appPageNum * MH.control.appLiWeight) + "px";
            
            var bordermargin = parseInt((MH.control.screenWidth() - MH.control.appPageNum * MH.control.appLiWeight) / 2);
            
            document.getElementById('appContainer').style.margin = "0 " + bordermargin + "px 0 " + bordermargin + "px";
            document.getElementById('appContainer').style.marginLeft = -parseInt((MH.control.appPageNum * MH.control.appLiWeight) / 2) + "px";
            
            if (MH.control.appPageNum == 0) 
                MH.control.appPageNum = 1;
            
            MH.control.currentAppPage = 0;
            document.getElementById('appContent').style.height = "216px";
        } 
        catch (e) {
            
        }
        /*根据页数显示*/
        
        if (l > (MH.control.appPageNum * 3)) {
            if (appLength % (MH.control.appPageNum * 3) > 0) {
                MH.control.totalAppPages = parseInt(l / (MH.control.appPageNum * 3)) + 1;
            }
            else {
                MH.control.totalAppPages = parseInt(l / (MH.control.appPageNum * 3));
            }
            content_length = MH.control.totalAppPages * MH.control.appPageNum * MH.control.appLiWeight + 1;
            
            document.getElementById('appContent').style.width = content_length + "px";
            
            document.getElementById('appLeft').style.visibility = "hidden";
            document.getElementById('appRight').style.visibility = "visible";
            document.getElementById('appPage').innerHTML = '<div class="apppagedoton"></div>';
            
            for (i = 1; i < MH.control.totalAppPages; i++) {
                document.getElementById('appPage').innerHTML += '<div class="apppagedotoff"  onclick="appScroll(' + i + ');"></div>';
            }
            
            document.getElementById('appPage').style.width = 18 * MH.control.totalAppPages + "px";
            document.getElementById('appPage').style.marginLeft = -9 * MH.control.totalAppPages + "px";
            document.getElementById('appPage').style.display = "";
        }
        else {
            document.getElementById('appContainer').style.textAlign = "left";
            MH.control.totalAppPages = 1;
            document.getElementById('appLeft').style.visibility = "hidden";
            document.getElementById('appRight').style.visibility = "hidden";
            //document.getElementById("appContainer").style.marginLeft=-98+"px";
            document.getElementById('appPage').style.display = "none";
        }
        
        //原始方法
        var list_1 = document.createElement("ul");
        var list_2 = document.createElement("ul");
        var list_3 = document.createElement("ul");
        
        /*
         if(MH.favStr){
         MH.favArr=MH.favStr.split("&");
         }
         */
        var setAttrToElem = function(elem, elemIndex){
            elem.setAttribute("apptype", jsonArray[elemIndex].apptype);
            elem.setAttribute("id", jsonArray[elemIndex].id);
            elem.setAttribute("arrayposition", jsonArray[elemIndex].position);
            elem.setAttribute("collected", jsonArray[elemIndex].collected);
        }
        showlist_1.innerHTML = "";
        showlist_1.appendChild(list_1);
        showlist_1.appendChild(list_2);
        showlist_1.appendChild(list_3);
        
        //开始循环写入
        var start = 0;
        for (var o in jsonArray) {
            var li = document.createElement("li");
            var tid = jsonArray[o].id;
            li.id = "li_" + tid;
            var l = MH.favArr.length;
            if (l > 0) {
                for (n in MH.favArr) {
                    if (tid == MH.favArr[n]) {
                        li.className = 'hover';
                    }
                }
            }
            var content = "";
            content += "<span id='1_" + tid + "' class='existFav'></span>" + "<img src='" + jsonArray[o].imageUrl + "'/>";
            content += "<span id='2_" + tid + "' class='graybg' style='display:none;'></span>";
            content += "<span id='3_" + tid + "' style='display:none;width:36px;height:36px;top:4px;left:54px;'></span>";
            content += "<img id='4_" + tid + "' class='bigstarimg' src='images/del.png' style='display:none;width:36px;height:36px;top:4px;left:54px;' />";
            
            li.onclick = (function(that, id){
                
                return function(){
                    addFavAction(that, id);
                }
            })(li, tid);
            
            li.innerHTML = content;
            li.onclick = (function(that, id){
                return function(){
                    if (that.className != "hover") {
                        addFavAction(that, id);
                    }
                    else {
                        delFavAction(that, id);
                    }
                }
            })(li, tid);
            
            
            li.onmouseover = (function(that, id){
                return function(){
                    var a = "#2_" + id;
                    var b = "#3_" + id;
                    var c = "#4_" + id;
                    $(a).show();
                    if (that.className == "hover") {
                        $(b).removeClass().addClass("bigstar2").show();
                        if (MH.control.IEVersion() > 6) {
                            $(c).attr("src", "images/del.png").show();
                        }
                    }
                    else {
						$(b).removeClass().addClass("bigstar1").show();
                        if (MH.control.IEVersion() > 6) {
                            $(c).attr("src", "images/add.png").show();
                        }
                    }
                }
            })(li, tid);
            li.onmouseout = (function(that, id){
                return function(){
                    var a = "#2_" + id;
                    var b = "#3_" + id;
                    var c = "#4_" + id;
                    if (that.className != "hover") {
                        $(a).hide();
                        $(b).hide();
                        if (MH.control.IEVersion() > 6) {
                            $(c).hide();
                        }
                    }
                    else {
                        $(a).hide();
                        $(b).hide();
                        if (MH.control.IEVersion() > 6) {
                            $(c).hide();
                        }
                    }
                }
            })(li, tid);
            setAttrToElem(li, o);
            if ((parseInt((start) / MH.control.appPageNum) + 3) % 3 == 0 && 0 <= (start) % MH.control.appPageNum <= MH.control.appPageNum) {
            
                list_1.appendChild(li);
            }
            if ((parseInt((start) / MH.control.appPageNum) + 3) % 3 == 1 && 0 <= (start) % MH.control.appPageNum <= MH.control.appPageNum) {
            
                list_2.appendChild(li);
            }
            if ((parseInt((start) / MH.control.appPageNum) + 3) % 3 == 2 && 0 <= (start) % MH.control.appPageNum <= MH.control.appPageNum) {
            
                list_3.appendChild(li);
            }
            start++;
        }
    } 
    catch (e) {
        
    }
}

function addFavAction(oNode, appId){
    var cls = oNode.className;
    
    if (cls != "hover") {
        bigStarInterval[appId] = setInterval('reduces("' + appId + '")', 10);
        oNode.className = "hover";
        oNode.getElementsByTagName('span')[0].className = "existFav";
        MH.interact.saveFav(appId);
       	favZoomIn(appId);
		renderFav();
		favPayAlert(appId);
    }
    
}


//支付收藏提醒
function favPayAlert(appId){
	switch(appId){
		case "e001":
		alert("如果您需要在支付宝中通过网银充值或支付,请同时收藏相关的银行");
		break;	
	}
	
}

function delFavAction(o, appId){
    try{
    	
		MH.interact.deleteFav(appId);
    	MH.interact.getFav();
		favZoomOut(appId);
		if(document.all(appId)!=null){   
       		document.getElementById(appId).className = "";   
		}
	}catch(e){
		//alert(e.description);
	}
	
}

function deleteFavAction(appId){
    try{
    	MH.interact.deleteFav(appId);
    	MH.interact.getFav();
    	favZoomOut(appId);
		if(document.all(appId)!=null){   
       		document.getElementById(appId).className = "";   
		}
	}catch(e){
		alert(e.description);
	}
	
}



function searchIndex(name, type){
    var resultArray = [];
    var targetArray = [];
    var tempArray = [];
    if (type == "all") {
        for (i in appList) {
            targetArray = appList[i].concat(targetArray);
        }
    }
    else {
        targetArray = appList[type];
    }
    var _stringLength = name.length;
    for (var i in targetArray) {
        var indexArray = targetArray[i].index.split(",");
        var indexLength = indexArray.length;
        for (j = 0; j < indexLength; j++) {
            if (name === indexArray[j].slice(0, _stringLength)) {
                resultArray.push(targetArray[i]);
                break;
            }
        }
    }
    return resultArray;
}


function renderAd(){
   /*
    var test;
	try {
		test=window.external.GetPhpData('http://moneyhub.ft.com/banksite/getfinprod.php?debug=1&encode=0');
	}catch(e){alert("excep1="+e.description);}
	var content="";
	var count=0;
	var styleTab="";
	var allPage=0;
	var curPage=0;
	var pageNumber=3;
	if (test != "") {
		content += '<div id="tabs1">';
		try{
		$.ajax({
			url: test,
			type: 'GET',
			dataType: 'html',
			timeout: 2000,
			error: function(xmlData){
				alert('Error loading XML document' + xml);
			},
			success: function(xmlData){
				if( typeof xmlData == "string" ){
                        var parser = new ActiveXObject('Microsoft.XMLDOM');
						parser.async = 'false';
						parser.loadXML(xmlData);
						xml = parser.documentElement;
				} else {
                         xml = data; 
                }
				$(xml).find("finprod").each(function(i){
					count++;
				});
				try{
					if ((count % pageNumber) == 0) allPage = parseInt(count / pageNumber);
					else allPage = parseInt(count / pageNumber) + 1;
					curPage = 1;
					content += '<ul>';
					for (var i = 1; i < (allPage+1); i++) {
						if(i==curPage){
							content += '<li id="child' + i + '" class="current"><a href=""><span>' + i + '</span></a></li>';
						} else{
							content += '<li id="child' + i + '"><a href=""><span>' + i + '</span></a></li>';	
						}
						
					}	
				}catch(e) {"excep2="+alert(e.message);}
				
				
				
				content += '</ul>';
				
				$("#desc").append(content);
				var f=0;
				$(xml).find("finprod").each(function(i){
					if((f%pageNumber)==0){
						if(parseInt(f/pageNumber) + 1==curPage){
							content+='<div id="" style="display:block"><TABLE style="BORDER-COLLAPSE: collapse" borderColor=#BCD2E6 cellSpacing=0 width=300 align=center bgColor=#ffffff border=1>';	
						}else{
							content+='<div id="" style="display:none"><TABLE style="BORDER-COLLAPSE: collapse" borderColor=#BCD2E6 cellSpacing=0 width=300 align=center bgColor=#ffffff border=1>';
						}
						content+='<tr><td>认购开始</td><td>认购截止</td><td>理财期限</td><td>年化收益率</td></tr>';
					}
					content+='<tr><td colspan="4">'+$(this).children("desc").text()+'</td></tr>';
					content+='<tr><td>'+$(this).children("startdate").text()+'</td><td>'+$(this).children("enddate").text()+'</td><td>'+$(this).children("duration").text()+'</td><td>'+$(this).children("interest").text()+'</td></tr>';
					if((f%pageNumber)==(pageNumber-1)){
						content+="</table>";
						content+="</div>";
					}
					f++;
				});
			}
		});
		}catch(e){alert(e.message);}
		content+='</div>';
		$("#finatech_ad").append(content);
	}else {
		alert("error");
	}
	*/
	
	var parent = MH.dom.$('finatech_ad');	
    //parent.innerHTML = '<iframe id="textAd" src="./ad/textad.html" frameborder="0" border="0" width="450px" height="100%" style="border:none; border-color:none; scrolling:yes;"></iframe>';
	parent.innerHTML = '<iframe id="textAd" src="./ad/textad.html" frameborder="0" border="0" height="100%" style="border:none; border-color:none; scrolling:yes;"></iframe>';	
}


/*
 0,显示+;
 1,显示-;
 */
function changePic(status){
    var contentStr = "";
    var disPic = "images/aplus.png";
    if (status == 1) {
        disPic = "images/aplus_.png";
    }
    contentStr = '<img src="' + disPic + '" style="padding-top:2px;"/>';
    
    MH.dom.$('aplusspan').innerHTML = "";
    MH.dom.$('aplusspan').innerHTML = contentStr;
}

/*
 *执行jquery的sortable
 */
var prevPagesOrder = "";
var curFavArray = "";

/*
 function destroySort(){
 $( "#favUL" ).sortable("destroy");
 }
 */
function sortableFav(){
    $("#favUL").sortable({
        revert: true,
        scroll: true,
        //cancel: '.downloading',
        axis: 'x',
        start: function(event, ui){
            var textId = "#pro_" + $(ui.item).attr("id").substring(3, $(ui.item).attr("id").length);
            $(textId).hide(300);
        },
        stop: function(event, ui){
            var textId = "#pro_" + $(ui.item).attr("id").substring(3, $(ui.item).attr("id").length);
            $(textId).show(300);
        },
        
        update: function(event, ui){
            var items = $('#favUL').sortable('option', 'items');
			//alert("item="+item);
			var cancel = $('.selector').sortable('option', 'cancel');
			//alert(cancel.id);
		    var xx = $("#favUL").sortable("serialize");
            var curFavArray = $("#favUL").sortable("toArray");
            
            var l = curFavArray.length;
            var end = 0;
            var start = 0;
            
            for (var i = 0; i < l; i++) {
                if ($(ui.item).attr("id") == curFavArray[i]) {
                    end = i + 1;
                    break;
                }
            }
            for (var i = 0; i < l; i++) {
                if ($(ui.item).attr("id").substring(3, $(ui.item).attr("id").length) == MH.favArr[i]) {
                    start = i + 1;
                    break;
                }
            }
            try {
                window.external.ChangeOrder($(ui.item).attr("id").substring(3, $(ui.item).attr("id").length), end, start);
            } 
            catch (e) {
                
            }
            //renderFav();
        }
    });
}

//C++调用方法
function favAction(appId){
    try {
        var liId = "#li_" + appId;
        
        if ($(liId).className != "hover") {
            bigStarInterval[appId] = setInterval('reduces("' + appId + '")', 10);
            $(liId).className = "hover";
            $("#1_" + appId).removeClass().addClass("existFav");
            favZoomIn(appId);
			renderFav();
			MH.interact.saveFav(appId);
            handle(currentAppArray);
			favPayAlert(appId);
        }
    } 
    catch (e) {
      
    }
    
}




function changeCss(div,status){
	try {
		$('#' + div).removeClass();
		if (status == "over") {
			$('#' + div).addClass("dt1");
		}
		if (status == "down") {
			$('#' + div).addClass("dt2");
		}
		if (status == "release") {
			$('#' + div).addClass("dt");
		}
		if (status == "out") {
			$('#' + div).addClass("dt");
		}
	}catch(e){
		//alert(e.message);
	}
}

function setAlarm(){
	MH.control.checkAlarm();
	//alert("执行更新");
}
