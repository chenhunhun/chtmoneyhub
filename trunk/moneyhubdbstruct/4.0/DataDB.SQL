////////////////================DataDB  
  
 CREATE  TABLE IF NOT EXISTS datExchangeRate (
			   `sysCurrency_id` INTEGER NOT NULL, 
			   `sysCurrency_id1` INTEGER NOT NULL, 
			   `ExchangeRate` FLOAT NOT NULL, 
			   PRIMARY KEY(sysCurrency_id, sysCurrency_id1));

CREATE  TABLE IF NOT EXISTS datUserInfo(
  `userid` CHAR(32) primary key NOT NULL DEFAULT 'NOT NULL' ,
  `mail` VARCHAR(256) NOT NULL ,
  `edek` VARCHAR(256) NULL ,
  `etk` VARCHAR(256) NULL ,
  `autoload` INT NULL DEFAULT 0 ,
  `autoinfo` INT NULL DEFAULT 0 ,
  `autoremb` INT NULL DEFAULT 0
)

CREATE TABLE `datUSBKeyInfo` (
 `vid` INTEGER NOT NULL,
 `pid` INTEGER NOT NULL,
 `mid` INTEGER NOT NULL ,
 `fav` CHAR(4),
 `xml` CHAR(512),
 `bkurl` CHAR(256),
 `ver` CHAR(16),
 `status` INT
)

CREATE  TABLE IF NOT EXISTS tbTotalEvent(
  `id` BIGINT PRIMARY KEY NOT NULL DEFAULT 0 ,
  `mark` INT(2) NULL DEFAULT 0,
  `event_date` INTEGER  NULL ,
  `repeat` INTEGER  NULL ,
  `alarm` INTEGER  NULL ,
  `status` INTEGER  NULL ,
  `datestring` CHAR(12) NULL ,
  `type` INT NULL ,
  `datUserInfo_userid` CHAR(32) NOT NULL
)

CREATE  TABLE IF NOT EXISTS datDBInfo(
  `schema_version` INTEGER  NOT NULL DEFAULT 8
)