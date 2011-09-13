function init(){
    var couponsList = Tesy.interact.getCoupons();
    var wrap = Tesy.dom.$('favlist');
    wrap.innerHTML = "";
    
    function rendcoupon(obj, oNode){
    	
        var itemW = document.createElement('div');
        var itemC = Tesy.dom.cE('div');
        var itemL = Tesy.dom.cE('div');
        var adrs = Tesy.interact.getAddress();
        
        itemW.id = obj.id;
        itemW.className = "couponitem";
        itemC.innerHTML = '<span class="del_btn"><span class="del_bg"></span><span class="del_ac" onclick="showCover(' + obj.id + ')"></span></span>';
        itemC.className = "imgctt";
        var DateA = new Date(obj.expire.replace(/\-/g, "/"));
        DateA.setHours(23);
        var Today = new Date();
	Today.setHours(0);
        if (DateA < Today) {
        	itemC.innerHTML += '<span class="expire_bg"></span><a href="' + adrs + obj.id + '.htm" target="_blank"><img alt="" src="' + adrs + obj.id + 'M.jpg" style="filter:gray;"></a>';
	} else {
        	itemC.innerHTML += '<a href="' + adrs + obj.id + '.htm" target="_blank"><img alt="" src="' + adrs + obj.id + 'M.jpg"></a>';
	}
        
        itemL.className = "layctt";
		itemL.id = "cover" + obj.id;
        itemL.innerHTML = "<div class='coverbg'></div>";
        
        var del = Tesy.dom.cE('span');
        var cancel = Tesy.dom.cE('span');
        
        del.className = "del";
        cancel.className = "cancel";
        
        itemW.onmouseover = (function(itemW){
            return function(){
                Tesy.dom.addClass(itemW, "hover");
            }
        })(itemW);
        
        itemW.onmouseout = (function(itemW){
            return function(){
                Tesy.dom.removeClass(itemW, "hover");
            }
        })(itemW);
        
		del.onclick = (function(id){
			return function(){
				delCoupon(id);
			}
		})(obj.id)
		
		cancel.onclick = (function(id){
			return function(){
				hideCover(id)
			}
		})(obj.id);
		
        itemL.appendChild(del);
        itemL.appendChild(cancel);
        
        itemW.appendChild(itemC);
        itemW.appendChild(itemL);
        oNode.appendChild(itemW);
    }
    
    for (var i = 0, l = couponsList.length; i < l; i++) {
        rendcoupon(couponsList[i], wrap);
    }
    
    initResize();
}

function showCover(id){
	var oNode = Tesy.dom.$('cover' + id);
	oNode.style.display = 'block';
}

function hideCover(id){
	var oNode = Tesy.dom.$('cover' + id);
	oNode.style.display = 'none';
}

function delCoupon(id){
    var target = Tesy.dom.$(id);
    if (target.nodeType == 1) {
        target.parentNode.removeChild(target);
        Tesy.interact.delCoupon(id);
    }
    else {
        return false;
    }
}


function initResize(){
    var tspan = 24;
    var tpadding = 10;
    function ajustHeight(){
        var favlist = Tesy.dom.$('favlist');
        favlist.style.height = document.body.offsetHeight - tspan - tpadding + "px";
    }
    window.onresize = ajustHeight;
    ajustHeight();
}


