<?php
namespace tyobot;

/**
 * ranbat aot 
 *
 **/

include_once "conf/ranbat.conf.inc";
include_once "bot/base.class.inc";

class tyobot_ranbat extends tyobot_base{

	var $scene = enum_ranbat_scene::WAIT;
	var $entry_list = Array();
	var $f_list_draw = false;
	
	var $game = null;
	
	var $conn = null;
	
	var $log = null;

	var $topic = null;

	function __construct( &$conn ){
		$this->conn = $conn;
	 }

	function _setCommand( &$irc ){
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^!entry_start', $this, 'entry_start' );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_ENTRY_CMD.' ' , $this, COMMAND_RANBAT_ENTRY );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_ENTRY_CMD_S.' ' , $this, COMMAND_RANBAT_ENTRY );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. $this->_convert(RANBAT_ENTRY_ALIAS_CMD), $this, COMMAND_RANBAT_ENTRY_ALIAS );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_CANCEL_CMD.' ' , $this, COMMAND_RANBAT_CANCEL );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_CANCEL_CMD_S.' ' , $this, COMMAND_RANBAT_CANCEL );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_START_CMD , $this, COMMAND_RANBAT_START );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_LIST_CMD , $this, COMMAND_RANBAT_LIST );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_WIN_CMD.' ' , $this, COMMAND_RANBAT_WIN );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. RANBAT_WIN_CMD_S.' ' , $this, COMMAND_RANBAT_WIN );

		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_ENTRY_CMD, $this, COMMAND_KOHAKU_ENTRY );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_LEADER_CMD.' ' , $this, COMMAND_KOHAKU_LEADER );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_LEADER_CMD_S.' ' , $this, COMMAND_KOHAKU_LEADER );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_SELECT_CMD.' ' , $this, COMMAND_KOHAKU_SELECT );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_SELECT_CMD_S.' ' , $this, COMMAND_KOHAKU_SELECT );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. KOHAKU_ADD_CMD.' ' , $this, COMMAND_KOHAKU_ADD );
		
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'.KAKUGE_SSF4_CMD, $this, COMMAND_KAKUGE_SSF4 );

		$rs = &$this->conn->Execute( 'select * from ranbat_schedules' );
		if(!$rs){ $this->sendDBError($irc); }		
		else{
			while(!$rs->EOF){
				$rec = $rs->FetchRow();
				$a_time = explode( ':', $rec['announce_time'] );
				$s_time = explode( ':', $rec['start_time'] );
				unset($data);
				$data = Array( 'id' => $rec['id'], 'title' => $rec['game_title'], 'time' => $s_time[0].':'.$s_time[1] );
				$irc->registerTimeScheduler( $rec['of_the_week'], $a_time[0], $a_time[1] , $this, 'scheduler_announce', $data );
				$irc->registerTimeScheduler( $rec['of_the_week'], $s_time[0]-1, $s_time[1] , $this, 'scheduler_ranbat_1h_ago', $data );
				$irc->registerTimeScheduler( $rec['of_the_week'], $s_time[0], $s_time[1]-30 , $this, 'scheduler_ranbat_entry', $data );
				$irc->registerTimeScheduler( $rec['of_the_week'], $s_time[0], $s_time[1] , $this, 'scheduler_ranbat_time', $data );
			}
		}
	}

	function getJoinChannelList(){
		return Array( $this->_convert( RANBAT_CHANNEL ) );
	}

	//scheduler

	function scheduler_announce(&$irc,&$args){
		if( $this->scene != enum_ranbat_scene::WAIT ){ return; }
		$str = $this->_template( RANBAT_MSG_SCHEDULER_ANNOUNCE,$args);
		$this->_notice($irc, $str, RANBAT_CHANNEL );
		$this->_twit( $str );
	}

	function scheduler_ranbat_1h_ago(&$irc,&$args){
		if( $this->scene != enum_ranbat_scene::WAIT ){ return; }
		$str = $this->_template( RANBAT_MSG_SCHEDULER_ENTRY_1H_AGO, $args);
		$this->_notice($irc, $str, RANBAT_CHANNEL );
		$this->_twit( $str );
	}
	
	function scheduler_ranbat_entry(&$irc,&$args){
		if( $this->scene != enum_ranbat_scene::WAIT ){
			$this->_notice($irc, $this->_template( RANBAT_MSG_SCHEDULER_ENTRY_START_ERROR, $args), RANBAT_CHANNEL );
			return;
		}
		$str = $this->_template( RANBAT_MSG_SCHEDULER_ENTRY_START, $args );
		$this->_notice($irc, $str, RANBAT_CHANNEL );
		$this->_twit( $str );

		$this->entry_list = Array();
		$this->f_list_draw = false;
		$this->game = new Tournament();
		$this->scene = enum_ranbat_scene::R_ENTRY;
		$this->log = new GameLog();
		$this->log->start( $this->conn, GameLog::GAME_TYPE_RANBAT, Array( 'title' => $args['title'], 'scheduler_id' => $args['id'] ) );
	}
	
	function scheduler_ranbat_time(&$irc,&$args){
		if( !( $this->scene == enum_ranbat_scene::R_ENTRY ) ){ return; }
		$this->_notice($irc, $this->_template( RANBAT_MSG_SCHEDULER_START_TIME, Array( 'title'=>$args['title'], 'user_cnt'=>  count($this->entry_list) ) ), RANBAT_CHANNEL );
	}
	

  // command

	/**
	 * entry_start 
	 * 
	 * @param mixed $irc 
	 * @param mixed $data 
	 * @access public
	 * @return void
	 */
	function entry_start(&$irc,&$data){
		if( !$this->checkChannel($data) ){return;}
    	
		if( $this->scene != enum_ranbat_scene::WAIT ){
			$this->_priv($irc, RANBAT_PRIV_NO_WAIT, $data->nick );
			return;
		}
		$this->_notice($irc, RANBAT_MSG_ENTRY_START, $data->channel, true ); 
		$this->_twit( RANBAT_MSG_ENTRY_START );
		
		$this->entry_list = Array();
		$this->f_list_draw = false;
		$this->game = new Tournament();
		$this->scene = enum_ranbat_scene::R_ENTRY;
		
		$this->log = new GameLog();
		$this->log->start( $this->conn, GameLog::GAME_TYPE_RANBAT, Array( 'title' => date("トーナメント(Y m/d H:i:s開始)") ) );
	}
  
	function kohaku(&$irc,&$data){
		if( !$this->checkChannel($data) ){return;}
	
		if( $this->scene != enum_ranbat_scene::WAIT ){
			$this->_priv($irc, RANBAT_PRIV_NO_WAIT, $data->nick );
			return;
		}

		$this->_notice($irc, KOHAKU_MSG_ENTRY_START, $data->channel, true );
		$this->_twit( KOHAKU_MSG_ENTRY_START );
		
		$this->entry_list = Array();
		$this->f_list_draw = false;
		$this->game = new Kohaku();
		$this->scene = enum_ranbat_scene::K_ENTRY;

		$this->log = new GameLog();
		$this->log->start( $this->conn, GameLog::GAME_TYPE_KOHAKU, Array( 'title' => date("紅白戦(Y m/d H:i:s開始)") ) );
	}

	function entry(&$irc,&$data){
		if( !$this->checkChannel($data) ){return;}
		if( !( $this->scene & enum_ranbat_scene::ENTRY ) ){
			return;
		}
		$text = $this->_command( $this->_2utf8($data->message), Array( RANBAT_ENTRY_CMD.' ', RANBAT_ENTRY_CMD_S.' ' ));
	
		//if($text[0] == 'STONP'){$text[0]='ゴワスさん';};
	
		if( isset( $this->entry_list[$text[0]] ) ){
			if( $this->entry_list[$text[0]]['char'] == $text[1] ){
				$this->_priv($irc, RANBAT_PRIV_ENTRY_ERROR, $data->nick);
				return;
			}
			$this->entry_list[ $text[0] ] = Array( 'name'=>$text[0],'char'=>$text[1] );
			$this->_notice($irc, $this->_template( RANBAT_PRIV_ENTRY_UPDATE, $this->entry_list[ $text[0] ] ),$data->channel, true );
		}else{
			$this->entry_list[ $text[0] ] = Array( 'name'=>$text[0],'char'=>$text[1] );
    		$this->_notice($irc, $this->_template( RANBAT_PRIV_ENTRY,  $this->entry_list[ $text[0] ] ),$data->channel, true);
    	}
		$this->f_list_draw = false;
	}
 
	function entry_gws(&$irc,&$data){
		$data->message = $this->_convert( str_replace( RANBAT_ENTRY_ALIAS_CMD, RANBAT_ENTRY_CMD.' ' , $this->_2utf8( $data->message ) ) );
		return $this->entry($irc,$data);
	}

	function cancel(&$irc,&$data){
		if( !$this->checkChannel($data) ){return;}
		if( !( $this->scene & enum_ranbat_scene::ENTRY ) ){
			return;
		}
		$text = $this->_command( $this->_2utf8($data->message), Array( RANBAT_CANCEL_CMD.' ', RANBAT_CANCEL_CMD_S.' ' ));
		if( isset( $this->entry_list[$text[0]] ) ){
			unset($this->entry_list[$text[0]]);
			$this->f_list_draw = false;
    		$this->_notice($irc, RANBAT_PRIV_CANCEL, $data->channel, true);
	    }else{
			$this->_priv($irc, RANBAT_PRIV_CANCEL_ERROR, $data->nick );
		}
	}
  
	function draw_list(&$irc,&$data){
		if( !$this->checkChannel($data) ){return;}
		if( $this->scene & enum_ranbat_scene::ENTRY ){
			if(!$this->f_list_draw){
				$entrys = Array();
				foreach(  $this->entry_list as $entry ){
					$entrys[] = $entry['name'].'('.$entry['char'].')';
				}
				$str = $this->_sep_longtext( count($entrys).'人:'.join(',',$entrys ) );
				$this->_notice( $irc, $str, $data->channel );
				$this->f_list_draw = true;
			}
		}else if( $this->scene & enum_ranbat_scene::OPEN ){
			$str = $this->game->getTableString('long');
			$this->_notice( $irc, $str, $data->channel );
		}
	}

	function start(&$irc,&$data){
		if( !$this->checkChannel($data) ){return;}
		if( !( $this->scene & enum_ranbat_scene::ENTRY ) ){
			$this->_priv($irc, RANBAT_PRIV_START_ERROR, $data->nick );
			return;
		}
		if( count( $this->entry_list ) <= 3 ){
			$this->_notice($irc, RANBAT_MSG_START_ERROR, $data->channel, true );
			$this->scene = enum_ranbat_scene::WAIT;
			$this->log->end();
			unset($this->log);
			return;
		}

		$this->scene += enum_ranbat_scene::OPEN - enum_ranbat_scene::ENTRY;
		$this->_notice($irc, $this->_template( RANBAT_MSG_START_LOT, Array('user_cnt' => count($this->entry_list) ) ), $data->channel, true );
		foreach( $this->entry_list as $entry ){
    		$this->game->addPlayer( $entry );
		}
		$str = $this->game->start();

		$str = 'log:'.SYSTEM_SITE_URI.GAMELOG_PATH.$this->log->game_id."\n".$str;

		$this->_notice($irc, $str, $data->channel, true );
		$this->_backup();

		$irc->setTopic( $data->channel, $this->_template( KAKUGE_TOPIC_OPEN, array( 'url' => SYSTEM_SITE_URI.GAMELOG_PATH.$this->log->game_id ) ) );
	}

	function win(&$irc,&$data){
    	if( !$this->checkChannel($data) ){return;}
		if( ! ( $this->scene & enum_ranbat_scene::OPEN ) ){
			return;
		}
		$args = $this->_command( $this->_2utf8($data->message), Array( RANBAT_WIN_CMD.' ', RANBAT_WIN_CMD_S.' ' ));
		$text = $args[0];
		$ret = $this->game->win($text);

		$this->_notice($irc, $ret, $data->channel , true);
		if( $this->game->checkEnd() ){
			$str = $this->game->getEndMessage();
			$this->scene = enum_ranbat_scene::WAIT;
			$this->_notice($irc, $str, $data->channel, true );
			$this->_twit( $str );
			$irc->setTopic( $data->channel, BASE_TOPIC );
			$this->log->end();
			unset($this->log);
			return;
		}
		$this->_backup();
	}
	
	function leader(&$irc,&$data){
		if( !$this->checkChannel($data) ){return;}	
		if( ! ( $this->scene & enum_ranbat_scene::K_OPEN ) ){
			return;
		}
		$args = $this->_command( $this->_2utf8($data->message), Array( KOHAKU_LEADER_CMD.' ', KOHAKU_LEADER_CMD_S.' ' ));
		if( count($args) >= 2  ){
			$str = $this->game->setLeader($args[0],$args[1],isset($args[2])?$args[2]:$data->nick);
			$this->_notice( $irc, $str, $data->channel, true );
		}else{ return; }
		$this->_backup();
	 }
	 
	 function select(&$irc,&$data){
	 	if( !$this->checkChannel($data) ){return;}
		if( ! ( $this->scene & enum_ranbat_scene::K_OPEN ) ){
			return;
		}
		$args = $this->_command( $this->_2utf8($data->message), Array( KOHAKU_SELECT_CMD.' ', KOHAKU_SELECT_CMD_S.' ' ));
		if( count($args) >= 1  ){
			$str = $this->game->setPlayer($args[0],$data->nick);
			$this->_notice( $irc, $str, $data->channel, true );
		}else{ return; }
		$this->_backup();
 	} 
	
	function add(&$irc, &$data){
		if( !$this->checkChannel($data) ){return;}
		if( ! ( $this->scene & enum_ranbat_scene::K_OPEN ) ){
			return;
		}
		$args = $this->_command( $this->_2utf8($data->message), KOHAKU_ADD_CMD.' ' );
		if( count($args) >= 2  ){
			$str = $this->game->addPlayer( Array( 'name' => $args[0], 'char' => $args[1], 'team' => $args[2]  ) );
			$this->_notice( $irc, $str, $data->channel, true );
		}else{ return; }
		$this->_backup();
	}

  // system
  
  function gowasu(&$irc, &$data){
	if( !$this->checkChannel($data) ){return;}
	$this->_notice( $irc, 'ゴワスさん...', $data->channel );
  }
  
  function gosama(&$irc, &$data){
    //if( !$this->checkChannel($data) ){return;}
	$this->_notice( $irc, '┌┤´д｀├┐<ヨバレタ', $data->channel );
  }
 
  function dosukoi(&$irc, &$data){
    if( !$this->checkChannel($data) ){return;}
	$text = str_replace( '!dosukoi ', '', $this->_2utf8($data->message) );
	if(strlen($text)){
		$rs = &$this->conn->Execute('select cnt from dosukoi where name = '.$this->conn->qstr($text).' ');
		if(!$rs){ d($this->conn->ErrorMsg());}
		else{
			if( $rs->RecordCount() ){
				$row = $rs->FetchRow();
				$cnt = $row['cnt'];
			}else{
				$cnt = 0;
			}
		}
		$cnt++;
		$this->_notice( $irc, $text.'さんがドスコイされました！ 現在の累計ドスコイ('.$cnt.'ドスコイ)', $data->channel);

		$ret = &$this->conn->Replace('dosukoi',
			Array( 'name' => $text, 'cnt' => $cnt ),
			'name', true);
		if(!$rs){ d($this->conn->ErrorMsg());}
	}
  }

  function dosukoi_rank(&$irc, &$data){
    if( !$this->checkChannel($data) ){return;}
    $rs = &$this->conn->Execute('select * from dosukoi ORDER BY cnt desc ');
	if(!$rs){ d($this->conn->ErrorMsg());}
	else{
		if( $rs->RecordCount() ){
			$row = $rs->FetchRow();
			$this->_notice( $irc, '現在のトップドスコイは'.$row['name'].'さんです。('.$row['cnt'].'ドスコイ)',$data->channel );
		}
	}
  }

  function ssf4(&$irc, &$data){
  	if( !$this->checkChannel($data) ){return;}
	$text = str_replace( KAKUGE_SSF4_CMD, '', $this->_2utf8($data->message) );
	if( strlen( $text ) ){ $text = '『'.$text.'』'; }
	$this->_twit( $this->_template( KAKUGE_TWIT_SSF4, array( 'nick' => $data->nick, 'option'=>$text) ) );
  }

  private function checkChannel( &$data ){
    return (RANBAT_CHANNEL == $this->_2utf8($data->channel )  );
  }

	///input log保存用のオーバーライド
	function _command( $str,$cmd ){
		$log_str = "";
		if( is_array( $cmd ) ){
			$log_str .= $cmd[0];
		}else{
			$log_str .= $cmd;
		}
		$ret = parent::_command( $str, $cmd );
		if( !is_null($this->log) ){
			$this->log->input( $log_str.join(' ', $ret ) );
		}
		return $ret;
	}

	///output log保存用のオーバーライド
	function _notice( &$irc, $str, $channel = CHANNEL, $log = false ){
		if( $log && !is_null($this->log) ){
			$this->log->output( $str );
		}
		parent::_notice( $irc, $str, $channel );
	}

	function _backup(){
		$this->log->update( serialize( $this->game ) );
	}
	
	function _twit( $str ){
		$options = Array( 'id' => KKG_TWITTER_ID, 'pass' => KKG_TWITTER_PASS );
		parent::_twit( $str, $options );
	}
}

