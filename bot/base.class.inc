<?php

/**
 * Bot Base class 
 *
 **/

include_once "./include/common_base.inc";

class tyobot_base extends common_base {

	function __invited__ (&$irc, &$data ) {
		$irc->join($data->message);
		$this->_notice( $irc, $data->nick.INVITE_MSG, $data->message );
	}
  
	function _setCommand( &$irc ){
		d('please override:_setCommand');
	}
}
?>
