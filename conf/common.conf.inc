<?php

define("COMMAND_QUIT", "quit");          // BOT を終了するコマンド
define("COMMAND_URL", "url");        // URLが含まれる発言があると
define("COMMAND_GOOGLE","google");   // コマンドからgoogle検索を行なう
define("COMMAND_CALCULATOR","calculator");
define("COMMAND_TWITTER","twitter");
define("COMMAND_USTREAM","ustream");
define("COMMAND_AMAZON","amazon");


// コマンド関連設定ここから
// COMMAND_URL 関連
define("URL_ERR_MSG", "URLの解析に失敗しました。");
define("URL_TITLE_MAX_LENGTH", 20 );
define("URL_AMAZON_ITEM_LINK_REGEX","/amazon(\.co){0,1}\.jp.*[^0-9A-Z=]([B0-9][0-9A-Z]{9})([^0-9A-Z=]|$)/");
define("URL_AMAZON_TINY_URL","http://a.tyo.ro/");
define("URL_NICONICO_WATCH_LINK_REGEX","/^http:\/\/www\.nicovideo\.jp\/watch\/(sm[\d]*)/");
define("URL_YOUTUBE_WATCH_LINK_REGEX","/^http:\/\/www\.youtube\.com\/watch?[^v]*v=([\w]*)/");


// COMMAND_QUIT 関連（それぞれの意味については、察してください）
define("QUIT_MSG", "終了します");

// COMMAND_GOOGLE 関連
define("GOOGLE_MSG","正常に検索が終了しませんでした。");
define("GOOGLE_CMD","g");
define("GOOGLE_CNT","3");
define("GOOGLE_DESCRIPTION_LENGTH",40);

//CMAND_CALCULATOR 関連
define("CALCULATOR_MSG","計算式の内容が不正です。");
define("CALCULATOR_CMD","c");

//COMMAND_TWITTER 関連
define("TWITTER_MSG","そのidのユーザーは存在しません。");
define("TWITTER_CMD","t");

define("TWITTER_TIMELINE_MAX",5);

//COMMAND_USTREAM 関連
define("USTREAM_CMD","u");
define("USTREAM_URL","http://www.ustream.tv/channel/");
define("USTREAM_MSG_ERROR","■this channnel is not found.");

//COMMAND_AMAZON_SEARCH関連
define("AMAZON_MSG_ERROR","正常に検索が終了しませんでした。");
define("AMAZON_MSG_NOT_FOUND","ヒットしませんでした。");
define("AMAZON_CMD","a");
define("AMAZON_CNT","3");
define("AMAZON_DESCRIPTION_LENGTH",40);

// コマンド関連設定ここまで


// システムメッセージ
define("INVITE_MSG", "にinviteされました");