class enum_ranbat_scene{
	const WAIT		= 0;
	const ENTRY		= 2;
	const R_ENTRY	= 2;
	const K_ENTRY	= 3;
	const OPEN		= 4;
	const R_OPEN	= 4;
	const K_OPEN	= 5;

	const RANBAT	= 0;
	const KOHAKU	= 1;
}

interface Game{ 
	const WIN_RETURN_ERROR = -1;
	const WIN_RETURN_ADD	= 0;
	const WIN_RETURN_NEXT	= 1;
	const WIN_RETURN_UPDATE = 2;
	const WIN_RETURN_NO_CHANGE = 3;

	function addPlayer( $entry );
	function start();
	function win( $name );
	function getTableString( $type = 'short' );
	function checkEnd();
	function getEndMessage();
	
	//function startLoggin( &$conn, $option); 
}

class Tournament extends common_base implements Game{
	var $player = null;
	var $table = null;
	var $winner = null;

	var $current = 0;

	var $battle_cnt = 0; //残りの試合数
	var $timer = null;


	var $game_type;
	var $game_title;
	var $scheduler_id;
	var $conn;
	function __construct($args){
		$this->player = Array();
		$this->table = Array();
		$this->winner = Array();
		$this->timer = Array();
		$this->current = 0;
	}

