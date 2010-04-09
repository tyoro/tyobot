<?php

include_once "Net/SmartIRC.php";
class Net_SmartIRC_JP extends Net_SmartIRC
{

	var $_charset = 'auto';

	/**
	 * 接続先IRCサーバーの内部文字コードを指定する。
	 * 現在はコマンドのチェック時のutf8への変換にしか使わない。
	 * 将来的にはSmartIRC内での文字コードをutf8に統一して、ライブラリの利用側では変換を意識しなくて良いようにすべきだと思う。
     *
	 * @param string $charset
	 * @return void
	 * @access public
	 */
	function setCharset($charset){
		$this->_charset = $charset;
	}	


    /**
     * tries to find a actionhandler for the received message ($ircdata) and calls it
     * 正規表現のチェック時に入力をutf8に変換し、マルチバイトで行なう。
	 * (コマンドの設定も全てutf8で行なう事になる
	 *
     * @param object $ircdata
     * @return void
     * @access private
     */
    function _handleactionhandler(&$ircdata)
    {   
        $handler = &$this->_actionhandler;
        $handlercount = count($handler);
        for ($i = 0; $i < $handlercount; $i++) {
            $handlerobject = &$handler[$i];
            
            if (substr($handlerobject->message, 0, 1) == '/') {
                $regex = $handlerobject->message;
            } else {
                $regex = '/'.$handlerobject->message.'/';
            }
            
            if (($handlerobject->type & $ircdata->type) &&
                (preg_match($regex, mb_convert_encoding( $ircdata->message, 'UTF-8', $this->_charset ) ) == 1)) {
                
                $this->log(SMARTIRC_DEBUG_ACTIONHANDLER, 'DEBUG_ACTIONHANDLER: actionhandler match found for id: '.$i.' type: '.$ircdata->type.' message: "'.$ircdata->message.'" regex: "'.$regex.'"', __FILE__, __LINE__);
                
                $methodobject = &$handlerobject->object;
                $method = $handlerobject->method;
                
                if (@method_exists($methodobject, $method)) {
                    $this->log(SMARTIRC_DEBUG_ACTIONHANDLER, 'DEBUG_ACTIONHANDLER: calling method "'.get_class($methodobject).'->'.$method.'"', __FILE__, __LINE__);
                    $methodobject->$method($this, $ircdata);
                } else {
                    $this->log(SMARTIRC_DEBUG_ACTIONHANDLER, 'DEBUG_ACTIONHANDLER: method doesn\'t exist! "'.get_class($methodobject).'->'.$method.'"', __FILE__, __LINE__);                    }
            }
        }
    }
}
