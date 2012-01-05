function calc(odate) {
   sDObj = odate
   sY = sDObj.getFullYear();
   sM = sDObj.getMonth();
   sD = sDObj.getDate();

   sMObj = new Date(sY, sM, 1, 0, 0, 0, 0);
   firstWeek = sMObj.getDay();    //公历当月1日星期几
   slength    = solarDays(sY, sM);    //公历当月天数

   lDObj = new Lunar(sDObj);     //农历
   lY    = lDObj.year;           //农历年
   lM    = lDObj.month;          //农历月
   lD    = lDObj.day;            //农历日
   lL    = lDObj.isLeap;         //农历是否闰月

   if (lD == 1) //显示农历月
      lunardisplay = (lL?'闰':'') + textMonth[lM-1] + '月';
   else //显示农历日
      lunardisplay = cDay(lD);

   //节气
   if (sD == sTerm(sY, sM*2)) lunardisplay = solarTerm[sM*2];
   if (sD == sTerm(sY, sM*2+1)) lunardisplay = solarTerm[sM*2+1];

   //公历节日
   for(i in sFtv)
      if(sFtv[i].match(/^(\d{2})(\d{2})([\s\*])(.+)$/))
         if(Number(RegExp.$1) == (sM+1)) {
            if (sD == Number(RegExp.$2)) lunardisplay = RegExp.$4 + ' ';
         }
   //月周节日
   for(i in wFtv)
      if(wFtv[i].match(/^(\d{2})(\d)(\d)([\s\*])(.+)$/))
         if(Number(RegExp.$1)==(sM+1)) {
            tmp1=Number(RegExp.$2);
            tmp2=Number(RegExp.$3);
            if(tmp1 < 5)
               if (sD == ((firstWeek>tmp2)?7:0) + 7*(tmp1-1) + tmp2 - firstWeek + 1) lunardisplay = RegExp.$5 + ' ';
            else {
               tmp1 -= 5;
               tmp3 = (firstWeek + slength - 1) % 7; //当月最后一天星期?
               if (sD == length - tmp3 - 7*tmp1 + tmp2 - (tmp2>tmp3?7:0)) lunardisplay = RegExp.$5 + ' ';
            }
         }

   //农历节日
   for(i in lFtv)
      if(lFtv[i].match(/^(\d{2})(.{2})([\s\*])(.+)$/))
         if(Number(RegExp.$1) == (lM)) {
            if (lD == Number(RegExp.$2)) lunardisplay = RegExp.$4 + ' ';
         }
   
   return lunardisplay;
}

//====================================== 算出农历, 传入日期控件, 返回农历日期控件
//                                       该控件属性有 .year .month .day .isLeap
function Lunar(objDate) {
   var i, leap=0, temp=0;
   var offset = (Date.UTC(objDate.getFullYear(),objDate.getMonth(),objDate.getDate()) - Date.UTC(1900,0,31))/86400000;
   
   for (i=1900; i<2100 && offset>0; i++) {
      temp = lYearDays(i);
      offset -= temp;
   }
   
   if (offset < 0) {
      offset+=temp;
      i--;
   }
   
   this.year = i;
   
   leap = leapMonth(i); //闰哪个月
   this.isLeap = false;
   
   for (i=1; i<13 && offset>0; i++) {
      //闰月
      if(leap>0 && i==(leap+1) && this.isLeap==false) {
         --i;
         this.isLeap = true;
         temp = leapDays(this.year);
      } else {
         temp = monthDays(this.year, i);
      }
      
      //解除闰月
      if(this.isLeap==true && i==(leap+1))
         this.isLeap = false;
      
      offset -= temp;
   }
   
   if(offset==0 && leap>0 && i==leap+1)
      if(this.isLeap)   {
         this.isLeap = false;
      } else {
         this.isLeap = true;
         --i;
      }
   
   if(offset<0) {
      offset += temp;
      --i;
   }
   
   this.month = i;
   this.day = offset + 1;
}

//==============================返回公历 y年某m+1月的天数
function solarDays(y,m) {
   if(m==1)
      return(((y%4 == 0) && (y%100 != 0) || (y%400 == 0))? 29: 28);
   else
      return(solarMonth[m]);
}

//====================================== 返回农历 y年的总天数
function lYearDays(y) {
   var i, sum = 348;
   for(i=0x8000; i>0x8; i>>=1) sum += (lunarInfo[y-1900] & i)? 1: 0;
   return(sum+leapDays(y));
}

//====================================== 返回农历 y年闰月的天数
function leapDays(y) {
   if(leapMonth(y)) return( (lunarInfo[y-1899]&0xf)==0xf? 30: 29);
   else return(0);
}

