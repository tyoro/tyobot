<?php
namespace tyobot;

/**
 * ranbat aot 
 *
 **/

include_once "conf/translate.conf.inc";
include_once "include/Google_Translate_adapter.php";
include_once "bot/base.class.inc";

require_once 'Zend/Loader.php';
	\Zend_Loader::loadClass('Zend_Translate');

class tyobot_translate extends tyobot_base{

	var $translate2en;
	var $translate2ja;

	function __construct() {
		$this->translate2ja = new \Zend_Translate('Quino_Translate_Adapter_Google',
									null,
									'ja',
									array('source' => 'en'));
		$this->translate2en = new \Zend_Translate('Quino_Translate_Adapter_Google',
									null,
									'en',
									array('source' => 'ja'));
	}

	function _setCommand( &$irc ){
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^[\w\s\!\?\.\'\-\,]*$' , $this, 'translate2ja' );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^[^\x01-\x7E]*$' , $this, 'translate2en' );
	}

	function getJoinChannelList(){
		return Array( $this->_convert( TRANSLATE_CHANNEL ) );
	}

	function translate2ja( &$irc, &$data ){
		if( !$this->checkChannel($data) ){return;}
		$this->_notice( $irc, $this->translate2ja->_($data->message), $data->channel );
	}
	function translate2en( &$irc, &$data ){
		if( !$this->checkChannel($data) ){return;}
		$this->_notice( $irc, $this->translate2en->_( $this->_2utf8( $data->message ) ), $data->channel );
	}

	private function checkChannel( &$data ){
		return (TRANSLATE_CHANNEL == $this->_2utf8($data->channel )  );
	}
}

	
