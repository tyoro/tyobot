<?php

// 全体の設定

define("DANI_CHANNEL",'#スパ4道場');
define("DANI_CHANNEL_SUFFIX_PS3",'PS3');
define("DANI_CHANNEL_SUFFIX_360",'360');

// コマンドの定義
define("COMMAND_DANI_ENTRY", "entry");
define("COMMAND_DANI_START", "start");
define("COMMAND_DANI_CHECK", "check");
define("COMMAND_DANI_WIN", "win");
define("COMMAND_DANI_CANCEL", "cancel");

define("COMMAND_DANI_ROOM_CREATE", "room_create");
define("COMMAND_DANI_ROOM_LIST", "room_list");
define("COMMAND_DANI_ROOM_JOIN", "room_join");
define("COMMAND_DANI_ROOM_LEAV", "room_leav");
define("COMMAND_DANI_ROOM_PASS", "room_pass");
define("COMMAND_DANI_ROOM_CHANGE", "room_change");

// メソッドの設定
define("DANI_ENTRY_CMD", "!dani_entry");
define("DANI_ENTRY_CMD_S","e");
define("DANI_ENTRY_CMD_OLD","!de");
define("DANI_START_CMD", "!dani_start");
define("DANI_START_CMD_S", "s");
define("DANI_START_CMD_OLD", "!ds");
define("DANI_WIN_CMD", "!dani_win");
define("DANI_WIN_CMD_S","w");
define("DANI_WIN_CMD_OLD","!dw");
define("DANI_CHECK_CMD", "!dani_check");
define("DANI_CHECK_CMD_S", "i");
define("DANI_CHECK_CMD_OLD", "!dc");
define("DANI_CANCEL_CMD", "!dani_cancel");
define("DANI_CANCEL_CMD_S", "c");
define("DANI_CANCEL_CMD_OLD", "!dcl");

define("DANI_ROOM_CREATE_CMD", 'room' );
define("DANI_ROOM_LIST_CMD", 'room_list' );
define("DANI_ROOM_JOIN_CMD", 'join' );
define("DANI_ROOM_LEAV_CMD", 'leav' );
define("DANI_ROOM_PASS_CMD", 'pass' );
define("DANI_ROOM_CHANGE_CMD", 'change' );

// コマンド関連の設定ここから
define("DANI_CHECK_RANK_MAX_DIFF",4);
define("DANI_CHECK_RANK_UPPER_MAX_DIFF",2);
define("DANI_CHECK_RANK_UP_PT",3);
define("DANI_CHECK_RANK_DOWN_PT",-3);

//System URL path
define("ENTRY_VIEW_PATH","entries/");
define("ENTRY_SEARCH_PATH","entries/search/");


// メッセージ定義
define("DANI_TWIT_PREFIX","【段位戦】");

define("DANI_MSG_ENTRY_START", "段位戦エントリーを受付ました。 {prof}");
define("DANI_MSG_ENTRY_ERROR","既にエントリー済みです。 {prof}");
define("DANI_MSG_ENTRY_ERROR_NO_CHAR","存在しないキャラクターです。 入力：{char}");
define("DANI_PRIV_ENTRY","エントリーに関する設定をサイトから変更できます。 url:{url} pass:{pass}");

define("DANI_MSG_START","段位戦開始：{profA} vs {profB} {option}");
define("DANI_MSG_START_OPTION_UP","{name}の昇格戦です！");
define("DANI_MSG_START_OPTION_DOWN","{name}の降格戦です！");
define("DANI_MSG_START_ERROR_NO_ENTRY","指定されたプレイヤーはエントリーされていません。 {entry}");
define("DANI_MSG_START_ERROR_BAT_ID","指定されたエントリーIDが不正です。 {entry}");
define("DANI_MSG_START_ERROR_DUP","指定されたプレイヤーは既に段位戦を行なっています。 {entry}");
define("DANI_MSG_START_ERROR_EQ","対戦者のPS3 IDが同じです。 自身とは対戦できません。");
define("DANI_MSG_START_ERROR_CHECK","段位戦として認められません。 {msg}");
define("DANI_MSG_START_ERROR_MSG_1","段位が離れすぎています。 段位戦は±".DANI_CHECK_RANK_MAX_DIFF."段以内の相手と行なってください。");
define("DANI_MSG_START_ERROR_MSG_2","段位が離れすぎています。 {name}の昇段戦の為、±".DANI_CHECK_RANK_UPPER_MAX_DIFF."段以内の相手と行なってください。");
define("DANI_MSG_START_ERROR_MSG_3","連戦はできません。");

define("DANI_MSG_CANCEL","以下の組み合わせの段位戦をキャンセルしました。 {profA} vs {profB}");

define("DANI_ANNOUNCE_UP","[{ps3id}さんは勝つと昇格]");
define("DANI_ANNOUNCE_DOWN","[{ps3id}さんは負けると降格]");
define("DANI_ANNOUNCE_SUFFIX","です。");

define("DANI_MSG_WIN","{ps3id}({character_code})の勝利を確認しました。{option} 勝者と対戦する場合は以下のコマンドを入力してください。 :s {id} 自身の番号");
define("DANI_MSG_WIN_OPTION_UP","に昇格");
define("DANI_MSG_WIN_OPTION_DOWN","に降格");
define("DANI_MSG_WIN_OPTION_SUFFIX","となります。");

define("DANI_MSG_CHECK","エントリー情報：{prof} {url}");
define("DANI_MSG_CHECK_PS3ID","PS3ID情報：{info} {url}");

define("DANI_TEMPLATE_PROF","{eid}.{ps3id}({char})【{rank}{point}p】");
define("DANI_TEMPLATE_PROF_S","{ps3id}({char})【{rank}{pt}p】");
//define("DANI_MSG_","");

define("DANI_NOBATLE_ERROR","指定されたプレイヤーは現在段位戦を行なっていません。 {prof}");
define("DANI_ENTRYCODE_ERROR","エントリーの記法に問題があります。 idを入力戴くか、『ps3id:キャラクタ』の形で入力してください。 入力:{input}");
define("DANI_CHAR_SEPARATOR","、");
define("DANI_DB_ERROR","DBとの通信に失敗しました。 処理を停止します。");

define("DANI_TWIT_RESULT","{ps3id}({char})が{rank}{result}しました。");
