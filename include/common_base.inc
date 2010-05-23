<?php
namespace tyobot;

/**
 * common base クラス
 *
 **/

include_once "include/util.inc";
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

	function _topic( &$irc, $str, $channel ){
		$irc->setTopic( $this->_convert($channel) ,$this->_convert($str) );
	}

	function _twit($str,$options=null){
		$id = TWITTER_ID;
		$pass = TWITTER_PASS;

		if( isset( $options) && is_array($options) ){
			$id = $options['id'];
			$pass = $options['pass'];
		}
		try{
			$twitter =& new \Services_Twitter( $id, $pass );
			$twitter->statuses->update($str);
		}catch( \Services_Twitter_Exception $e ){
			d( $e->getMessage() );
			error_log( $e->getMessage(), 0 );
		}catch( Exception $e) {
			d( $e->getMessage() );
			error_log( $e->getMessage(), 0 );
		}
	}

  	function _template($str,$param){
		if( is_array($param) && count($param) ){
			foreach( $param as $k => $p ){ $keys[] = '{'.$k.'}';  }
			return str_replace( $keys ,$param, $str );
		}
		return $str;
			
	}


	function _command($str,$cmd){
		if( !is_array( $cmd ) ){ $cmd = array( $cmd ); }
		foreach( $cmd as $c ){
			if( strpos( $str, $c ) === 0 ){
				$str = substr( $str, strlen( $c ) );
				break;
			}
		}
		//$str = str_replace( $cmd, '', $str );
		$ret = explode( ' ' , str_replace( '　', ' ', $str ) );	
		return $ret;
	}
	
	function _sep_longtext( $text, $sep = ',', $max_text = 300 ){
		if( strlen( $text ) < $max_text ){
			return $text;
		}
		$a = explode( $sep, $text ); 
		$ret = Array();
		$r = 0;
		$c = count($a);
		$cnt = 0;
		for($i=0;$i<$c;$i++){
			$s = $a[$i];
			if( $i+1<$c && $cnt+strlen($a[$i+1])-$r*$max_text>$max_text){
				$s .= "\n";
				$r++;
			}else if($i+1<$c){ $s .= $sep; }
			$ret[] = $s;
			$cnt += strlen($s);
		}
		return join($ret);
	}
}

?>
