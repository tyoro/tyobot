<?php
namespace tyobot;

/**
 * common Bot クラス
 *
 **/

include_once "conf/util.conf.inc";

include_once "./bot/base.class.inc";

class tyobot_util extends tyobot_base {


	var $conn = null;
	
	function __construct( &$conn ) {
		parent::__construct();
		$this->conn = $conn;
	}

	function _setCommand( &$irc ){
		
		// calculator
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. CALCULATOR_CMD . ' ', $this, COMMAND_CALCULATOR );

		//sunset
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. SUNSET_CMD . ' ', $this, COMMAND_SUNSET );

		// omikujiされた
		//$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!omikuji', $this, 'omikuji' );

		// 名前を呼んで
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, CALLNAME_PATTERN , $this, COMMAND_CALLNAME );

		// 遅らせて名前を呼んで
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, DELAY_CALLNAME_PATTERN , $this, COMMAND_DELAY_CALLNAME );
	}


	function calculator(&$irc, &$data) {
		$message = $this->_2utf8($data->message );
		$text = substr($message, strlen(CALCULATOR_CMD)+1);
		$m = Array();
		if (preg_match('/[^0123456789()+-\/\*\% ]/', $text, $m)) {
			$this->_notice($irc, CALCULATOR_MSG, $data->channel);
			return;
		}
		eval('$ret = '."$text;" );
		$this->_notice($irc, $ret, $data->channel);
	}


	function omikuji(&$irc, &$data) {
		$list = array(
			28 =>'【大吉】',
			42 => '【中吉】',
			56 => '【吉】',
			67 => '【小吉】',
			78 => '【末吉】',
			85 => '【凶】',
			92 => '【大凶】',
			94 => '【ぴょん吉】',
			96 => '【だん吉】',
			97 => '【とらきち】',
			98 => '【豚】',
			99=>'【神】',
			100=>'【女神】');
		$num = rand(0,100);
		foreach( $list as $key => $val){
			if( $num <= $key ){
				$str = $data->nick."さんは...".$val."です！";
				$this->_notice( $irc, $str, $data->channel );
				return;
			}
		}
	}

	function call_name(&$irc, &$data)
	{
		$this->_message( $irc, $data->nick, $data->channel);
	}

	function delay_call_name(&$irc, &$data)
	{
		$s = (int) $data->message;
		if( $s > MAX_DELAY_CALLNAME_TIME )
		{
			$this->_notice( $irc, DELAY_CALLNAME_MSG, $data->channel );
			return;
		}
		$irc->registerDilayAction($s,function($args) use($irc,$data){
			$this->_message( $irc, $data->nick, $data->channel);

		});
	}


	function sunset(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), SUNSET_CMD.' ' );
		$time =null;
		$lat = 35.65861;
		$lon = 139.745447;
		if( is_numeric($args[1] )){
			$lat = $args[1]; 
		}
		if( is_numeric($args[2] )){
			$lon = $args[2]; 
		}
		switch( $args[0] ){
			case 'now':
				$time = date_sunset ( time(), SUNFUNCS_RET_STRING, $lat, $lon );
				break;
			default:
				$this->_notice( $irc, SUNSET_MSG , $data->channel );
				return;
			
		}
		$this->_notice( $irc, $time, $data->channel );
	}

}
?>
