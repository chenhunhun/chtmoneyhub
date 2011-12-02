//done
CREATE TABLE tbAccount(
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NOT NULL ,
  `tbBank_id` BIGINT  NULL ,
  `tbAccountType_id` INTEGER  NOT NULL ,
  `AccountNumber` VARCHAR(256) NULL ,
  `Comment` VARCHAR(256) NULL ,
  `order` INT(4) NOT NULL DEFAULT 0 ,
  `EndDate` CHAR(4) NULL DEFAULT 0,
  `keyInfo` varchar(255)
)
		
CREATE TABLE tbAccountGetBillMonth(
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NULL DEFAULT 0 ,
  `mark` INT(2) NULL DEFAULT 0 ,
  `tbmonth` VARCHAR(6) NULL DEFAULT NULL ,
  `tbKeyInfo` VARCHAR(100) NULL DEFAULT NULL ,
  `tbAccount_id` BIGINT NOT NULL 
)

CREATE TABLE tbAccountType(
  `id` integer primary key NOT NULL,
  `Name` VARCHAR(256) NULL 
)

//done	
CREATE TABLE tbBank(
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NOT NULL ,
  `classId` INTEGER NOT NULL,
  `BankID` VARCHAR(4) NULL ,
  `Phone` VARCHAR(256) NULL ,
  `Website` VARCHAR(256) NULL ,
  `order` INT(4) NOT NULL DEFAULT 0 
)
	
//done 	2011-11-29 15:35
CREATE TABLE tbCategory1(
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NULL,
  `Type` TINYINT NOT NULL,
  `order` INT(4) NOT NULL DEFAULT 0
)

CREATE TABLE tbCategory2(
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NULL ,
  `tbCategory1_id` BIGINT NOT NULL ,
  `order` INT(4) NOT NULL DEFAULT 0 
)

CREATE TABLE tbCurrency(
  `id` integer  primary key NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NOT NULL ,
  `order` INT(4) NOT NULL DEFAULT 0
)

CREATE TABLE tbDBInfo(schema_version INT)

CREATE TABLE tbEvent(
  `id` BIGINT PRIMARY KEY NOT NULL DEFAULT 0 ,
  `UT` BIGINT NULL DEFAULT 0 ,
  `mark` INT(2) NULL DEFAULT 0 ,
  `event_date` INTEGER  NULL ,
  `description` CHAR(256) NULL ,
  `repeat` INTEGER  NULL ,
  `alarm` INTEGER  NULL ,
  `status` INTEGER  NULL ,
  `datestring` CHAR(12) NULL ,
  `tbAccount_id` BIGINT NULL ,
  `tbSubAccount_id` BIGINT NULL ,
  `type` INT(2) NULL 
)


CREATE TABLE tbFav(
  `id` BIGINT PRIMARY KEY NOT NULL DEFAULT 0 ,
  `UT` BIGINT NULL DEFAULT 0 ,
  `mark` INT(2) NULL DEFAULT 0 ,
  `favinfo` CHAR(4) NOT NULL ,
  `status` INTEGER  NULL ,
  `favorder` INTEGER  NULL ,
  `deleted` TINYINT(1)  NULL 
)
  
//done 
CREATE TABLE tbPayee(
  `id` bigint  PRIMARY KEY NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `Name` VARCHAR(256) NOT NULL ,
  `email` VARCHAR(256),
  `tel` VARCHAR(256),
  `order` INT(4) NOT NULL DEFAULT 0 
)
  
  
CREATE TABLE tbProductChoice(
  `id` BIGINT PRIMARY KEY  NOT NULL DEFAULT 0 ,
  `UT` BIGINT NULL DEFAULT 0 ,
  `mark` INT(2) NULL DEFAULT 0 ,
  `interestchoice` VARCHAR(256) NULL DEFAULT NULL ,
  `durationchoice` VARCHAR(256) NULL DEFAULT NULL ,
  `currencychoice` VARCHAR(256) NULL DEFAULT NULL ,
  `bankchoice` VARCHAR(256) NULL DEFAULT NULL ,
  `userId` BIGINT NULL DEFAULT 0 
)


//done
CREATE TABLE tbSubAccount(
  `id` BIGINT primary key NOT NULL DEFAULT 0 ,
  `UT` BIGINT NOT NULL DEFAULT 0 ,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `tbAccount_id` BIGINT NOT NULL ,
  `tbCurrency_id` BIGINT NOT NULL ,
  `name` VARCHAR(256) NOT NULL ,
  `OpenBalance` FLOAT NOT NULL ,
  `Balance` FLOAT NOT NULL ,
  `Days` INTEGER  NULL ,
  `EndDate` DATE NULL DEFAULT 0 ,
  `Comment` VARCHAR(256) NULL ,
  `tbAccountType_id` INTEGER  NULL ,
  `order` INT(4) NOT NULL DEFAULT 0
)
  
//done
//done 2011-11-29 15:34
CREATE TABLE tbTransaction(
  `id` BIGINT primary key NOT NULL DEFAULT 0,
  `UT` BIGINT NOT NULL DEFAULT 0,
  `mark` INT(2) NOT NULL DEFAULT 0 ,
  `TransDate` DATE NOT NULL ,
  `tbPayee_id` BIGINT NULL ,
  `tbCategory2_id` BIGINT NULL ,
  `Amount` FLOAT NOT NULL DEFAULT 0 ,
  `direction` BIGINT NULL DEFAULT 0 ,
  `tbSubAccount_id` BIGINT NOT NULL ,
  `tbSubAccount_id1` BIGINT NULL ,
  `ExchangeRate` FLOAT NULL ,
  `Comment` VARCHAR(256) NULL ,
  `sign` VARCHAR(256) NULL,
  `transactionClasses` int(2),
  `order` INT(4) NOT NULL DEFAULT 0
)
  





