lastScrollY=0;
function heartBeat(){ 
var diffY;
if (document.documentElement && document.documentElement.scrollTop)
    diffY = document.documentElement.scrollTop;
else if (document.body)
    diffY = document.body.scrollTop
else
    {/*Netscape stuff*/}
    
//alert(diffY);
percent=.1*(diffY-lastScrollY); 
if(percent>0)percent=Math.ceil(percent); 
else percent=Math.floor(percent); 
document.getElementById("lovexin12").style.top=parseInt(document.getElementById("lovexin12").style.top)+percent+"px";

lastScrollY=lastScrollY+percent; 
//alert(lastScrollY);
}
suspendcode12="<DIV id=\"lovexin12\" style='right:5px;POSITION:absolute;TOP:180px;z-index:100'>";
var recontent='<table align="left" style="margin-right:0px;width:90px" border="0" cellpadding=0 cellspacing=0 height="32">' + 
'<tr>' + 
'<td style="padding:0;font-size:13px" height="32" ><table border="0" cellspacing="0" cellpadding="0" height="1">' + 
'<tr>' + 
'<td style="padding:0;font-size:13px; background:none" height="20"><img src="http://www.slgz.cn/sanqiqq/images/qq/qq_top.gif"  border="0" usemap="#MapMapMap"  width="90" height="33"></td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding:0;font-size:13px;padding-left:1px" background="http://www.slgz.cn/sanqiqq/images/qq/qq_center.gif" height="19">' + 
'<table style="width:86px"  border="0" align="center" cellpadding="0" cellspacing="0" height="1">' +  
'<tr>' + 
'<td style="padding:0;padding-left:1px;padding-right:4px;" >' + 
'<table  border="0" align="left" cellpadding="0" cellspacing="0" >' + 
'<!--begin-->' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://wpa.qq.com/pa?p=1:844974664:4 alt="" align="right"/></td>' + 
'<td style="background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0; padding-left:3px; background-position: 0%" width="55" height="24" align="left">' + 
'<a style="color:#F14400;" target=blank href=http://wpa.qq.com/msgrd?V=1&Uin=844974664&Site=www.slgz.cn&Menu=yes title="为了更好的为您服务,请加在线客服为好友,QQ:844974664">客服①号' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://wpa.qq.com/pa?p=1:93942916:4 alt="" align="right"/></td>' + 
'<td style=" background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0;  padding-left:3px;background-position: 0%" height="24" align="left">' + 
'<a style="color:#F14400;" target=blank href=http://wpa.qq.com/msgrd?V=1&Uin=93942916&Site=www.slgz.cn&Menu=yes title="为了更好的为您服务,请加在线客服为好友,QQ:93942916">客服②号' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://wpa.qq.com/pa?p=1:93942916:4 alt="" align="right"/></td>' + 
'<td style=" background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0;  padding-left:3px;background-position: 0%" height="24" align="left">' + 
'<a style="color:#F14400;" target=blank href=http://wpa.qq.com/msgrd?V=1&Uin=93942916&Site=www.slgz.cn&Menu=yes title="为了更好的为您服务,请加在线客服为好友,QQ:93942916">客服③号' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://www.slgz.cn/sanqiqq/images/qq/qq_01.gif alt="" align="right" width="16" height="16"/></td>' + 
'<td style=" background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0;  padding-left:3px;background-position: 0%" height="24" align="left">' + 
'<a style="color:#F14400;line-height:24px;"  target=blank href=http://www.slgz.cn/html/about/2011/0316/23787.html title="购买毕业设计(论文)前请先访问我们的购买指南">信誉说明' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://www.slgz.cn/sanqiqq/images/qq/qq_01.gif alt="" align="right" width="16" height="16"/></td>' + 
'<td style=" background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0; padding-left:3px; background-position: 0%" height="24" align="left">' + 
'<a style="color:#F14400;line-height:24px;"  target=blank href=http://www.slgz.cn/html/about/2011/0316/23788.html title="咨询前请先访问我们的帮助中心-疑难解答">付款方式' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://wpa.qq.com/pa?p=1:81120664:4 alt="" align="right"/></td>' + 
'<td style="background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0; padding-left:3px; background-position: 0%" height="24" align="left">' + 
'<a style="color:#666666;" target=blank href=http://wpa.qq.com/msgrd?V=1&Uin=81120664&Site=www.slgz.cn&Menu=yes title="">售后服务' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://wpa.qq.com/pa?p=1:81120664:4 alt="" align="right" /></td>' + 
'<td style="background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0; padding-left:3px; background-position: 0%" height="24" align="left">' + 
'<a style="color:#666666;" target=blank href=http://wpa.qq.com/msgrd?V=1&Uin=81120664&Site=www.slgz.cn&Menu=yes title="">技术支持' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://www.slgz.cn/sanqiqq/images/qq/qq_02.gif alt="" align="right" width="16" height="16"/></td>' + 
'<td style=" background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0; padding-left:3px; background-position: 0%" height="24" align="left">' + 
'<a style="color:#666666;line-height:24px;"  target=blank href=http://www.slgz.cn/html/about/2011/0316/23789.html title="需要订做毕业设计(论文)前请先查看订做流程">购买成品' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://www.slgz.cn/sanqiqq/images/qq/qq_02.gif alt="" align="right" width="16" height="16"/></td>' + 
'<td style=" background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0; padding-left:3px; background-position: 0%" height="24" align="left">' + 
'<a style="color:#666666;line-height:24px;"  target=blank href=http://www.slgz.cn/html/about/2011/0316/23790.html title="客户至上 真诚服务 诚信为本">订制论文' + 
'</td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding-left:0px" width="23" height="24"><img border="0" SRC=http://www.slgz.cn/sanqiqq/images/qq/call.gif alt="" align="right" width="16" height="16"/></td>' + 
'<td style="color:#006600; font-size:9px;font-weight:bold; font-family:Arial; background-color: bgcolor; background-repeat: repeat; background-attachment: scroll; padding: 0; padding-left:3px; background-position: 0%" height="24" align="left">' + 
'13545062927' + 
'</td>' + 
'</tr>' + 
'<!--end-->' + 
'</table></td>' + 
'</tr>' + 
'</table></td>' + 
'</tr>' + 
'<tr>' + 
'<td style="padding:0;font-size:13px" height="1"><img src="http://www.slgz.cn/sanqiqq/images/qq/qq_bottom.gif" width="90" height="76"></td>' + 
'</tr>' + 
'</table>' + 
'</td>' + 
'</tr>' + 
'</table>' + 
'<map name="MapMapMap" onclick="far_close()" style="cursor:handle">' + 
'<area shape="rect" coords="55,8,102,30" >' + 
'</map>';
document.write(suspendcode12); 
document.write(recontent); 
document.write("</div>"); 
window.setInterval("heartBeat()",1);

function far_close()
{
	document.getElementById("lovexin12").innerHTML="";
}

function setfrme()
{
	var tr=document.getElementById("lovexin12");
	var twidth=tr.clientWidth;
	var theight=tr.clientHeight;
	var fr=document.getElementById("frame55la");
	fr.width=twidth-1;
	fr.height=theight-30;
}
//setfrme()


