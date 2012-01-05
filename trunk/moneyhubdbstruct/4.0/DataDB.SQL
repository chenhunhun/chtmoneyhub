////////////////================DataDB  
  
 CREATE  TABLE IF NOT EXISTS datExchangeRate (
			   `sysCurrency_id` INTEGER NOT NULL, 
			   `sysCurrency_id1` INTEGER NOT NULL, 
			   `ExchangeRate` FLOAT NOT NULL, 
			   PRIMARY KEY(sysCurrency_id, sysCurrency_id1));

//done 2011-12-09 16:09
CREATE  TABLE IF NOT EXISTS datUserInfo(
  `userid` CHAR(32) primary key NOT NULL DEFAULT 'NOT NULL' ,
  `mail` VARCHAR(256) NOT NULL ,
  `edek` VARCHAR(256) NULL ,
  `etk` VARCHAR(256) NULL ,
  `autoload` INT NULL DEFAULT 0 ,
  `autoinfo` INT NULL DEFAULT 1 , //默认为总是提示
  `autoremb` INT NULL DEFAULT 0 ,
  `popLoad`  INT NULL DEFAULT 0 ,//点击泡泡时，进行用户识别
  `lastload` INT NULL DEFAULT 0 ,//最后登录，进行识别
  `needsynchro` int default(0), // 是否有数据需要同步
  `loadtime` DATA NULL,//记录用户登录时间
  `mark` INT NULL DEFAULT 0 //标记删除
)

CREATE TABLE datFav(
  `favinfo` CHAR(4) NOT NULL ,
  `status` INTEGER  NULL ,
  `favorder` INTEGER  NULL ,
  `deleted` TINYINT(1)  NULL 
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