//====================================== 返回农历 y年闰哪个月 1-12 , 没闰返回 0
function leapMonth(y) {
   var lm = lunarInfo[y-1900] & 0xf;
   return(lm==0xf?0:lm);
}

//====================================== 返回农历 y年m月的总天数
function monthDays(y,m) {
   return( (lunarInfo[y-1900] & (0x10000>>m))? 30: 29 );
}

//====================== 中文日期
function cDay(d){
   var s;
   
   switch (d) {
      case 10:
         s = '初十'; break;
      case 20:
         s = '二十'; break;
         break;
      case 30:
         s = '三十'; break;
         break;
      default :
         s = nStr2[Math.floor(d/10)];
         s += nStr1[d%10];
   }
   return(s);
}

//===== 某年的第n个节气为几日(从0小寒起算)
function sTerm(y,n) {
   var offDate = new Date( ( 31556925974.7*(y-1900) + sTermInfo[n]*60000  ) + Date.UTC(1900,0,6,2,5) );
   return(offDate.getUTCDate());
}

var textMonth = new Array("正", "二", "三", "四", "五", "六", "七", "八", "九", "十", "十一", "十二");

/*****************************************************************************
                                   日期资料
*****************************************************************************/
var lunarInfo = new Array(
0x4bd8,0x4ae0,0xa570,0x54d5,0xd260,0xd950,0x5554,0x56af,0x9ad0,0x55d2,
0x4ae0,0xa5b6,0xa4d0,0xd250,0xd295,0xb54f,0xd6a0,0xada2,0x95b0,0x4977,
0x497f,0xa4b0,0xb4b5,0x6a50,0x6d40,0xab54,0x2b6f,0x9570,0x52f2,0x4970,
0x6566,0xd4a0,0xea50,0x6a95,0x5adf,0x2b60,0x86e3,0x92ef,0xc8d7,0xc95f,
0xd4a0,0xd8a6,0xb55f,0x56a0,0xa5b4,0x25df,0x92d0,0xd2b2,0xa950,0xb557,
0x6ca0,0xb550,0x5355,0x4daf,0xa5b0,0x4573,0x52bf,0xa9a8,0xe950,0x6aa0,
0xaea6,0xab50,0x4b60,0xaae4,0xa570,0x5260,0xf263,0xd950,0x5b57,0x56a0,
0x96d0,0x4dd5,0x4ad0,0xa4d0,0xd4d4,0xd250,0xd558,0xb540,0xb6a0,0x95a6,
0x95bf,0x49b0,0xa974,0xa4b0,0xb27a,0x6a50,0x6d40,0xaf46,0xab60,0x9570,
0x4af5,0x4970,0x64b0,0x74a3,0xea50,0x6b58,0x5ac0,0xab60,0x96d5,0x92e0,
0xc960,0xd954,0xd4a0,0xda50,0x7552,0x56a0,0xabb7,0x25d0,0x92d0,0xcab5,
0xa950,0xb4a0,0xbaa4,0xad50,0x55d9,0x4ba0,0xa5b0,0x5176,0x52bf,0xa930,
0x7954,0x6aa0,0xad50,0x5b52,0x4b60,0xa6e6,0xa4e0,0xd260,0xea65,0xd530,
0x5aa0,0x76a3,0x96d0,0x4afb,0x4ad0,0xa4d0,0xd0b6,0xd25f,0xd520,0xdd45,
0xb5a0,0x56d0,0x55b2,0x49b0,0xa577,0xa4b0,0xaa50,0xb255,0x6d2f,0xada0,
0x4b63,0x937f,0x49f8,0x4970,0x64b0,0x68a6,0xea5f,0x6b20,0xa6c4,0xaaef,
0x92e0,0xd2e3,0xc960,0xd557,0xd4a0,0xda50,0x5d55,0x56a0,0xa6d0,0x55d4,
0x52d0,0xa9b8,0xa950,0xb4a0,0xb6a6,0xad50,0x55a0,0xaba4,0xa5b0,0x52b0,
0xb273,0x6930,0x7337,0x6aa0,0xad50,0x4b55,0x4b6f,0xa570,0x54e4,0xd260,
0xe968,0xd520,0xdaa0,0x6aa6,0x56df,0x4ae0,0xa9d4,0xa4d0,0xd150,0xf252,
0xd520);