	function addPlayer( $entry ){
		$this->player[$entry[name]] = new Player( $entry ); 
	}

	function lot2(){
		if( !$this->current ){
			$this->battle_cnt = 0;
			$l = array_keys($this->player); 
			shuffle($l);
			$stack =Array();
			$this->table[$this->current] = Array();
			while($l){
				$n = ceil( count($l)/2 );
				list($l,$l2) = array_chunk($l,$n);
				if( $n <= 2 ){
					array_push( $this->table[$this->current], $l, $l2 );
					$l = array_pop($stack);
				}else{
					array_push($stack,$l2);
				}
			}
			foreach( $this->table[$this->current] as $match){ if(count($match) == 2 ){$this->battle_cnt++;} }
		}
	}

	function start(){
		$this->lot();
		return $this->getTableString('long');
	}

	private function lot(){
		if( !$this->current ){
			$this->battle_cnt = 0;
			$l = array_keys($this->player); 
    		shuffle($l);
			$this->table[$this->current] = Array();
	  
			$lengh = count($l);
	  
			for($s=1;$lengh>$s<<1;$s=$s<<1);

			$order = Array();
			for($i=0;$i<$s;$i+=2){
				$this->table[$this->current][$i] = Array(array_shift($l),array_shift($l));
				$this->battle_cnt++;
			}

			for($i=0;$i<$s>>1;$i++){
				$n=1;
				for($j=0;pow(2,$j)<=$i;$j++){
					if($i&pow(2,$j)){ $n += $s>>($j+1); } 
				}   
				$order[] = $n; 
			}

			foreach( $order as $i => $n ){
				$cnt = count($l);
				if( ! $cnt ){ $this->table[$this->current][$n] = Array(); }
				else if( $cnt > ($s>>1)-$i ){
					$this->table[$this->current][$n] = Array(array_shift($l),array_shift($l));
					$this->battle_cnt++;
				}else{ $this->table[$this->current][$n] = Array( array_shift($l) ); }
	 		}

			ksort($this->table[$this->current]);
		}
	}

