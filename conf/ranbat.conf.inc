<?php

// 全体の設定

define("RANBAT_CHANNEL",'#格ゲー');
define("RANBAT_TIME",0);
//define("RANBAT_DAYS", Array(3,6));

define('BASE_TOPIC','対戦募集は「すぱよーん」　ランバトは現状無差別です　http://tinyurl.com/ydssh8j　ケータイURL変わりました　http://tinyurl.com/2err2db');

// コマンドの定義
define("COMMAND_RANBAT_ENTRY", "entry");
define("COMMAND_RANBAT_ENTRY_ALIAS","entry_gws");
define("COMMAND_RANBAT_CANCEL", "cancel");
define("COMMAND_RANBAT_START", "start");
define("COMMAND_RANBAT_LIST", "draw_list");
define("COMMAND_RANBAT_WIN", "win");
define("COMMAND_RANBAT_NOW", "now");

define("COMMAND_KOHAKU_ENTRY", "kohaku");
define("COMMAND_KOHAKU_LEADER", "leader");
define("COMMAND_KOHAKU_SELECT","select");
define("COMMAND_KOHAKU_ADD", "add");
//define("COMMAND_KOHAKU_", "");

define("COMMAND_KAKUGE_SSF4", "ssf4");

// メソッドの設定
define("RANBAT_ENTRY_CMD", "!entry");
define("RANBAT_ENTRY_CMD_S","!e");
define("RANBAT_ENTRY_ALIAS_CMD","【ゴワス】");
define("RANBAT_CANCEL_CMD", "!cancel");
define("RANBAT_CANCEL_CMD_S","!c");
define("RANBAT_START_CMD", "!start");
define("RANBAT_LIST_CMD", "!list");
define("RANBAT_WIN_CMD", "!win");
define("RANBAT_WIN_CMD_S","!w");
define("RANBAT_NOW_CMD", "!now");

define("KOHAKU_ENTRY_CMD", "!kohaku" );
define("KOHAKU_LEADER_CMD", "!leader");
define("KOHAKU_LEADER_CMD_S", "!l");
define("KOHAKU_SELECT_CMD","!select");
define("KOHAKU_SELECT_CMD_S","!s");
define("KOHAKU_ADD_CMD","!add");
//define("KOHAKU_GET_CMD","!get");

define("KAKUGE_SSF4_CMD","すぱよーん");

// コマンド関連の設定ここから
define("RANBAT_DUPLICATION_COMMAND_THROUGH_TIME",20);

define("GAMELOG_PATH",'games/');

// ランバト メッセージ定義
define("RANBAT_MSG_SCHEDULER_ANNOUNCE", "本日は{time}より『{title}』が開催されます。 エントリー受付開始は開始30分前です。");
define("RANBAT_MSG_SCHEDULER_ENTRY_1H_AGO", "『{title}』開始1時間前です。  30分後にエントリーの受付け開始します。");
define("RANBAT_MSG_SCHEDULER_ENTRY_START", "{title} エントリー開始時刻です。 『".RANBAT_ENTRY_CMD." PSNID キャラクター名』を入力してください。");
define("RANBAT_MSG_SCHEDULER_ENTRY_START_ERROR", "{title} エントリー開始時刻でが、大会中な為エントリー受付けを行いません。");
define("RANBAT_MSG_SCHEDULER_START_TIME","{title}開始時刻になりました。 現在の参加者は{user_cnt}名です。  エントリーがもう無い場合は『".RANBAT_START_CMD."』と入力してください。");

define("RANBAT_MSG_ENTRY_START", "ランバト エントリー受付を開始します。 『".RANBAT_ENTRY_CMD." PSNID キャラクター名』を入力してください。");
define("RANBAT_PRIV_ENTRY","エントリーを受け付けました。PSNID:{name},キャラクター:{char}");
define("RANBAT_PRIV_ENTRY_UPDATE","キャラクターの変更を受け付けました。PSNID:{name},キャラクター:{char}");
define("RANBAT_PRIV_ENTRY_ERROR","そのユーザーは既にそのキャラクターでエントリーしています。");
define("RANBAT_PRIV_CANCEL","エントリーのキャンセルを受け付けました。");
define("RANBAT_PRIV_CANCEL_ERROR","そのユーザーはエントリーしていません。");
define("RANBAT_MSG_START_LOT","エントリーを締め切りました。 今回の参加者は{user_cnt}名です。");
define("RANBAT_PRIV_START_ERROR","現在エントリー募集を行なっていない為、以下のコマンドは無効です。(".RANBAT_START_CMD.")");
define("RANBAT_MSG_START_ERROR","エントリーを締切ました。 参加者が3人以下な為進行を終了します。");
define("RANBAT_PRIV_WINNER_CHECK","{winner}さんの勝利を確認しました。");
define("RANBAT_PRIV_WINNER_UPDATE","対戦結果を更新しました。");
define("RANBAT_PRIV_WINNER_ERROR","報告済みの勝負結果です。");
define("RANBAT_PRIV_WINNER_TIME_ERROR","同じ勝利報告が".RANBAT_DUPLICATION_COMMAND_THROUGH_TIME."秒以内に行なわれた為無視しました。");
define("RANBAT_MSG_NOW_MATCHING","{now}の現在の対戦表は以下になります。");
define("RANBAT_MSG_CHAMP","今回の優勝者は{chanp}さん、準優勝は{2nd}さんでした。  おめでとうございます！");
define("RANBAT_PRIV_NO_WAIT","既に大会が開始されています。");
//define("RANBAT_MSG_","");