var solarMonth=new Array(31,28,31,30,31,30,31,31,30,31,30,31);
var solarTerm = new Array("小寒","大寒","立春","雨水","惊蛰","春分","清明","谷雨","立夏","小满","芒种","夏至","小暑","大暑","立秋","处暑","白露","秋分","寒露","霜降","立冬","小雪","大雪","冬至");
var sTermInfo = new Array(0,21208,42467,63836,85337,107014,128867,150921,173149,195551,218072,240693,263343,285989,308563,331033,353350,375494,397447,419210,440795,462224,483532,504758);

var nStr1 = new Array('日','一','二','三','四','五','六','七','八','九','十');
var nStr2 = new Array('初','十','廿','卅','□');

//公历节日 *表示放假日
var sFtv = new Array(
"0101*元旦",
//"0101*新年元旦",
//"0202 世界湿地日",
//"0207 国际声援南非日",
//"0210 国际气象节",
"0214 情人节",
//"0301 国际海豹日",
//"0303 全国爱耳日",
"0308 妇女节",
//"0308 国际妇女节",
//"0312 孙中山逝世纪念日",
//"0314 国际警察日",
//"0315 国际消费者权益日",
//"0317 中国国医节",
//"0321 世界森林日",
//"0321 世界儿歌日",
//"0322 世界水日",
//"0323 世界气象日",
//"0324 世界防治结核病日",
//"0330 巴勒斯坦国土日",
"0401 愚人节",
//"0407 世界卫生日",
//"0422 世界地球日",
//"0423 世界图书和版权日",
//"0424 亚非新闻工作者日",
"0501 劳动节",
//"0501 国际劳动节",
"0504 青年节",
//"0504 中国五四青年节",
//"0505 碘缺乏病防治日",
//"0508 世界红十字日",
//"0512 国际护士节",
//"0515 国际家庭日",
//"0517 世界电信日",
//"0518 国际博物馆日",
//"0520 全国学生营养日",
//"0523 国际牛奶日",
//"0531 世界无烟日", 
"0601 儿童节",
//"0601 国际儿童节",
//"0605 世界环境日",
//"0606 全国爱眼日",
//"0617 防治荒漠化和干旱日",
//"0623 国际奥林匹克日",
//"0625 全国土地日",
//"0626 国际反毒品日",
"0701 建党节",
//"0701 中国共产党建党日",
//"0702 国际体育记者日",
//"0707 抗日战争纪念日",
//"0711 世界人口日",
//"0730 非洲妇女日",
"0801 建军节",
//"0815 日本无条件投降日",
//"0908 国际扫盲日",
"0910 教师节",
//"0914 世界清洁地球日",
//"0916 国际臭氧层保护日",
//"0918 九·一八事变纪念日",
//"0920 国际爱牙日",
//"0927 世界旅游日",
"1001*国庆节",
//"1001 国际音乐日",
//"1004 世界动物日",
//"1008 全国高血压日",
//"1008 世界视觉日",
//"1009 世界邮政日",
//"1010 辛亥革命纪念日",
//"1013 世界保健日",
//"1014 世界标准日",
//"1015 国际盲人节",
//"1016 世界粮食日",
//"1017 世界消除贫困日",
//"1022 世界传统医药日",
//"1024 联合国日",
//"1031 世界勤俭日",
//"1107 十月革命纪念日",
//"1108 记者日",
//"1110 世界青年节",
//"1112 孙中山诞辰纪念日",
//"1114 世界糖尿病日",
//"1117 国际大学生节",
//"1121 世界问候日",
//"1201 世界艾滋病日",
//"1203 世界残疾人日",
//"1208 国际儿童电视日",
//"1209 世界足球日",
//"1210 世界人权日",
//"1212 西安事变纪念日",
//"1213 南京大屠杀纪念日",
//"1221 国际篮球日",
//"1224 平安夜",
"1225 圣诞节");
//"1229 国际生物多样性日");

//某月的第几个星期几。 5,6,7,8 表示到数第 1,2,3,4 个星期几
var wFtv = new Array(
//"0110 黑人日",
//"0150 世界麻风日", //一月的最后一个星期日（月倒数第一个星期日）
"0520 母亲节",
//"0520 国际母亲节",
//"0530 全国助残日",
"0630 父亲节");
//"0911 劳动节",
//"0932 国际和平日",
//"0940 世界儿童日",
//"0950 世界海事日",
//"1011 国际住房日",
//"1013 国际减轻自然灾害日",
//"1144 感恩节");

//农历节日
var lFtv = new Array(
"0101*春节",
"0115 元宵节",
//"0202 龙抬头节",
//"0323 妈祖生辰 (天上圣母诞辰)",
"0505 端午节",
//"0707 七七中国情人节",
"0815 中秋节",
"0909 重阳节",
//"1208 腊八节",
//"1223 祭灶节",
"0100*除夕");