<?php
namespace tyobot;

/**
 * Bot Base class
 *
 **/

include_once "include/common_base.inc";

class tyobot_base extends common_base {

	function __invited__ (&$irc, &$data ) {
		$irc->join($data->message);
		$this->_notice( $irc, $data->nick.INVITE_MSG, $data->message );
	}

	function getJoinChannelList(){
		return Array();
	}

	function _setCommand( &$irc ){
		d('please override:_setCommand');
	}

	/**
	 * DBのエラーを出力する。
	 *
	 * @param mixed $irc
	 * @access private
	 * @return void
	 */
	protected function sendDBError( &$irc = null ){
		d( $this->conn->ErrorMsg() );
		if(is_null($irc)){
			$this->_notice($irc, DANI_DB_ERROR, $data->channel );
		}
	}

}
?>