define("KOHAKU_MSG_ENTRY_START","紅白戦のエントリー受付を開始します。『".RANBAT_ENTRY_CMD." PSNID キャラクター名』を入力してください。");
define("KOHAKU_MSG_GET","");
define("KOHAKU_PRIV_GET","{user_name}さんを取りました。");
define("KOHAKU_MSG_GET_ERROR","");
define("KOHAKU_MSG_PLACE_LEADER","チームリーダーを確認します。 『".KOHAKU_LEADER_CMD." D(A|B) (PSNID|num) [nick]』を入力してくださ。 チームはAかBを選択してください。 nickが省略された場合、発言者をそのPSNIDと紐付けます。");
define("KOHAKU_MSG_LEADER_CHECK","{t_name}チームのリーダーを確認しました。 PSN ID:{name} IRC nick:{nick}");
define("KOHAKU_MSG_LEADER_CHANGE","{t_name}チームのリーダーを変更しました。 PSN ID:{name} IRC nick:{nick}");
define("KOHAKU_MSG_LEADER_NICK_CHANGE","{t_name}チームのリーダーのnickを更新しました。 PSN ID:{name} IRC nick:{nick}");
define("KOHAKU_MSG_NO_ENTRY","その名前のプレイヤーはエントリーしていません。 PSN id:{name}");
define("KOHAKU_MSG_MEMBER_SELECT","{t_name}チームのリーダーさん、メンバーを選んでください。 『".KOHAKU_SELECT_CMD." (PSNID|num)』");
define("KOHAKU_MSG_MEMBER_SELECT_LAST","残り一人な為、{t_name}チームに{name}さんを追加しました。");
define("KOHAKU_MSG_MEMBER_SELECT_LAST_HP","また、奇数人数な為BチームのリーダーさんのHPを2とします。");
define("KOHAKU_MSG_MEMBER_SELECT_ERROR","選択済みのプレイヤーです。 他の人を選択してください。 PSN id:{name}");
define("KOHAKU_MSG_ADD_MEMBER","選択対象にプレイヤーを追加しました。 PSN ID:{name} IRC nick:{nick}");
define("KOHAKU_MSG_ADD_BATTLE","チームにプレイヤーを追加しました。 PSN ID:{name}");
define("KOHAKU_MSG_ADD_BATTLE_A","A".KOHAKU_MSG_ADD_BATTLE." それに伴ないBチームリーダーのHPを1増やしました。");
define("KOHAKU_MSG_ADD_BATTLE_B","B".KOHAKU_MSG_ADD_BATTLE." それに伴ないBチームリーダーのHPを1減らしました。");
define("KOHAKU_MSG_ADD_ERROR","そのユーザーは既に参加しています。");
define("KOHAKU_MSG_SELECT_START","両チームのリーダーが確定しました。 ".KOHAKU_MSG_MEMBER_SELECT);
define("KOHAKU_MSG_SELECT","{t_name}チームのリーダーさん、戦うプレイヤーを指定してください。『".KOHAKU_SELECT_CMD." (PSNID|num)』");
define("KOHAKU_MSG_SELECT_ERROR","既に戦闘済みのプレイヤーです。 他の人を指定してください。 PSN ID:{name}");
define("KOHAKU_MSG_SELECT_ERROR2","チームメンバーではありません。 PSN ID:{name}");
define("KOHAKU_MSG_START","メンバーが確定した為、紅白戦を開始します。");
define("KOHAKU_MSG_SELECTED","第{n}戦：【A】{a_name} VS 【B】{b_name} ({s_name}から招待してください。)");
define("KOHAKU_MSG_WINNER_CHECK","{name}さんの勝利を確認しました。 {t_name}のリーダーさんは次に戦うプレイヤーを指定してください。");
define("KOHAKU_MSG_TEAM_WIN","{t_name}チームに戦えるプレイヤーが残っていない為{win_name}チームの勝利です！ おめでとうございます！");
define("KOHAKU_MSG_URAGIRI_START","{t_name}チームに対戦可能なプレイヤーが残っている為、裏切りモードを開始します。");
define("KOHAKU_MSG_GAMESET","両チームに戦えるプレイヤーが残っていない為、紅白戦を終了します。");
define("KOHAKU_MSG_URAGIRI_LAST_PLAYER","戦えるプレイヤーが残り一人な為、自動で選出されます。");
define("KOHAKU_MSG_","");

define("KAKUGE_TWIT_SSF4","{nick}さんが対戦相手を求めています。 {option}");

define("KAKUGE_TOPIC_OPEN","大会が開催中です {url}");