	function win($name){
		if( !isset($this->player[$name]) ){ return RANBAT_PRIV_CANCEL_ERROR;}

    	if( in_array( $name, $this->winner ) ){ 
			return RANBAT_PRIV_WINNER_ERROR;
		}
		if( isset($this->timer[$name]) && $this->timer[$name] + RANBAT_DUPLICATION_COMMAND_THROUGH_TIME > time() ){
			return RANBAT_PRIV_WINNER_TIME_ERROR;
		}
		$this->timer[$name] = time();
		foreach( $this->table[$this->current] as $key => $match ){
			if( count( $match ) >= 2 && in_array( $name, $match ) ){
				$ret = $this->_template( RANBAT_PRIV_WINNER_CHECK, Array( 'winner' => $name ) );
				if( isset($this->winner[$key])  ){
					$this->winner[$key] = $name;
					$ret = RANBAT_PRIV_WINNER_UPDATE;
				}else{
					$this->winner[$key] = $name;
					$this->battle_cnt--;
					if($this->battle_cnt <= 0 ){
						$ret .= "\n".$this->next();
					}
				}
				return $ret;
			}
		}
		return "";
	}

	private function next(){
		if( count($this->table[$this->current]) == 1  ){
			foreach( $this->table[$this->current][0] as $p ){
				if( $p != $this->winner[0]  ){
					return "";
				}
			}
		}

		if( $this->battle_cnt ){ return ""; }
		$this->table[$this->current+1] = Array();
		foreach( $this->table[$this->current] as $key => $match ){
			if( isset($this->winner[$key]) ){
				$this->table[$this->current+1][ floor($key/2) ][] = $this->winner[$key];
			}else if( isset($match[0]) ){
				$this->table[$this->current+1][ floor($key/2) ][] = $match[0];
			}
		}
		$this->winner = Array();
		$this->current++;
		$this->battle_cnt = 0;
		foreach( $this->table[$this->current] as $match){ if(count($match) == 2 ){$this->battle_cnt++;} }
		return $this->getTableString();
	}

	function checkEnd(){
		return count($this->table[$this->current]) == 1 && count($this->winner) == 1;
	}

	function getEndMessage(){
		foreach( $this->table[$this->current][0] as $p ){
			if( $p != $this->winner[0]  ){
  				return $this->_template( RANBAT_MSG_CHAMP,Array( 'chanp' => $this->winner[0], '2nd' => $p ) );
			}
		}
	}

