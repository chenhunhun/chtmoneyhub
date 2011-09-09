/*
 *    	version 1.0 Tesy
 *    	include Tesy.calendar
 *	  	author Qbaty.qi
 *    	2009-12-05
 */
var Tesy = (typeof Tesy == "undefined") ? {} : Tesy;

Tesy.date = {
	//获取星期几
	getDayInWeek: function(year, month, day){
		var d = new Date(year, month, day);
		return d.getDay();
	},
	//获取当月天数
	getDaysNumBy: function(year, month){
		var d = new Date(year, (month + 1), 0);
		return d.getDate();
	},
	//获取显示的月数
	getRMonth: function(month){
		return parseInt(month) + 1;
	},
	getRMonthByDate: function(date){
		var d = date;
		return (d.getMonth() + 1);
	},
	//根据日期获取一年中有多少天
	//区分闰年
	getDaysofYear: function(y){
		if (!(y % 400)) {
			return 366;
		}
		else 
			if (!(y % 4) && y % 100) {
				return 366;
			}
			else {
				return 365;
			}
	},
	easyGetDaysofYear: function(y){
		var a = new Date(y, 1, 1);
		var b = new Date(y + 1, 1, 1);
		var c = b - a;
		return c / (3600000 * 24);
	},
	//根据日期获取一年的第几周,从0开始
	getWeekNumofDay: function(date){
		var y = date.getFullYear();
		var daynum = (date - new Date(y, 1, 1)) / (3600000 * 24);
		var firstDay = yearFirstDay.getDay();
		return (daynum + firstDay) / 7;
	},
	//根据年和周取，周从0开始
	getFirstDayByWeek: function(year, weeknum){
		var firstDay = new Date(year, 1, 1).getDay();
		return
	},
	stringToDate: function(sdate){
		var adate = sdate.split('-');
		var date = new Date(adate[0], (adate[1] - 1), adate[2]);
		return date;
	},
	dateToString: function(date){
		return (date.getFullYear() + '-' + date.getMonth() + '-' + date.getDate());
	},
	getMonthScale: function(date){
		var m = date.getMonth();
		var y = date.getFullYear();
		var ds = Tesy.date.getDaysNumBy(y, m);
		var fdw = Tesy.date.getDayInWeek(y, m, 1)
		var scale = (Math.floor((ds + fdw - 7) / 7) == 4 && (ds + fdw - 7) % 7 > 0) ? 6 : 5;
		return scale;
	}
};

Tesy.Calendar = function(container, date){
	this.container = typeof container == "string" ? document.getElementById(container) : container;
	this.date = date;
};

