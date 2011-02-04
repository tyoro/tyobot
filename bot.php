<?php

/**
 * Bot メイン
 *
 **/

//読み込むconfファイル
include_once "./bot.conf.inc";

include_once "./conf/conf.inc";
include_once "./bot/common.class.inc";
include_once "./bot/ranbat.class.inc";
include_once "./bot/dani.class.inc";
include_once "./bot/my.class.inc";
include_once "./include/SmartIRC_JP.php";

$conn =& ADONewConnection('mysql');
$conn->PConnect(DATABASE_HOST, DATABASE_ID, DATABASE_PASS, DATABASE_NAME);
$conn->Execute('set names utf8');

$bot_c = &new tyobot\tyobot_common( $conn );
$bot_r = &new tyobot\tyobot_ranbat( $conn );
$bot_d = &new tyobot\tyobot_dani( $conn );

$irc = &new Net_SmartIRC_JP();
$irc->setDebug(SMARTIRC_DEBUG_ALL);
$irc->setUseSockets(TRUE);
$irc->setAutoReconnect(TRUE);
$irc->setCharset( IRC_ENCODING );

$bot_c->_setCommand( $irc );
$bot_r->_setCommand( $irc );
$bot_d->_setCommand( $irc );

$channels = Array( $bot_c->_convert(CHANNEL) );
$channels = array_merge( $channels, $bot_r->getJoinChannelList() );
$channels = array_merge( $channels, $bot_d->getJoinChannelList() );

$bot_m = &new tyobot\tyobot_my( $conn );
$bot_m->_setCommand( $irc );

// test
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!gowasu', $bot_r, 'gowasu' );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!dosukoi ', $bot_r, 'dosukoi' );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!dosukoi_rank',$bot_r, 'dosukoi_rank' );
$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, 'ｺﾞｻﾏ', $bot_r,'gosama');

$irc->connect(SERVER_HOST, SERVER_PORT);
$irc->login(BOT_NICK, BOT_NAME);
$irc->join($channels);
$irc->listen();

?>