	function getTableString( $type = 'short' ){
		if( !count( $this->table ) ){
			$this->lot();
		}
		$array = Array();
	
		$cnt = count($this->table[$this->current]);
		$str = '■';
		if( $cnt == 1  ){
			$str .= '決勝';
		}else if( $cnt == 2 ){
			$str .= '準決勝';
		}else{
			$str .= ($this->current+1).'回戦';
		}
		$array[] = $str;
		$i=0;
		foreach( $this->table[$this->current] as $key => $match ){
			$str = ++$i.':';
			switch(count($match)){
				case 2:
					$str .= $match[0].' vs '.$match[1];
					if( isset( $this->winner[ $key ] ) ){
						$str .= '(win:'.$this->winner[$key].')';
					}
					break;
				case 1:
					$str .= 'seed:'.$match[0];
					break;
				case 0:
					$str .= ' ';
					break;
			}
			$array[] = $str;
		}
		return join("\n",$array);
	}
}

class Kohaku extends common_base implements Game{
    var $player = null;
	var $team = null;
	var $battle = null;
	var $battle_cnt = 0; //何試合目か

	var $scene = null;
	var $wait_leader = null;

	var $TEAM_LIST = Array( 'B'=>'A', 'A'=> 'B' );
	var $winner = null;

	var $num_list;

	const PLAYER_STATUS_NONE = 0;
	const PLAYER_STATUS_SELECT = 1;
	const PLAYER_STATUS_LIFE = 1;
	const PLAYER_STATUS_DEFEAT = 2;

	const KOHAKU_SCENE_START = -1;
	const KOHAKU_SCENE_LEADER = 0;
	const KOHAKU_SCENE_MEMBER_SELECT = 1;
	const KOHAKU_SCENE_SELECT = 2;
	const KOHAKU_SCENE_BATTLE = 3;
	const KOHAKU_SCENE_URAGIRI = 4;
	const KOHAKU_SCENE_URAGIRI_SELECT = 5;
	const KOHAKU_SCENE_END = 6;

	function __construct(){
		$this->team = Array();
		$this->battle = Array();
		$this->num_list = null;
		$this->battle_cnt = 0;
		$this->scene = self::KOHAKU_SCENE_START;
		$this->wait_leader = null;
	}

    function addPlayer( $entry ){
		$str = "";
		switch( $this->scene ){
		case self::KOHAKU_SCENE_START: 
			$entry['status'] = self::PLAYER_STATUS_NONE;
			$this->player[$entry[name]] = new player( $entry );
			break;
		case self::KOHAKU_SCENE_MEMBER_SELECT:
			if( $this->getPlayerNum( $entry['name']) != -1 ){	
				return $this->_template( KOHAKU_MSG_ADD_ERROR, $entry );
			}
			$entry['status'] = self::PLAYER_STATUS_NONE;
			$this->player[$entry[name]] = new player( $entry );
			$str .= $this->_template( KOHAKU_MSG_ADD_MEMBER, Array( 'name'=> $entry['name'], 'nick'=> $entry['nick'] ) );
			break;
		case self::KOHAKU_SCENE_SELECT:
		case self::KOHAKU_SCENE_BATTLE:
			if( $this->getPlayerNum( $entry['name']) != -1 ){	
				return $this->_template( KOHAKU_MSG_ADD_ERROR, $entry );
			}

			if( !strlen($entry['team']) || array_search( $entry['team'], $this->TEAM_LIST  ) === FALSE  ){
			//追加先チームの指定が無いか、不正な場合。
				$t = (count( $this->player ) % 2) ? 'B':'A';
			}else{
				$t = $entry['team'];
			}
			$num = $entry[name];
			$entry['status'] = self::PLAYER_STATUS_SELECT;
			$this->player[$num] = new player( $entry );
			//チームへの追加処理
			$this->team[$t]['member'][] = $num;
			if( $t == 'A' ){
				//BのリーダーのHPを追加。(死んでいた場合は0以上なら復帰)
				$this->team['B']['leader']['hp']++;
				if( $this->player[ $this->team['B']['leader']['num'] ]->status == self::PLAYER_STATUS_DEFEAT && 
							$this->team['B']['leader']['hp'] >= 1 ){
					$this->player[ $this->team['B']['leader']['num'] ]->status == self::PLAYER_STATUS_LIFE;
				}
				$str .= $this->_template( KOHAKU_MSG_ADD_BATTLE_A, Array( 'name'=> $entry['name'] ) )."\n";
			}else{
				//BのリーダーのHPを減らす。(既に死んでいる場合も-1に)
				$this->team['B']['leader']['hp']--;
				if( $this->player[ $this->team['B']['leader']['num'] ]->status == self::PLAYER_STATUS_LIFE &&
							$this->team['B']['leader']['hp'] < 1 ){
					$this->player[ $this->team['B']['leader']['num'] ]->status == self::PLAYER_STATUS_DEFEAT;
				}
				$str .= $this->_template( KOHAKU_MSG_ADD_BATTLE_B, Array( 'name'=> $entry['name'] ) )."\n";
			}

			//$str .= $this->_template( KOHAKU_MSG_ADD_BATTLE, Array( 't_name'=> $t ,'name'=> $entry['name'], 'nick'=> $entry['nick'] ) );
			break;
		}
		return $str;
	}

    function start(){
		$str = "";
	
		//一覧の表示、リーダー決定のアナウンス
		$str .= KOHAKU_MSG_PLACE_LEADER."\n";
		$str .= $this->_sep_longtext( 'プレイヤー一覧：'.join(',',$this->getRemainderList()));
	
		$this->scene = self::KOHAKU_SCENE_LEADER;
		return $str;
	}

