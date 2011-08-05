	function Pid(id,tag){
	    if(!tag){
		return document.getElementById(id);
	    }
	    else{
		return document.getElementById(id).getElementsByTagName(tag);
	    }
	}
	function tab_change(id,hx,box,iClass,s,pr){
	    var hxs=Pid(id,hx);
	    var boxs=Pid(id,box);
	    if(!iClass){ // 如果不指定class，则：
		boxsClass=boxs; // 直接使用box作为容器
	    }
	    else{ // 如果指定class，则：
		var boxsClass = [];
		for(i=0;i<boxs.length;i++){
		    if(boxs[i].className.match(/\bdSetRight\b/)){// 判断容器的class匹配
			boxsClass.push(boxs[i]);
		    }
		}
	    }
	    if(!pr){ // 如果不指定预展开容器，则：
		go_to(0); // 默认展开序列
		yy();
	    }
	    else {
		go_to(pr);
		yy();
	    }
	    function yy(){
		for(var i=0;i<hxs.length;i++){
		    hxs[i].temp=i;
		    if(!s){// 如果不指定事件，则：
			s="onclick"; // 使用默认事件
			hxs[i][s]=function(){
			    go_to(this.temp);
			}
		    }
		    else{
			hxs[i][s]=function(){
			    go_to(this.temp);
			}
		    }
		}
	    }
	    function go_to(pr){
		for(var i=0;i<hxs.length;i++){
		    if(!hxs[i].tmpClass){
			hxs[i].tmpClass=hxs[i].className+=" ";
			boxsClass[i].tmpClass=boxsClass[i].className+=" ";
		    }
		    if(pr==i){
			hxs[i].className+=" up"; // 展开状态：标题
			boxsClass[i].className+=" up"; // 展开状态：容器
		    }
		    else {
			hxs[i].className=hxs[i].tmpClass;
			boxsClass[i].className=boxsClass[i].tmpClass;
		    }
		}
	    }
	}
	tab_change("dSetPage","h3","div","dSetRight");