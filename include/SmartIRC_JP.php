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
     * オーバーライド関数
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


    /**
     * registers a timehandler and returns the assigned id
     *
     * Registers a timehandler in Net_SmartIRC, which will be called in the specified interval.
     * The timehandler id is needed for unregistering the timehandler.
	 *
	 * オーバーライド。
	 * 任意のオブジェクトを関数呼出しに付与する。
     *
     * @see example7.php
     * @param integer $interval interval time in milliseconds
     * @param object $object a reference to the objects of the method
     * @param string $methodname the methodname that will be called when the handler happens
     * @return integer assigned timehandler id
     * @access public
     */
    function registerTimehandler($interval, &$object, $methodname, &$args=null  )
    {
        $id = $this->_timehandlerid++;
        $newtimehandler = &new Net_SmartIRC_timehandler_ex();

        $newtimehandler->id = $id;
        $newtimehandler->interval = $interval;
        $newtimehandler->object = &$object;
        $newtimehandler->method = $methodname;
        $newtimehandler->lastmicrotimestamp = $this->_microint();
		$newtimehandler->args = &$args;

        $this->_timehandler[] = &$newtimehandler;
        $this->log(SMARTIRC_DEBUG_TIMEHANDLER, 'DEBUG_TIMEHANDLER: timehandler('.$id.') registered', __FILE__, __LINE__);

        if (($interval < $this->_mintimer) || ($this->_mintimer == false)) {
            $this->_mintimer = $interval;
        }

        return $id;
    }


    /**
     * Checks the running timers and calls tee registered timehandler,
     * when the interval is reached.
     *
	 * チェック関数の拡張。
	 * 実行関数にオブジェクトを渡す。
	 *
     * @return void
     * @access private
     */
    function _checktimer()
    {
        if (!$this->_loggedin) {
            return;
        }

        // has to be count() because the array may change during the loop!
        for ($i = 0; $i < count($this->_timehandler); $i++) {
            $handlerobject = &$this->_timehandler[$i];
            $microtimestamp = $this->_microint();
            if ($microtimestamp >= ($handlerobject->lastmicrotimestamp+($handlerobject->interval/1000))) {
                $methodobject = &$handlerobject->object;
                $method = $handlerobject->method;
                $handlerobject->lastmicrotimestamp = $microtimestamp;

                if (@method_exists($methodobject, $method)) {
                    $this->log(SMARTIRC_DEBUG_TIMEHANDLER, 'DEBUG_TIMEHANDLER: calling method "'.get_class($methodobject).'->'.$method.'"', __FILE__, __LINE__);
                    $methodobject->$method($this,$handlerobject->args);
                }
            }
        }
    }

	/**
     *
	 * 週一や1日1回といった形で実行したい関数を登録できるタイムハンドラーのカスタマイズ登録関数。
     * 曜日を指定した場合は週一、曜日にnullを渡した場合は毎日指定時間に実行するようになる。
     *
     * @param integer $w 実行する曜日(1つしか指定できない)。 nullを指定された時は毎日。 日曜日を0とした数字をで指定する。
     * @param integer $h 実行する時間。
     * @param integer $m 実行する分。
     * @param object $object a reference to the objects of the method
     * @param string $methodname the methodname that will be called when the handler happens
     * @return integer assigned timehandler id
     * @access public
     */
    function registerTimeScheduler( $w, $h, $m, &$object, $methodname, &$args=null)
    {
		if( is_null( $w ) ){
			//1日分
			$interval = 24*60*60;

			$last_time = mktime( $h, $m, 0);
			if( $last_time > time() ){ $last_time -= $interval; }
		}else{
			//1週間
			$interval = 24*60*60*7;
			$last_time = mktime( $h, $m, 0, date('n'), date('j') - ( date('w') - $w ) );
			if( $last_time > time() ){ $last_time -= $interval; }
		}
        $id = $this->_timehandlerid++;
        $newtimehandler = &new Net_SmartIRC_timehandler_ex();

        $newtimehandler->id = $id;
        $newtimehandler->interval = $interval*1000;
        $newtimehandler->object = &$object;
        $newtimehandler->method = $methodname;
        $newtimehandler->lastmicrotimestamp = "$last_time.0000";//$this->_microint();
		$newtimehandler->args = &$args;

        $this->_timehandler[] = &$newtimehandler;
        $this->log(SMARTIRC_DEBUG_TIMEHANDLER, 'DEBUG_TIMEHANDLER: timehandler('.$id.') registered', __FILE__, __LINE__);

        if (($interval < $this->_mintimer) || ($this->_mintimer == false)) {
            $this->_mintimer = $interval;
        }

        return $id;
    }

}

class Net_SmartIRC_timehandler_ex extends Net_SmartIRC_timehandler{
	var $args;
}
