<?php

/**
 * Bot メイン
 *
 **/

//読み込むconfファイル
include_once "./bot.setting.inc";

include_once "./conf/conf.inc";
include_once "./bot/common.class.inc";
include_once "./bot/ranbat.class.inc";
include_once "Net/SmartIRC.php";

$bot_c = &new tyobot_common();
$bot_r = &new tyobot_ranbat();
$irc = &new Net_SmartIRC();
$irc->setDebug(SMARTIRC_DEBUG_ALL);
$irc->setUseSockets(TRUE);
$irc->setAutoReconnect(TRUE);

// BOT を終了する
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^' . BOT_NICK . DELIMITER . COMMAND_QUIT, $bot_c, COMMAND_QUIT);

// URLのタイトルを表示
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, URL_PATTERN , $bot_c, COMMAND_URL );

// Google検索
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. GOOGLE_CMD . ' ', $bot_c, COMMAND_GOOGLE );

// 計算機機能
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. CALCULATOR_CMD . ' ', $bot_c, COMMAND_CALCULATOR ); 

//twitter
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. TWITTER_CMD . ' ', $bot_c, COMMAND_TWITTER );

// inviteされた
$irc->registerActionhandler(SMARTIRC_TYPE_INVITE, '.*', $bot_c, '__invited__' );

// amazon
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. AMAZON_CMD . ' ', $bot_c, COMMAND_AMAZON );

$channels[] = $bot_c->_convert(CHANNEL);

// ustream
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. USTREAM_CMD .' ', $bot_c, COMMAND_USTREAM );

// ランバト関連
$channels[] = $bot_r->_convert(RANBAT_CHANNEL);

$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_ENTRY_CMD.' ' , $bot_r, COMMAND_RANBAT_ENTRY );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_ENTRY_CMD_S.' ' , $bot_r, COMMAND_RANBAT_ENTRY );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. $bot_r->_convert(RANBAT_ENTRY_ALIAS_CMD), $bot_r, COMMAND_RANBAT_ENTRY_ALIAS );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_CANCEL_CMD.' ' , $bot_r, COMMAND_RANBAT_CANCEL );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_CANCEL_CMD_S.' ' , $bot_r, COMMAND_RANBAT_CANCEL );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_START_CMD , $bot_r, COMMAND_RANBAT_START );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_LIST_CMD , $bot_r, COMMAND_RANBAT_LIST );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_WIN_CMD.' ' , $bot_r, COMMAND_RANBAT_WIN );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_WIN_CMD_S.' ' , $bot_r, COMMAND_RANBAT_WIN );

$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_ENTRY_CMD, $bot_r, COMMAND_KOHAKU_ENTRY );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_LEADER_CMD.' ' , $bot_r, COMMAND_KOHAKU_LEADER );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_LEADER_CMD_S.' ' , $bot_r, COMMAND_KOHAKU_LEADER );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_SELECT_CMD.' ' , $bot_r, COMMAND_KOHAKU_SELECT );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_SELECT_CMD_S.' ' , $bot_r, COMMAND_KOHAKU_SELECT );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_ADD_CMD.' ' , $bot_r, COMMAND_KOHAKU_ADD );

// test
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!entry_start', $bot_r, 'entry_start' );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!gowasu', $bot_r, 'gowasu' );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!dosukoi ', $bot_r, 'dosukoi' );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!dosukoi_rank',$bot_r, 'dosukoi_rank' );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, 'gosama',$bot_r,'gosama');

$irc->connect(SERVER_HOST, SERVER_PORT);
$irc->login(BOT_NICK, BOT_NAME);
$irc->join($channels);
$irc->listen();

?>