	function setLeader( $t, $name, $nick ){
		//a,bのリーダーをセット
		if( $this->scene != self::KOHAKU_SCENE_LEADER || array_search($t, $this->TEAM_LIST  ) === FALSE ){
			return "";
		}
		if( is_numeric( $name ) && isset( $this->num_list[$name] ) ){
			$num = $this->num_list[ $name ];
			$name = $this->player[$num]->name;
		}else{
			$num = $this->getPlayerNum($name);
		}
		if( $num == -1 ){
			$str = $this->_template( KOHAKU_MSG_NO_ENTRY, Array( 'name' => $name ) );
		}else if( $this->player[$num]->status != self::PLAYER_STATUS_NONE ){
			//選択済みのリーダー
			if( $this->team[$t]['leader']['num'] == $num && $this->team[$t]['leader']['nick'] != $nick ){
				$this->team[$t]['leader']['nick'] = $nick;
				$str = $this->_template( KOHAKU_MSG_LEADER_NICK_CHANGE, Array( 't_name' => $t, 'name' => $name, 'nick' => $nick ) );
			}else{
				$str = $this->_template( KOHAKU_MSG_MEMBER_SELECT_ERROR, Array( 'name' => $name ) );
			}
		}else{
			if( isset($this->team[$t]['leader'] ) ){
				$this->player[$this->team[$t]['leader']['num']]->status = self::PLAYER_STATUS_NONE;
				$this->team[$t]['member'] = Array();
				$str = $this->_template( KOHAKU_MSG_LEADER_CHANGE, Array( 't_name' => $t, 'name' => $name, 'nick' => $nick ) );
			}else{
				$str = $this->_template( KOHAKU_MSG_LEADER_CHECK, Array( 't_name' => $t, 'name' => $name, 'nick' => $nick ) );
			}
			$this->team[$t]['leader'] = Array( 'num' => $num  , 'nick' => $nick, 'hp' => 1 );
			$this->player[$num]->status = self::PLAYER_STATUS_SELECT; 
			$this->team[$t]['member'][] = $num;
			$str .= "\n";
			if( isset($this->team['A']['leader']) && isset($this->team['B']['leader']) ){
				$this->scene = self::KOHAKU_SCENE_MEMBER_SELECT;
				$this->wait_leader = 'A';
				$str .= $this->_template(KOHAKU_MSG_SELECT_START,Array( 't_name' => $this->wait_leader ) )."\n";
				$str .= $this->_sep_longtext( '残りプレイヤー:'.join(',',$this->getRemainderList()));
			}else{
				$str .= $this->_sep_longtext( 'プレイヤー一覧：'.join(',',$this->getRemainderList()));
			}
		}
		return $str;
	}

	function setPlayer($name,$nick){
		if( $nick != $this->team[$this->wait_leader]['leader']['nick'] && array_search( $nick,Array('swirhen','tyoro')  ) === FALSE ){
			return;
		}
		if( is_numeric( $name ) && isset( $this->num_list[$name] ) ){
			$num = $this->num_list[ $name ];
			$name = $this->player[ $num ]->name;
		}else{
			$num = $this->getPlayerNum($name);
		}
		if( $num == -1 ){
			return $this->_template( KOHAKU_MSG_NO_ENTRY, Array( 'name' => $name ) );
		}
		switch( $this->scene ){
			case self::KOHAKU_SCENE_MEMBER_SELECT:
				if( $this->player[$num]->status != self::PLAYER_STATUS_NONE ){
					$str = $this->_template( KOHAKU_MSG_MEMBER_SELECT_ERROR, Array( 'name' => $name ) );
				}else{
					$str = "";
					$this->player[$num]->status = self::PLAYER_STATUS_SELECT;
					$this->team[$this->wait_leader]['member'][] = $num;

					$list = $this->getRemainderList();

					$this->wait_leader = $this->TEAM_LIST[ $this->wait_leader ]; 
					if( count( $list ) > 1 ){
						$str .= $this->_template(KOHAKU_MSG_MEMBER_SELECT,Array( 't_name' => $this->wait_leader ) )."\n";
						$str .= $this->_sep_longtext( '残りプレイヤー:'.join(',',$list) )."\n";
					}else{
						if( count( $list ) == 1  ){
							foreach( $list as $last_num => $last_name ){
								$this->player[$last_num]->status = self::PLAYER_STATUS_SELECT;
								$this->team[$this->wait_leader]['member'][] = $last_num;
								$str .= $this->_template( KOHAKU_MSG_MEMBER_SELECT_LAST, Array( 't_name' => $this->wait_leader, 'name' => $last_name ) );
								if( count( $this->player ) % 2 == 1 ){
									$this->team['B']['leader']['hp']=2;
									$str .= KOHAKU_MSG_MEMBER_SELECT_LAST_HP;
								}
								break;
							}
						}
						$this->scene = self::KOHAKU_SCENE_SELECT;
						$this->wait_leader = 'A';
							
						$str .= KOHAKU_MSG_START."\n";
						$str .= $this->getTableString('long')."\n";
						$str .= $this->_template( KOHAKU_MSG_SELECT, Array( 't_name' => $this->wait_leader ) )."\n";
						$str .= $this->getTableString();
					}
				}
				break;
			case self::KOHAKU_SCENE_SELECT:
				if( $this->player[$num]->status != self::PLAYER_STATUS_LIFE ){
					return $this->_template( KOHAKU_MSG_SELECT_ERROR, Array( 'name' => $name ) )."\n";
				}else if( array_search( $num, $this->team[ $this->wait_leader ]['member']) === FALSE ){
					return $this->_template( KOHAKU_MSG_SELECT_ERROR2, Array( 'name' => $name ) )."\n";	
				}
				$str = "";
				$this->battle[ $this->battle_cnt ][ $this->wait_leader ] = $num;

				if( $this->battle_cnt == 0 && !isset( $this->battle[0]['B'] ) ){
					//初回のA登録後、Bが登録されていない。
					$this->wait_leader = 'B';
					$str .= $this->_template( KOHAKU_MSG_SELECT, Array( 't_name' => $this->wait_leader ) )."\n";
					$str .= $this->getTableString();
				}else{
					//戦闘開始
					$str .= $this->getBattleString();
					$this->scene = self::KOHAKU_SCENE_BATTLE;
				}
				break;
			case self::KOHAKU_SCENE_URAGIRI_SELECT:
				if( $this->player[$num]->status != self::PLAYER_STATUS_LIFE ){	
					return $this->_template( KOHAKU_MSG_SELECT_ERROR, Array( 'name' => $name ) )."\n";
				}
				$str = "";
				$this->battle[ $this->battle_cnt ][ $this->wait_leader ] = $num;

				$str .= $this->getBattleString();
				$this->scene = self::KOHAKU_SCENE_URAGIRI;
				break;
		}
		return $str;
	}

