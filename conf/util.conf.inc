<?php

define("COMMAND_CALCULATOR","calculator");
define("COMMAND_SUNSET","sunset");
define("COMMAND_CALLNAME","call_name");
define("COMMAND_DELAY_CALLNAME","delay_call_name");


// コマンド関連設定ここから
//CMAND_CALCULATOR 関連
define("CALCULATOR_MSG","計算式の内容が不正です。");
define("CALCULATOR_CMD","cal");


//CMAND_SUNSET 関連
define("SUNSET_MSG","引数が不正です。");
define("SUNSET_CMD","sunset");


//COMMAND_CALL_MAYNAME
define("CALLNAME_PATTERN", "^(?:俺の|僕の|私の|誰か)?名前(?:を)?(?:呼|よ)んで$");

//COMMAND_CALL_DELAY_MAYNAME
define("DELAY_CALLNAME_PATTERN", "^(\d+)秒後に(?:俺の|僕の|私の|誰か)?名前(?:を)?(?:よ|呼)んで$");
define("MAX_DELAY_CALLNAME_TIME",3600);
define("DELAY_CALLNAME_MSG",MAX_DELAY_CALLNAME_TIME."までしか指定できません。");




// コマンド関連設定ここまで