Tesy.Calendar.prototype = {
	DAY_ARR: ["周日", "周一", "周二", "周三", "周四", "周五", "周六"],
	makeCalendar: function(nyear, nmonth){
		var dom = Tesy.dom;
		var Cal = this;
		var show, daySpan, singleDay, date_string, tempMonth, tempCell, tempTr, tempTable, tempDiv, oneDay;
		var calendarHead, calendarBody, wrapper, isThisMonth, tdate, currentDayCell, tdayInMonth, start, days, weeks, dh, eventWrap, lunar;
		
		Cal.show = show = Tesy.dom.$('yearmonth');
		if (typeof nyear == "undefined" || typeof nmonth == "undefined") {
			var tempd = new Date();
			var year = tempd.getFullYear();
			var month = tempd.getMonth();
		}
		else {
			var tempd = new Date(nyear, nmonth);
			var year = tempd.getFullYear();
			var month = tempd.getMonth();
		}
		
		Cal.thisMonth = month;
		Cal.thisYear = year;
		show.innerHTML = year + '年' + (month + 1) + '月';
		
		tdate = Cal.tdate = new Date();
		tdayInMonth = tdate.getDate();
		wrapper = Cal.wrapper = Cal.wrapper || Tesy.dom.cE("div");
		isThisMonth = (year == (tdate.getFullYear()) && month == (tdate.getMonth())) ? true : false;
		days = Tesy.date.getDaysNumBy(year, month);
		
		start = 1 - Tesy.date.getDayInWeek(year, month, 1);
		weeks = Tesy.date.getMonthScale(tempd);
		//weeks = 6;
		dh = (1 / weeks) * 100;
		
		function makeCalendarHead(){
			if (typeof Cal.calendarHead !== 'undefined') {
				return;
			}
			else {
				calendarHead = Cal.calendarHead = Tesy.dom.cE("table");
				calendarHead.setAttribute('cellspacing', '0');
				calendarHead.setAttribute('cellpadding', '0');
				calendarHead.className = "c_head";
				calendarHead.id = "c_head";
				calendarHead.appendChild(document.createElement('tbody'));
			}
			tempTr = calendarHead.tBodies[0].insertRow(0);
			for (var i = 0; i < 7; ++i) {
				tempCell = tempTr.insertCell(i);
				tempCell.appendChild(document.createTextNode(Cal.DAY_ARR[i].slice(-2)));
			}
			wrapper.appendChild(calendarHead);
		}
		makeCalendarHead();
		
		if (typeof Cal.calendarBody !== 'undefined') {
			var calendarBody = Cal.calendarBody;
			calendarBody.innerHTML = "";
		}
		else {
			calendarBody = Cal.calendarBody = Tesy.dom.cE("div");
			calendarBody.className = "c_body";
			calendarBody.id = "c_body";
			calendarBody.style.height = '200px';
			wrapper.appendChild(calendarBody);
			Cal.container.appendChild(wrapper);
		}
		
		Cal.dList = [];
		Cal.evtWrapList = [];
		for (var i = 0; i < weeks; ++i) {
			tempDiv = Tesy.dom.cE('div');
			tempTable = Tesy.dom.cE('table');
			tempTr = tempTable.insertRow(0);
			for (var j = 0; j < 7; ++j) {
				singleDay = new Date(year, month, start);
				oneDay = tempTr.insertCell(j).appendChild(Tesy.dom.cE("div"));
				daySpan = Tesy.dom.cE("span");
				lunar = Tesy.dom.cE("span");
				topinfo = Tesy.dom.cE('div');
				eventWrap = Tesy.dom.cE('div');
				
				daySpan.innerHTML = singleDay.getDate();
				daySpan.className = "dspan";
				
				lunar.innerHTML = calc(singleDay);
				lunar.className = "lunar";
				
				topinfo.appendChild(daySpan);
				topinfo.appendChild(lunar);
				topinfo.className = "clear";
				
				eventWrap.className = 'eventWrap';
				
				if (isThisMonth && start == tdayInMonth) {
					oneDay.className = "day currentDay";
					currentDayCell = oneDay;
					Cal.evtWrapList.push(eventWrap);
					Cal.dList.push(oneDay);
				}
				else 
					if (singleDay.getMonth() == month) {
						oneDay.className = "day";
						Cal.evtWrapList.push(eventWrap);
						Cal.dList.push(oneDay);
					}
					else {
						oneDay.className = "fday";
					}
				oneDay.setAttribute("date", (year + "-" + (singleDay.getMonth() + 1) + "-" + singleDay.getDate()));
				oneDay.appendChild(topinfo);
				oneDay.appendChild(eventWrap);
				start++;
			}
			tempTable.className = "row_table";
			tempTable.setAttribute('cellspacing', '0');
			tempTable.setAttribute('cellpadding', '0');
			tempDiv.appendChild(tempTable);
			tempDiv.style.cssText = 'height:' + dh + '%;top:' + dh * (i) + '%;left:0;position:absolute;';
			calendarBody.appendChild(tempDiv);
		}
	},
	
	nextMonth: function(){
		var thisMonth = parseInt(this.thisMonth);
		var thisyear = this.thisYear;
		this.makeCalendar(thisyear, thisMonth + 1);
		Tesy.util.enableEvent();
		Tesy.uEvents.init();
		return false;
	},
	
	preMonth: function(){
		var thisMonth = parseInt(this.thisMonth);
		var thisyear = this.thisYear;
		this.makeCalendar(thisyear, thisMonth - 1);
		Tesy.util.enableEvent();
		Tesy.uEvents.init();
		return false;
	}
};


Tesy.miniCalendar = function(container, date){
	this.container = typeof container == "string" ? document.getElementById(container) : container;
	this.date = date;
}