    function win( $name ){
		if( $this->scene != self::KOHAKU_SCENE_BATTLE && $this->scene != self::KOHAKU_SCENE_URAGIRI ){
			return "";
		}
		
		if( array_search($name, $this->TEAM_LIST  ) !== FALSE ){
			$num = $this->battle[ $this->battle_cnt ][ $name ];	
			$name = $this->player[ $num ]->name;
		}else{
			$num = $this->getPlayerNum($name);
		}
		if( $num == -1 ){
			return $this->_template( KOHAKU_MSG_NO_ENTRY, Array( 'name' => $name ) );
		}else if( $this->battle[$this->battle_cnt]['A'] != $num && $this->battle[$this->battle_cnt]['B'] != $num  ){
			return "";
		}

		foreach( $this->TEAM_LIST as $t ){
			if( $this->battle[$this->battle_cnt][ $t ] == $num ){
				//勝者確認
				$str = $this->_template( RANBAT_PRIV_WINNER_CHECK, Array( 'winner' => $name ) )."\n";
				
				$this->battle_cnt++;
				$this->battle[$this->battle_cnt][$t] = $num;
				$this->wait_leader = $this->TEAM_LIST[$t];
				$l = $this->battle[$this->battle_cnt-1][$this->wait_leader];
				
				if( $this->scene == self::KOHAKU_SCENE_BATTLE ){
					if( $this->team[ $this->wait_leader ]['leader']['num'] == $l && $this->team[ $this->wait_leader ]['leader']['hp'] > 1 ){
						$this->team[ $this->wait_leader ]['leader']['hp']--;
					}else{
						$this->player[$l]->status = self::PLAYER_STATUS_DEFEAT;
					}
					$this->winner = $this->TEAM_LIST[$this->wait_leader];

					if( $this->getLifePlayerCount( $this->wait_leader ) > 0 ){
						//次に戦う人を選んでってアナウンス
						$str .= $this->_template( KOHAKU_MSG_SELECT, Array( 't_name' => $this->wait_leader ) )."\n";
						$str .= $this->getTableString();
						$this->scene = self::KOHAKU_SCENE_SELECT;
					}else{
						$str .= $this->_template( KOHAKU_MSG_TEAM_WIN, Array( 't_name' => $this->wait_leader , 'win_name' => $this->winner ) )."\n";
						//負けチームに人が残っていない。
						if( $this->getLifePlayerCount( $this->winner ) > 1 ){
							$this->scene = self::KOHAKU_SCENE_URAGIRI;
							$str .= $this->_template( KOHAKU_MSG_URAGIRI_START, Array( 't_name' => $this->winner ) );
				
							//次に裏切る人を選んでってアナウンス
							$str .= $this->_template( KOHAKU_MSG_SELECT, Array( 't_name' => $this->wait_leader ) )."\n";
							$str .= $this->getTableString('uragiri');
							$this->scene = self::KOHAKU_SCENE_URAGIRI_SELECT;
						}else{
							$this->scene = self::KOHAKU_SCENE_END;
						}
					}
				}else{//$this->scene == KOHAKU_SCENE_URAGIRI
					$this->player[$l]->status = self::PLAYER_STATUS_DEFEAT;
					$cnt = $this->getLifePlayerCount( );
				
					if( $cnt <= 1 ){
						$this->scene = self::KOHAKU_SCENE_END;
					}else if( $cnt == 2 ){
						$this->getTableString('uragiri');
						$this->battle[$this->battle_cnt][ $this->wait_leader ] = $this->num_list[1];
						$str .= KOHAKU_MSG_URAGIRI_LAST_PLAYER;
						$str .= $this->getBattleString();
					}else{
						$str .= $this->_template( KOHAKU_MSG_SELECT, Array( 't_name' => $this->wait_leader ) )."\n";
					$str .= $this->getTableString('uragiri');
						$this->scene = self::KOHAKU_SCENE_URAGIRI_SELECT;
					}
					break;
				}
				break;
			}
		}
		return $str;
	}
	
	private function getLifePlayerCount( $team = null ){
		if( !is_null($team) && array_search($team, $this->TEAM_LIST  ) === FALSE ){ return -1; }
		$cnt=0;
		foreach( $this->TEAM_LIST as $t ){
			if( isset($team) && $t != $team ){ continue; }
			foreach( $this->team[ $t ]['member'] as $m ){
			 	if( $this->player[$m]->status == self::PLAYER_STATUS_LIFE ){
					$cnt++;
				}
			}
		}
		return $cnt;
	}

