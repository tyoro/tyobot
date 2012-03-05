<?php

define("COMMAND_QUIT", "quit");          // BOT を終了するコマンド
define("COMMAND_URL", "url");        // URLが含まれる発言があると
define("COMMAND_GOOGLE","google");   // コマンドからgoogle検索を行なう
define("COMMAND_CALCULATOR","calculator");
define("COMMAND_TWITTER","twitter");
define("COMMAND_FACEBOOK","facebook");
define("COMMAND_USTREAM","ustream");
define("COMMAND_AMAZON","amazon");
define("COMMAND_ASIN","asin");
define("COMMAND_ALIAS","alias");
define("COMMAND_ITUNES","itunes");
define("COMMAND_ITUNES_MUSIC","itunes_m");
define("COMMAND_ITUNES_APP","itunes_a");
define("COMMAND_ITUNES_IPAD","itunes_ipad");
define("COMMAND_ZUNE_WP7_APP","zune_wp7_app");
define("COMMAND_SUNSET","sunset");

define("COMMAND_GOOGLE_TRANSIT","google_transit");


// コマンド関連設定ここから
// COMMAND_URL 関連
define("URL_ERR_MSG", "URLの解析に失敗しました。");
define("URL_TITLE_MAX_LENGTH", 64 );
define("URL_AMAZON_ITEM_LINK_REGEX","/amazon(\.co){0,1}\.jp.*[^0-9A-Z=]([B0-9][0-9A-Z]{9})([^0-9A-Z=]|$)/");
define("URL_AMAZON_TINY_URL","http://a.tyo.ro/");
define("URL_NICONICO_WATCH_LINK_REGEX","/^http:\/\/www\.nicovideo\.jp\/watch\/([sn]?m?[\d]*)/");
define("URL_YOUTUBE_WATCH_LINK_REGEX","/^http:\/\/www\.youtube\.com\/watch?[^v]*v=([\w-]*)/");
//define("URL_GOOGLE_SPREADSHEETS_LINK_REGEX","/^https?:\/\/spreadsheets\.google\.com\/ccc\?(?<!key=).*key=(\w*)/");
define("URL_GOOGLE_SPREADSHEETS_LINK_REGEX","/^https?:\/\/.*spreadsheets\.google\.com\%2Fccc\%3F(?<!key\%3D).*key\%3D([^\%]*)/");
define("URL_WIKIPEDIA_TOPPAGE_LINK_REGEX","/^http:\/\/ja\.wikipedia\.org\/wiki\/$/");

define("URL_TWITTER_STATUS_LINK_REGEX","/^https?:\/\/(mobile\.)?twitter\.com\/(\#\!\/)?(\w*)\/status(es)?\/(\w*)/");
define("URL_TWITTER_USER_LINK_REGEX","/^https?:\/\/(mobile\.)twitter\.com\/(\#\!\/)?(\w*)$/");

// COMMAND_QUIT 関連（それぞれの意味については、察してください）
define("QUIT_MSG", "終了します");

// COMMAND_GOOGLE 関連
define("GOOGLE_MSG","正常に検索が終了しませんでした。");
define("GOOGLE_CMD","g");
define("GOOGLE_CNT","3");
define("GOOGLE_DESCRIPTION_LENGTH",40);

//COMMAND_GOOGLE_TRANSIT
define("GOOGLE_TRANSIT_MSG","駅名が見付かりませんでした。");
define("GOOGLE_TRANSIT_CMD","gt");


//CMAND_CALCULATOR 関連
define("CALCULATOR_MSG","計算式の内容が不正です。");
define("CALCULATOR_CMD","cal");

//COMMAND_TWITTER 関連
define("TWITTER_MSG","そのidのユーザーは存在しません。");
define("TWITTER_CMD","t");

define("TWITTER_TIMELINE_MAX",5);

//COMMAND_FACEBOOK 関連
define("FACEBOOK_MSG","そのidのユーザーは存在しません。");
define("FACEBOOK_CMD","f");


//COMMAND_USTREAM 関連
define("USTREAM_CMD","u");
define("USTREAM_URL","http://www.ustream.tv/channel/");
define("USTREAM_MSG_ERROR","■this channnel is not found.");

//COMMAND_ALIAS 関連
define("ALIAS_CMD","al");
define("ALIAS_MSG_ERROR","未登録のaliasです。");

//COMMAND_AMAZON_SEARCH関連
define("AMAZON_MSG_ERROR","正常に検索が終了しませんでした。");
define("AMAZON_MSG_NOT_FOUND","ヒットしませんでした。");
define("AMAZON_CMD","a");
define("ASIN_CMD","asin");
define("AMAZON_CNT","3");
define("AMAZON_DESCRIPTION_LENGTH",40);

//COMMAND_ITUNES
define("ITUNES_MSG_ERROR","正常に検索が終了しませんでした。");
define("ITUNES_MSG_NOT_FOUND","ヒットしませんでした。");
define("ITUNES_CMD","is");
define("ITUNES_APP_CMD","ia");
define("ITUNES_IPAD_CMD","ipad");
define("ITUNES_MUSIC_CMD","im");
define("ITUNES_CNT","3");
define("ITUNES_DESCRIPTION_LENGTH",40);
define("ITUNES_BASE_URL","http://ax.itunes.apple.com/WebObjects/MZStoreServices.woa/wa/wsSearch?");
define("ITUNES_TINY_URL","http://its.tyo.ro/");

//COMMAND_WP7
define("WP7_MSG_ERROR","正常に検索が終了しませんでした。");
define("WP7_MSG_NOT_FOUND","ヒットしませんでした。");
define("WP7_CMD","wp");
define("WP7_CNT","3");
define("WP7_BASE_URL","http://catalog.zune.net/v3.2/ja-JP/apps?clientType=WinMobile%207.1&store=zest&q=");
define("WP7_APP_URL","http://www.windowsphone.com/ja-JP/apps/");

//CMAND_SUNSET 関連
define("SUNSET_MSG","引数が不正です。");
define("SUNSET_CMD","sunset");

// コマンド関連設定ここまで


// システムメッセージ
define("INVITE_MSG", "にinviteされました");