Tesy.miniCalendar.prototype = {
	makeCalendar: function(year, month, day){
		var Cal = this;
		var tempCell, tempMonth, daySpan, mdate, singleDay, date_string, i, show, dayShow;
		
		Cal.monthshow = Tesy.dom.$('select_date');
		Cal.dayShow = Tesy.dom.$('dateselect');
		
		if (arguments.length == 0) {
			Cal.tdate = Cal.mdate = new Date();
		}
		else 
			if (arguments.length == 2) {
				Cal.mdate = new Date(year, month);
				if (typeof Cal.tdate == 'undefined') {
					Cal.tdate = new Date();
				}
			}
			else 
				if (arguments.length == 3) {
					Cal.tdate = new Date(year, month, day);
					if (typeof Cal.mdate == 'undefined') {
						Cal.mdate = new Date();
					}
					else {
						Cal.mdate = new Date(year, month);
					}
				}
		
		Cal.year = Cal.mdate.getFullYear();
		Cal.month = Cal.mdate.getMonth();
		Cal.tdayInMonth = Cal.tdate.getDate();
		Cal.monthshow.innerHTML = Cal.year + '年' + (Cal.month + 1) + '月';
		Cal.wrapper = Cal.wrapper || Tesy.dom.cE("div");
		Cal.isThisMonth = (Cal.year == (Cal.tdate.getFullYear()) && Cal.month == (Cal.tdate.getMonth())) ? true : false;
		Cal.dayNum = Tesy.date.getDaysNumBy(Cal.year, Cal.month);
		Cal.start = 1 - Tesy.date.getDayInWeek(Cal.year, Cal.month, 1);
		Cal.weeks = Tesy.date.getMonthScale(Cal.mdate);
		Cal.end = Cal.start + (Cal.weeks * 7);
		
		if (typeof Cal.calendarBody !== "undefined") {
			Cal.calendarBody.innerHTML = "";
		}
		else {
			Cal.calendarBody = Tesy.dom.cE("ul");
			Cal.calendarBody.className = "minibody";
		}
		
		for (var i = Cal.start, l = Cal.end; i < l; ++i) {
			singleDay = new Date(Cal.year, Cal.month, i);
			tempCell = Tesy.dom.cE("li");
			tempCell.innerHTML = singleDay.getDate();
			tempMonth = singleDay.getMonth();
			
			if (tempMonth == Cal.month) {
				if (Math.floor((new Date() - singleDay) / 86400000) > 0) {
					tempCell.className = "pass";
				}
				else {
					tempCell.className = "day";
					tempCell.onclick = (function(date){
						return function(){
							var outerwrap = Tesy.dom.$('minicalendar');
							
							Cal.dayShow.innerHTML = date.getFullYear() + "年" + (date.getMonth() + 1) + "月" + date.getDate() + "日";
							Cal.dayShow.setAttribute('date', date.getFullYear() + '-' + (date.getMonth() + 1) + '-' + date.getDate());
							
							if (Cal.currentDayCell) {
								Cal.currentDayCell.removeAttribute("today");
								Tesy.dom.removeClass(Cal.currentDayCell, 'currentDay');
							}
							Tesy.dom.addClass(this, 'currentDay');
							Cal.currentDayCell = this;
							
							Tesy.eventlay.caculateDay();
							outerwrap.setAttribute('show', 'off');
							outerwrap.style.display = 'none';
						}
					})(singleDay);
				}
			}
			else {
				tempCell.className = "fday";
			}
			
			if (Cal.isThisMonth && i == Cal.tdayInMonth) {
				Tesy.dom.addClass(tempCell, "currentDay");
			}
			
			Cal.calendarBody.appendChild(tempCell);
		}
		Cal.wrapper.appendChild(Cal.calendarBody);
		Cal.container.appendChild(Cal.wrapper);
	},
	
	nextMonth: function(){
		var thisMonth = parseInt(this.month);
		var thisyear = this.year;
		this.makeCalendar(thisyear, thisMonth + 1);
		return false;
	},
	
	preMonth: function(){
		var thisMonth = parseInt(this.month);
		var thisyear = this.year;
		this.makeCalendar(thisyear, thisMonth - 1);
		return false;
	}
}