    function getTableString( $type = 'short' ){
		//生存者一覧
		$str = "";
		switch( $type ){
			case 'long':
				foreach( $this->TEAM_LIST as $t ){
					$str .= "○".$t."チーム\n";
					foreach( $this->team[$t]['member'] as $m ){
						$suffix = "";

						if( $this->player[$m]->status == self::PLAYER_STATUS_LIFE ){
							$str .= '　';
						}else{
							$str .= '×';
						}

						if( $m == $this->team[$t]['leader']['num'] ){
							$str .= '☆';
							if( $this->team[$t]['leader']['hp'] > 1 ){
								$suffix = '(HP:'.$this->team[$t]['leader']['hp'].')';
							}
						}else{
							$str .= '　';
						}
						$str .= $this->player[$m]->name.'【'.$this->player[$m]->char.'】'.$suffix."\n";
					}
					$str .= "-----------------------------\n";
				}
				break;
			case 'short':
			default:
				$t = $this->wait_leader;
				$str .= "○".$t."チーム:";
				$l = Array();
				$d = Array();
				$i = 1;
				$this->num_list = Array();
				foreach( $this->team[$t]['member'] as $m ){
					$prefix = "";
					$suffix = "";
					if( $m == $this->team[$t]['leader']['num'] ){
						$prefix = '☆';
						if( $this->team[$t]['leader']['hp'] > 1 ){
							$suffix = '('.$this->team[$t]['leader']['hp'].')';
						}
					}
					if( $this->player[$m]->status == self::PLAYER_STATUS_LIFE ){
						$this->num_list[$i] = $m;
						$prefix = $i++.'.'.$prefix;
					}
					$s = $prefix.$this->player[$m]->name.'【'.$this->player[$m]->char.'】'.$suffix;
	
					if( $this->player[$m]->status == self::PLAYER_STATUS_LIFE ){
						$l[] = $s;
					}else{
						$d[] = $s;
					}
				}
				$str .= join( '、', $l );
				if( count($d) > 0 ){
					$str .= ' / ×('.join('、',$d ).")";
				}
				$str .= "\n";
				break;
			case 'uragiri':
				$l = Array();
				$this->num_list = Array();
				$i = 1;
				foreach( $this->TEAM_LIST as $t ){
					foreach( $this->team[$t]['member'] as $m ){
						$prefix = "";
						if( $this->player[$m]->status == self::PLAYER_STATUS_LIFE && $this->battle[ $this->battle_cnt ][ $this->TEAM_LIST[ $this->wait_leader ] ] != $m  ){
							$this->num_list[$i] = $m;
							$l[] = $i++.'.'.$this->player[$m]->name.'【'.$this->player[$m]->char.'】';
						}
					}
				}	
				$str = join( '、', $l );
				break;
		}
		return $str;
	}

	function getBattleString(){
		$str = $this->_template( KOHAKU_MSG_SELECTED,
			Array( 'n' => ($this->battle_cnt+1),
					'a_name' => $this->player[$this->battle[$this->battle_cnt]['A']]->name,
					'b_name' => $this->player[$this->battle[$this->battle_cnt]['B']]->name,
					's_name' => $this->player[$this->battle[$this->battle_cnt][$this->TEAM_LIST[$this->wait_leader]]]->name
				)
			);
		return $str;
	}

    function checkEnd(){
		return $this->scene == self::KOHAKU_SCENE_END;
	}
    function getEndMessage(){
		return KOHAKU_MSG_GAMESET;
	}

	private function getRemainderList(){
		$list = Array();
		$i=1;
		$this->num_list = Array();
		foreach( $this->player as $k => $p ){
			if( $p->status == self::PLAYER_STATUS_NONE  ){
				$this->num_list[$i] = $k;
				$list[$k] = $i++.'.'.$p->name.'【'.$p->char.'】';
			}
		}
		return $list;
	}
	
	private function getPlayerNum($name){
		foreach( $this->player as $k => $p ){
			if( $p->name == $name ){ return $k; }
		}
		return -1;
	}
}

class Player{
	var $name;
	var $char;
	var $suffix;

	var $rank;
	var $point;

	var $status;

	function __construct( $param ){
		foreach( $param as $k => $p ){
			switch( $k ){
				case 'name':
				case 'char':
				case 'suffix':
				case 'rank':
					case 'point':
				case 'status':
					$this->$k = $p;
					break;
			}
		}
	}

	function getString(){
		return $this->name.'('.$this->char.')';
	}

	function getFullString(){
		return $this->name.'('.$this->char.')';
	}
}

class GameLog{
	var $conn;
	var $game_id;
	var $game_type;

	const GAME_TYPE_RANBAT = 1;
	const GAME_TYPE_KOHAKU = 2;

	const GAME_LOG_TYPE_INPUT = 1;
	const GAME_LOG_TYPE_OUTPUT = 2;

	const GAME_STATUS_ENTRY = 0;
	const GAME_STATUS_PLAY = 1;
	const GAME_STATUS_END = 2;
	const GAME_STATUS_STOP = 3;

	function start( &$conn, $type, $option ){
		$this->conn = &$conn;
		$cols = Array( 'title', 'scheduler_id' );
		$record['type'] = $type;
		$record['created'] = time();//'CURRENT_TIMESTAMP';
		foreach( $cols as $c ){
			if( isset( $option[$c] ) ){	$record[$c] = $option[$c];	} 
		}
		$insertSQL = $this->conn->AutoExecute( 'games', $record, 'INSERT');
		$this->game_type = $type;
		$this->game_id = $this->conn->Insert_ID();
	}

	function end( $state = self::GAME_STATUS_END ){
		if( !is_null( $this->game_id ) ){
			$record['status']	= $state;
			$record['data'] = '';
			$this->conn->AutoExecute( 'games', $record, 'UPDATE', 'id = '.$this->game_id );
		}
	}

	///入力のログを取る。
	function input( $str ){
		if( !is_null( $this->game_id ) ){
			$record['game_id'] = $this->game_id;
			$record['type'] = self::GAME_LOG_TYPE_INPUT;
			$record['log'] = $str;
			$this->conn->AutoExecute( 'game_logs', $record, 'INSERT');
		}
	}
	
	///出力をログに残す。
	function output( $str ){
		if( !is_null( $this->game_id ) ){
			$record['game_id'] = $this->game_id;
			$record['type'] = self::GAME_LOG_TYPE_OUTPUT;
			$record['log'] = $str;
			$this->conn->AutoExecute( 'game_logs', $record, 'INSERT');
		}
	}

	///復帰用にgameインスタンスをシリアライズ化したものを記憶する。
	function update( &$obj, $option = null ){
		if( !is_null( $this->game_id ) ){
			$record['id']		= $this->game_id;
			$record['data'] = $obj;
			if( !is_null( $option ) ){
				foreach( $option as $key => $o ){
					if( isset( $option[$key] ) ){ $record[$key] = $option[$key]; }
				}
			}
			$this->conn->AutoExecute( 'games', $record, 'UPDATE', 'id = '.$this->game_id );
		}
	}

}

?>
