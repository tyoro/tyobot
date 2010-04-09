<?php

/**
 * common base クラス
 *
 **/

include_once "./include/util.inc";
include_once "Services/Twitter.php";
include_once "include/adodb/adodb.inc.php";

class common_base {

	function _2utf8 ( $str ) {
		if( ( $enc = detect_encoding_ja( $str ) ) != 'UTF8' ){
			$str = mb_convert_encoding($str, 'utf8', $enc );
		}
	return $str;
	}

	/**
	 * IRC のエンコーディングへ変換
	 *
	*/
	function _convert ($str) {
		if( ( $enc = detect_encoding_ja( $str ) ) != IRC_ENCODING ){
			$str = mb_convert_encoding($str, IRC_ENCODING, $enc );
		}
		return $str;
	}

	function _notice( &$irc, $str, $channel = CHANNEL ){
		$str = explode( "\n", $str ); 
		foreach( $str as $s ){
			if( strlen( $s ) ){
				$irc->message(SMARTIRC_TYPE_NOTICE, $this->_convert($channel),$this->_convert($s));
			}
		}
	} 

	function _message( &$irc, $str, $channel = CHANNEL ){
		$str = explode( "\n", $str );  
		foreach( $str as $s ){
			if( strlen( $s ) ){
				$irc->message(SMARTIRC_TYPE_CHANNEL, $this->_convert($channel),$this->_convert($s));
			}   
		}    
	}

	function _priv( &$irc, $str, $user ){
		$str = explode( "\n", $str );  
		foreach( $str as $s ){
			$len = strlen( $s );
			if( $len ){
				$irc->message(SMARTIRC_TYPE_NOTICE, $this->_convert($user),$this->_convert($s));
			}   
		}    
	}

	function _twit($str){
		$twitter =& new Services_Twitter( TWITTER_ID,TWITTER_PASS );
		$twitter->statuses->update($str);
	}

  	function _template($str,$param){
		if( is_array($param) && count($param) ){
			foreach( $param as $k => $p ){ $keys[] = '{'.$k.'}';  }
			return str_replace( $keys ,$param, $str );
		}
		return $str;
			
	}


	function _command($str,$cmd){
		$str = str_replace( $cmd, '', $str );
		$ret = explode( ' ' , str_replace( '　', ' ', $str ) );	
		return $ret;
	}
}

?>
