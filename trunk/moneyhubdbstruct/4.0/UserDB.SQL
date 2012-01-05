//done
CREATE TABLE tbAccount(   //主账户表
  `id` BIGINT primary key NOT NULL DEFAULT 0 , //id，按规则生成
  `UT` BIGINT NOT NULL DEFAULT 0 ,  //最后更新时间
  `mark` INT(2) NOT NULL DEFAULT 0 , //标记删除字段，0正常，1删除
  `Name` VARCHAR(256) NOT NULL ,  //账户名
  `tbBank_id` BIGINT  NULL ,  //关联的银行编号
  `tbAccountType_id` INTEGER  NOT NULL , //关联的账户类型编号
  `AccountNumber` VARCHAR(256) NULL ,  //目前未使用 
  `Comment` VARCHAR(256) NULL ,  //详情
  `seq` INT(4) NOT NULL DEFAULT 0 ,  //排序
  `EndDate` CHAR(4) NULL DEFAULT 0,  //信用卡类型到期日和还款日的类型，共4位，前两位到期日，后两位为还款日
  `keyInfo` varchar(255) //账户关联的卡号，账号
)
		
CREATE TABLE tbAccountGetBillMonth( //记录账单抓取月份
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NULL DEFAULT 0 ,
  `mark` INT(2) NULL DEFAULT 0 ,
  `tbmonth` VARCHAR(6) NULL DEFAULT NULL ,  //账单抓取的时间，6位，4位年，2位月
  `tbKeyInfo` VARCHAR(100) NULL DEFAULT NULL ,//卡号,账号
  `tbAccount_id` BIGINT NOT NULL  //账号的id
)

CREATE TABLE tbAccountType(  //账户类型表，100以后记录了储蓄卡子账户的类型
  `id` integer primary key NOT NULL,
  `Name` VARCHAR(256) NULL 
)

//done	
CREATE TABLE tbBank( //金融机构表
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NOT NULL ,
  `classId` INTEGER NOT NULL, //金融机构类型
  `BankID` VARCHAR(4) NULL ,  //编号
  `Phone` VARCHAR(256) NULL , //电话
  `Website` VARCHAR(256) NULL , //网址
  `seq` INT(4) NOT NULL DEFAULT 0 //排序
)
	
//done 	2011-11-29 15:35
CREATE TABLE tbCategory1( //主分类
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NULL,
  `Type` TINYINT NOT NULL, //类型，0，为收入，1，为支出
  `seq` INT(4) NOT NULL DEFAULT 0 //排序
)

CREATE TABLE tbCategory2( //子分类
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NULL ,
  `tbCategory1_id` BIGINT NOT NULL ,//主分类id
  `seq` INT(4) NOT NULL DEFAULT 0 //排序
)

CREATE TABLE tbCurrency( //币种表
  `id` integer  primary key NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NOT NULL ,
  `seq` INT(4) NOT NULL DEFAULT 0 //排序
)

CREATE TABLE tbDBInfo(schema_version INT) //数据库版本

CREATE TABLE tbEvent(  //提醒时间表
  `id` BIGINT PRIMARY KEY NOT NULL DEFAULT 0 ,
  `UT` BIGINT NULL DEFAULT 0 ,
  `mark` INT(2) NULL DEFAULT 0 ,
  `event_date` INTEGER  NULL ,  //提醒时间
  `description` CHAR(256) NULL ,  //提醒内容
  `repeat` INTEGER  NULL ,  //周期
  `alarm` INTEGER  NULL ,   //提前几天提醒
  `status` INTEGER  NULL ,  //是否提醒
  `datestring` CHAR(12) NULL ,  //提醒日期，格式如下：2011-01-01 1
  `tbAccount_id` BIGINT NULL ,  //主账户id
  `tbSubAccount_id` BIGINT NULL , //子账户id
  `type` INT(2) NULL  //提醒类型
)
  
//done 
CREATE TABLE tbPayee(  //支付对象
  `id` bigint  PRIMARY KEY NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NOT NULL ,
  `email` VARCHAR(256),
  `tel` VARCHAR(256),
  `seq` INT(4) NOT NULL DEFAULT 0 //排序 
)
  
  
CREATE TABLE tbProductChoice(
  `id` BIGINT PRIMARY KEY  NOT NULL DEFAULT 0 ,
  `UT` BIGINT NULL DEFAULT 0 ,
  `mark` INT(2) NULL DEFAULT 0 ,
  `interestchoice` VARCHAR(256) NULL DEFAULT NULL , //记录了用户理财产品的保存
  `durationchoice` VARCHAR(256) NULL DEFAULT NULL , //记录了用户理财产品的保存
  `currencychoice` VARCHAR(256) NULL DEFAULT NULL , //记录了用户理财产品的保存                          
  `bankchoice` VARCHAR(256) NULL DEFAULT NULL , //记录了用户理财产品的保存
  `userId` BIGINT NULL DEFAULT 0 //账户id 
)


//done
CREATE TABLE tbSubAccount(
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `tbAccount_id` BIGINT NOT NULL ,  //主账户类型
  `tbCurrency_id` BIGINT NOT NULL , //币种类型
  `name` VARCHAR(256) NOT NULL ,
  `OpenBalance` FLOAT NOT NULL ,  //开户金额
  `Balance` FLOAT NOT NULL ,    //当前余额
  `Days` INTEGER  NULL ,        //储蓄卡类型账户的周期
  `EndDate` DATE NULL DEFAULT 0 ,  //储蓄卡类型账户的到期日
  `Comment` VARCHAR(256) NULL ,  //详情
  `tbAccountType_id` INTEGER  NULL , //储蓄卡类型的子账户类型，对应tbaccounttpye表的100以后的类型
  `seq` INT(4) NOT NULL DEFAULT 0  //排序
)
  
//done
//done 2011-11-29 15:34
CREATE TABLE tbTransaction(  //交易表
  `id` BIGINT primary key NOT NULL DEFAULT 0,
  `UT` BIGINT NOT NULL DEFAULT 0,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `TransDate` DATE NOT NULL , //交易日期
  `tbPayee_id` BIGINT NULL ,  //交易对象
  `tbCategory2_id` BIGINT NULL , //交易分类
  `Amount` FLOAT NOT NULL DEFAULT 0 , //交易金额
  `direction` BIGINT NULL DEFAULT 0 , //转账类型相关记录的id
  `tbSubAccount_id` BIGINT NOT NULL , //交易所属的子账户id
  `tbSubAccount_id1` BIGINT NULL , //转账类型相关记录所属的子账户id
  `ExchangeRate` FLOAT NULL ,  //汇率，未使用
  `Comment` VARCHAR(256) NULL , //详情
  `sign` VARCHAR(256) NULL,  //记录抓账单记录原始日期
  `transactionClasses` int(2),  //标识类型，0，手工帐；1，为系统帐（导账单）
  `seq` INT(4) NOT NULL DEFAULT 0 //排序
)
  





