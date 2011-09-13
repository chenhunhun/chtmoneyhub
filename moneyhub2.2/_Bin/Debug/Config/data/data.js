apptypes = ["all","banks","securities","insurances","funds","payments","shopping"]
appList = {
	banks:{
		a003 : {
		"id":"a003",
		"name":"中国工商银行",
		"url":"http://www.icbc.com.cn/",
		"imageUrl":"../../Config/logos/A007.gif",
		"favUrl":"../../Config/logos/A007_s.gif",
		"index":"gh,gsyh,zhongguogongshangyinghang,icbc,icbc,Industrial and commercial bank of china,中国工商银行,工行",
		"collected":"false",
		"apptype":"banks",
		"position":"2",
		"support64":"no",
		"sublink" : {
		 	 "url" : "http://www.icbc.com.cn/",
			 "txt_ebank":"个人网银",
			 "ebank":"http://www.icbc.com.cn/icbc/%e7%bd%91%e9%93%b6%e7%b3%bb%e7%bb%9f/alert.htm",
			 "txt_vip":"贵宾版",
			 "vip":"https://vip.icbc.com.cn/icbc/perbank/index.jsp"
		 }
		},
		a002 : {
		"id":"a002",
		 "name":"中国农业银行",
		 "url":"http://www.abchina.com/",
		 "imageUrl":"../../Config/logos/A004.gif",
		 "favUrl":"../../Config/logos/A004_s.gif",
		 "index":"nh,nyyh,zhongguonongyeyinghang,abc,agricultual bank of china,中国农业银行,农行",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"1",
		 "sublink" : {
		 	 "url" : "http://www.abchina.com/",
			 "txt_ebank":"个人网银",
			 "ebank":"http://www.abchina.com/cn/EBanking/Ebanklogin/PCustomerLogin/default.htm"
		 }
		},		
		a004 : {
		"id":"a004",
		 "name":"中国建设银行",
		 "url":"http://www.ccb.com/",
		 "imageUrl":"../../Config/logos/A006.gif",
		 "favUrl":"../../Config/logos/A006_s.gif",
		 "index":"jh,jsyh,zhongguojiansheyinhang,CCB,ccb,China Construction Bank,中国建设银行,建行",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"3",
		 "sublink" : {
		 	 "url" : "http://www.ccb.com/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://ibsbjstar.ccb.com.cn/app/V5/CN/STY1/login.jsp"
		 }
		},
		a001 : {
		"id":"a001",
		 "name":"中国银行",
		 "url":"http://www.boc.cn/",
		 "imageUrl":"../../Config/logos/A001.gif",
		 "favUrl":"../../Config/logos/A001_s.gif",
		 "index":"zgyh,zhongguoyinhang,BOC,boc,Bank of China,中国银行,中行",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"0",
		 "sublink" : {
		 	 "url" : "http://www.boc.cn/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://ebs.boc.cn/BocnetClient/LoginFrame.do?_locale=zh_CN"
		 }
		},
		a005 : {
		"id":"a005",
		 "name":"交通银行",
		 "url":"http://www.bankcomm.com",
		 "imageUrl":"../../Config/logos/A013.gif",
		 "favUrl":"../../Config/logos/A013_s.gif",
		 "index":"jh,jtyh,jiaotongyinhang,BOCOMM,bocomm,Bank of Communication,交通银行,交行",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"4",
		 "sublink" : {
		 	 "url" : "http://www.bankcomm.com/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://pbank.95559.com.cn/personbank/index.jsp"
		 }
		},
		a006 : {
		"id":"a006",
		 "name":"招商银行",
		 "url":"http://www.cmbchina.com/",
		 "imageUrl":"../../Config/logos/A009.gif",
		 "favUrl":"../../Config/logos/A009_s.gif",
		 "index":"zh,zsyh,zhaoshangyinhang,CMB,cmb,China Merchants Bank,招商银行,招行",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"5",
		 "sublink" : {
		 	 "url" : "http://www.cmbchina.com/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://pbsz.ebank.cmbchina.com/CmbBank_GenShell/UI/GenShellPC/Login/Login.aspx"
		 }
		},
		a007 : {
		"id":"a007",
		 "name":"中信银行",
		 "url":"http://bank.ecitic.com/",
		 "imageUrl":"../../Config/logos/A002.gif",
		 "favUrl":"../../Config/logos/A002_s.gif",
		 "index":"zxyh,zhongxinyinhang,CNCB,cncb,China Citic Bank,中信银行,中信",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"6",
		 "sublink" : {
		 	 "url" : "http://bank.ecitic.com/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://e.bank.ecitic.com/perbank5/signIn.do"
		 }
		},
		a008 : {
		"id":"a008",
		 "name":"光大银行",
		 "url":"http://www.cebbank.com/",
		 "imageUrl":"../../Config/logos/A016.gif",
		 "favUrl":"../../Config/logos/A016_s.gif",
		 "index":"gdyh,guangdayinhang,CEB,ceb,China Everbright Bank,光大银行,光大",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"7",
		 "sublink" : {
		 	 "url" : "http://www.cebbank.com/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://www.cebbank.com/per/prePerlogin1.do?_locale=zh_CN",
			 "txt_vip":"专业版",
			 "vip":"http://ebank.cebbank.com/preLogin.html"
		 }
		},
		a009 : {
		"id":"a009",
		 "name":"浦发银行",
		 "url":"http://www.spdb.com.cn/",
		 "imageUrl":"../../Config/logos/A012.gif",
		 "favUrl":"../../Config/logos/A012_s.gif",
		 "index":"pfyh,pufayinhang,SPDB,spdb,SPD Bank,浦发银行,浦发",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"8",
		 "sublink" : {
		 	 "url" : "http://www.spdb.com.cn/",
			 "txt_ebank":"个人网银",
			 "ebank":"http://ebank.spdb.com.cn/login/perlogin.html"
		 }
		},
/*
		a010 : {
		"id":"a010",
		 "name":"深圳发展银行",
		 "url":"http://www.sdb.com.cn/",
		 "imageUrl":"../../Config/logos/A011.gif",
		 "favUrl":"../../Config/logos/A011_s.gif",
		 "index":"szfzyh,shenzhenfazhanyinhang,SDB,Shenzhen Development Bank,深圳发展银行,深发",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"9",
		 "sublink" : {
		 	 "url" : "http://www.sdb.com.cn/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://ebank.sdb.com.cn/perbank/logon_pro.jsp"
		 }
		},
	
		a011 : {
		"id":"a011",
		 "name":"广东发展银行",
		 "url":"http://www.gdb.com.cn/",
		 "imageUrl":"../../Config/logos/A015.gif",
		 "favUrl":"../../Config/logos/A015_s.gif",
		 "index":"gdfzyh,guangdongfazhanyinhang,GDB,Guangdong Development Bank,广东发展银行,广发",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"10",
		 "sublink" : {
		 	 "url" : "http://www.gdb.com.cn/",
			 "ebank":"https://ebanks.gdb.com.cn/sperbank/perbankLogin.jsp"
		 }
		},
		*/
		a012 : {
		"id":"a012",
		 "name":"华夏银行",
		 "url":"http://www.hxb.com.cn/",
		 "imageUrl":"../../Config/logos/A014.gif",
		 "favUrl":"../../Config/logos/A014_s.gif",
		 "index":"hxyh,huaxiayinhang,HXB,Huaxia Bank,华夏银行,华夏",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"11",
		 "sublink" : {
		 	 "url" : "http://www.hxb.com.cn/",
		/*   "txt_ebank":"个人网银普通版",
			 "ebank":"https://ebank.hxb.com.cn/HxPer/basPreLogin.do",
			 "txt_vip":"个人网银签约版",
			 "vip":"https://ebank.hxb.com.cn/HxPer/regLogin01.do",
		*/
			 "txt_credit":"个人网银证书版",
			 "credit":"https://dbank.hxb.com.cn/easybanking/jsp/indexCert.jsp"
		 }
		},
		a013 : {
		"id":"a013",
		 "name":"民生银行",
		 "url":"http://www.cmbc.com.cn/",
		 "imageUrl":"../../Config/logos/A005.gif",
		 "favUrl":"../../Config/logos/A005_s.gif",
		 "index":"msyh,minshengyinhang,CMBC,China Minsheng Bank Corp,民生银行,民生",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"12",
		 "sublink" : {
		 	 "url" : "http://www.cmbc.com.cn/",
			 "txt_ebank":"个人大众版",
			 "ebank":"https://ebank.cmbc.com.cn/index_NonPrivate.html",
			 "txt_vip":"贵宾版",
			 "vip":"https://business.cmbc.com.cn/index_Private_pri.html",
			 "txt_credit":"信用卡",
			 "credit":"https://ebank.cmbc.com.cn/indexCCLogin.html"
		 }
		},
		a014 : {
		"id":"a014",
		 "name":"兴业银行",
		 "url":"http://www.cib.com.cn/",
		 "imageUrl":"../../Config/logos/A010.gif",
		 "favUrl":"../../Config/logos/A010_s.gif",
		 "index":"xyyh,xingyeyinhang,CIB,Industrail Bank,兴业银行,兴业",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"13",
		 "sublink" : {
		 	 "url" : "http://www.cib.com.cn/",
			 "txt_ebank":"个人普通用户",
			 "ebank":"https://www.cib.com.cn/index.jsp",
			 "txt_vip":"个人证书用户",
			 "vip":"http://www.cib.com.cn/netbank/cn/persbankalert.html"
		 }
		},
		a015 : {
		"id":"a015",
		 "name":"北京银行",
		 "url":"http://www.bankofbeijing.com.cn/",
		 "imageUrl":"../../Config/logos/A018.gif",
		 "favUrl":"../../Config/logos/A018_s.gif",
		 "index":"bjyh,beijingyinhang,Bank of Beijing,北京银行,北京",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"14",
		 "sublink" : {
		 	 "url" : "http://www.bankofbeijing.com.cn/",
			 "txt_ebank":"个人普通用户",
			 "ebank":"https://ebank.bankofbeijing.com.cn/bccbpb/accountLogon.jsp",
			 "txt_vip":"个人财富用户",
			 "vip":"https://ebank.bankofbeijing.com.cn/bccbpb/customerLogon.jsp?language=zh_CN"
		 }
		},
		a016 : {
		"id":"a016",
		 "name":"北京农商银行",
		 "url":"http://www.bjrcb.com/",
		 "imageUrl":"../../Config/logos/A019.gif",
		 "favUrl":"../../Config/logos/A019_s.gif",
		 "index":"bjncsyyh,beijingnongcunshangyeyinhang,bjrcb,Beijing Rural Commercial Bank,北京农村商业银行,农商",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"15",
		 "sublink" : {
		 	 "url" : "http://www.bjrcb.com/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://ibs.bjrcb.com/per/prelogin.do",
			 "txt_credit":"支付密码系统",
			 "credit":"https://ebank.bjrcb.com/ent/preloginCheque.do"
			}
		},
		a017 : {
		"id":"a017",
		 "name":"中国邮政储蓄银行",
		 "url":"http://www.psbc.com/",
		 "imageUrl":"../../Config/logos/A003.gif",
		 "favUrl":"../../Config/logos/A003_s.gif",
		 "index":"zgyzcxyh,zhongguoyouzhengchuxuyinhang,Postal Savings Bank of China,PSBC,中国邮政储蓄银行,邮政",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"16",
		 "sublink" : {
		 	 "url" : "http://www.psbc.com/",
			 "txt_ebank":"个人网银",
			 "ebank":"https://pbank.psbc.com/pweb/prelogin.do?_locale=zh_CN&BankId=9999"
		 }
		},
		a018 : {
		"id":"a018",
		 "name":"平安银行",
		 "url":"http://bank.pingan.com/",
		 "imageUrl":"../../Config/logos/A020.gif",
		 "favUrl":"../../Config/logos/A020_s.gif",
		 "index":"payh,pinganyinhang,Pingan Bank,Pingan,平安银行,平安",
		 "collected":"false",
		 "apptype":"banks",
		 "position":"17",
		 "sublink" : {
		 	 "url" : "http://bank.pingan.com/",
			 "txt_ebank":"平安一账通",
			 "ebank":"https://www.pingan.com.cn/pinganone/pa/ebanklogin.screen"
		 }
		}
	},
	securities:{
		b001 : {
		"id":"b001",
		 "name":"中信证券",
		 "url":"http://www.cs.ecitic.com/",
		 "imageUrl":"../../Config/logos/securities/B007.png",
		 "imageUrl":"../../Config/logos/B001.gif",
		 "favUrl":"../../Config/logos/B001_s.gif",
		 "index":"zxzq,zhongxinzhengquan,CITIC Securities,中信证券,中信",
		 "collected":"false",
		 "apptype":"securities",
		 "position":"0",
		 "sublink" : {
		 	 "url" : "http://www.cs.ecitic.com/"
		 }
		}
	},
	insurances:{
		c001 : {
		"id":"c001",
		 "name":"中国人寿保险",
		 "url":"http://www.chinalife.com.cn/",
		 "imageUrl":"../../Config/logos/C001.gif",
		 "favUrl":"../../Config/logos/c001_s.gif",
		 "index":"rsbx,zgrs,China Life Insurance(Group) Company,zhongguo renshou,中国人寿,人保",
		 "collected":"false",
		 "apptype":"insurances",
		 "position":"0",
		 "sublink" : {
		 	 "url" : "http://www.chinalife.com.cn/"
		 }
		}
	},
	funds:{
		"d001" :{
		"id":"d001",
		 "name":"华夏基金",
		 "url":"http://www.chinaamc.com/",
		 "imageUrl":"../../Config/logos/E001.gif",
		 "favUrl":"../../Config/logos/E001_s.gif",
		 "index":"hxjj,huaxiajijin,华夏基金",
		 "collected":"false",
		 "apptype":"funds",
		 "position":"0",
		 "sublink" : {
		 	 "url" : "http://www.chinaamc.com/",
			 "txt_ebank":"交易登录",
			 "ebank":"https://fundtrade.chinaamc.com/etrading/etrading.jsp",
			 "txt_vip":"查询登录",
			 "vip":"https://www.chinaamc.com/portal/cn/register/loginmap.jsp"
		 }
		},
		"d002" :{
		"id":"d002",
		 "name":"招商基金",
		 "url":"http://www.cmfchina.com/index.html",
		 "imageUrl":"../../Config/logos/E002.gif",
		 "favUrl":"../../Config/logos/E002_s.gif",
		 "index":"zsjj,zhaoshangjijin,招商基金",
		 "collected":"false",
		 "apptype":"funds",
		 "position":"1",
		 "sublink" : {
		 	 "url" : "http://www.cmfchina.com/index.html",
		 	 "txt_ebank":"基金网上交易",
			 "ebank":"https://direct.cmfchina.com/index.jsp",
			 "txt_vip":"登录账户查询",
			 "vip":"https://service.cmfchina.com/user/login.jsp"
		 }
		}
	},
	payments:{
		e001 : {
		"id":"e001",
		 "name":"支付宝",
		 "url":"http://www.alipay.com/",
		 "imageUrl":"../../Config/logos/D001.gif",
		 "favUrl":"../../Config/logos/D001_s.gif",
		 "index":"zfb,Alipay,360buy,zhifubao,支付宝",
		 "collected":"false",
		 "apptype":"payments",
		 "position":"0",
		 "sublink" : {
		 	 "url" : "http://www.alipay.com/",
		 	 "txt_ebank":"安全保障",
			 "ebank":"http://home.alipay.com/security/product.htm"
		 }
		},
		/*
		e002 : {
		"id":"e002",
		 "name":"首信易支付",
		 "url":"http://www.beijing.com.cn/",
		 "imageUrl":"../../Config/logos/D002.gif",
		 "favUrl":"../../Config/logos/D002_s.gif",
		 "index":"sxyzf,beijing,首信易支付",
		 "collected":"false",
		 "apptype":"payments",
		 "position":"1",
		 "sublink" : {
		 	 "url" : "http://www.beijing.com.cn/"
		 }
		},
		e003 : {
		"id":"e003",
		 "name":"开联",
		 "url":"http://www.openunion.cn/",
		 "imageUrl":"../../Config/logos/D003.gif",
		 "favUrl":"../../Config/logos/D003_s.gif",
		 "index":"kl,kailian,openunion,开联",
		 "collected":"false",
		 "apptype":"payments",
		 "position":"2",
		 "sublink" : {
		 	 "url" : "http://www.openunion.cn/"
		 }
		},
		*/
		e004 : {
		"id":"e004",
		 "name":"易宝支付",
		 "url":"http://www.yeepay.com/",
		 "imageUrl":"../../Config/logos/D004.gif",
		 "favUrl":"../../Config/logos/D004_s.gif",
		 "index":"yb,yibao,yeepay,易宝支付",
		 "collected":"false",
		 "apptype":"payments",
		 "position":"3",
		 "sublink" : {
		 	 "url" : "http://www.yeepay.com/"
		 }
		}
	},
	shopping:{
		f001 : {
		"id":"f001",
		 "name":"乐淘商城",
		 "url":"http://www.letao.com/",
		 "imageUrl":"../../Config/logos/F001.gif",
		 "favUrl":"../../Config/logos/f001_s.gif",
		 "index":"正品鞋,鞋子,网上鞋城,乐淘",
		 "collected":"false",
		 "apptype":"shopping",
		 "position":"0",
		 "sublink" : {
		 	 "url" : "http://www.letao.com/"
		 }
		},
		f002 : {
		"id":"f002",
		 "name":"淘宝网",
		 "url":"http://www.taobao.com/",
		 "imageUrl":"../../Config/logos/F002.gif",
		 "favUrl":"../../Config/logos/f002_s.gif",
		 "index":"淘宝,掏宝,网上购物,C2C,在线交易,交易市场,网上交易,交易市场,网上买,网上卖,购物网站,团购,网上贸易,安全购物,电子商务,放心买,供应,买卖信息,网店,一口价,拍卖,网上开店,网络购物,打折,免费开店,网购,频道,店铺",
		 "collected":"false",
		 "apptype":"shopping",
		 "position":"1",
		 "sublink" : {
		 	 "url" : "http://www.taobao.com/",
		 	 "txt_ebank":"淘宝商城",
			 "ebank":"http://www.tmall.com/"
		 }
		}
	}
}