<?php

$query = $_REQUEST["q"];
$list_content = getupdatedata($query);
echo $list_content;

function getupdatedata($query)
{
	// 这里解析$query请求
	
	$actual_query = base64_decode($query);
	$logfile = getcwd() . "/log/query.txt";
	file_put_contents($logfile, "------------------------------------------\r\n", FILE_APPEND);
	file_put_contents($logfile, $query, FILE_APPEND);	
	file_put_contents($logfile, "\r\n\r\n", FILE_APPEND);
	file_put_contents($logfile, $actual_query . "\r\n", FILE_APPEND);
	file_put_contents($logfile, "\r\n\r\n", FILE_APPEND);
	file_put_contents($logfile, "\r\n\r\n", FILE_APPEND);
	
	$listfile = getcwd() . "/data/list.xml";
	$content = file_get_contents($listfile);
	echo $content;
}

?>