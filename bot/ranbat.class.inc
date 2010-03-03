<?php

/**
 * ranbat aot 
 *
 **/

include_once "./conf/ranbat.conf.inc";

include_once "XML/RSS.php";
include_once "Services/Twitter.php";
include_once "include/simple_html_dom.php";

include_once "./bot/base.class.inc";




class tyobot_ranbat extends tyobot_base{

  var $scene = enum_ranbat_scene::WAIT;
  var $entry_list = Array();
  var $f_list_draw = false;
  
  var $game = null;
 
  var $conn = null;

  function __construct(){
    $this->conn = &ADONewConnection('mysql');
    $this->conn->PConnect(DATABASE_HOST,DATABASE_ID,DATABASE_PASS,DATABASE_NAME);
  }

  // command

  function entry_start(&$irc,&$data){
    if( !$this->checkChannel($data) ){return;}
    
	if( $this->scene != enum_ranbat_scene::WAIT ){
		$this->_priv($irc, RANBAT_PRIV_NO_WAIT, $data->nick );
		return;
	}
	$this->_notice($irc, RANBAT_MSG_ENTRY_START, $data->channel ); 
	$this->entry_list = Array();
	$this->f_list_draw = false;
    $this->game = new Tournament();
	$this->scene = enum_ranbat_scene::R_ENTRY;
  }
  
  function kohaku(&$irc,&$data){
	if( !$this->checkChannel($data) ){return;}
	
	if( $this->scene != enum_ranbat_scene::WAIT ){
	    $this->_priv($irc, RANBAT_PRIV_NO_WAIT, $data->nick );
		return;
	}

	$this->_notice($irc, KOHAKU_MSG_ENTRY_START, $data->channel );
	$this->entry_list = Array();
	$this->f_list_draw = false;
    $this->game = new Kohaku();
	$this->scene = enum_ranbat_scene::K_ENTRY;
  }

  function entry(&$irc,&$data){
    if( !$this->checkChannel($data) ){return;}
    if( !( $this->scene & enum_ranbat_scene::ENTRY ) ){
	  return;
	}
	$text = str_replace( Array( RANBAT_ENTRY_CMD.' ', RANBAT_ENTRY_CMD_S.' ' ), '', $this->_2utf8($data->message) );
	$text = explode( ' ', $text );
	
	if($text[0] == 'STONP'){$text[0]='ゴワスさん';};
	
	if( isset( $this->entry_list[$text[0]] ) ){
      if( $this->entry_list[$text[0]]['char'] == $text[1] ){
	    $this->_priv($irc, RANBAT_PRIV_ENTRY_ERROR, $data->nick);
		return;
	  }
	  $this->entry_list[ $text[0] ] = Array( 'name'=>$text[0],'char'=>$text[1] );
	  $this->_priv($irc, $this->_template( RANBAT_PRIV_ENTRY_UPDATE, $this->entry_list[ $text[0] ] ),$data->nick);
    }else{
	  $this->entry_list[ $text[0] ] = Array( 'name'=>$text[0],'char'=>$text[1] );
      $this->_priv($irc, $this->_template( RANBAT_PRIV_ENTRY,  $this->entry_list[ $text[0] ] ),$data->nick);
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
    $text = substr($this->_2utf8($data->message),strlen(RANBAT_CANCEL_CMD)+1);
    if( isset( $this->entry_list[$text] ) ){
      unset($this->entry_list[$text]);
	  $this->f_list_draw = false;
      $this->_priv($irc, RANBAT_PRIV_CANCEL, $data->nick);
    }else{
	  $this->_priv($irc, RANBAT_PRIV_CANCEL_ERROR, $data->nick );
	}
  }
  
  function draw_list(&$irc,&$data){
    if( !$this->checkChannel($data) ){return;}
    if( !( $this->scene & enum_ranbat_scene::ENTRY ) ){
	  return;
	}
	if(!$this->f_list_draw){
	  $entrys = Array();
	  foreach(  $this->entry_list as $entry ){
	    $entrys[] = $entry['name'].'('.$entry['char'].')';
	  }
	  $str = count($entrys).'人:'.join(',',$entrys );
	  $this->_notice( $irc, $str, $data->channel);
	  $this->f_list_draw = true;
	}
  }

  function start(&$irc,&$data){
    if( !$this->checkChannel($data) ){return;}
    if( !( $this->scene & enum_ranbat_scene::ENTRY ) ){
	  $this->_priv($irc, RANBAT_PRIV_START_ERROR, $data->nick );
	  return;
	}
	if( count( $this->entry_list ) <= 3 ){
	  $this->_notice($irc, RANBAT_MSG_START_ERROR, $data->channel );
	  $this->scene = enum_ranbat_scene::WAIT;
	  return;
	}

	$this->scene += enum_ranbat_scene::OPEN - enum_ranbat_scene::ENTRY;
	$this->_notice($irc, $this->_template( RANBAT_MSG_START_LOT, Array('user_cnt' => count($this->entry_list) ) ), $data->channel );
	foreach( $this->entry_list as $entry ){
      $this->game->addPlayer( $entry );
    }
	$str = $this->game->start();
    $this->_notice($irc, $str, $data->channel );
  }

  function win(&$irc,&$data){
    if( !$this->checkChannel($data) ){return;}
	if( ! ( $this->scene & enum_ranbat_scene::OPEN ) ){
	  return;
	}
	$text = str_replace( Array( RANBAT_WIN_CMD.' ', RANBAT_WIN_CMD_S.' ' ), '', $this->_2utf8($data->message) );
	$ret = $this->game->win($text);

	switch( $ret ){
		case Game::WIN_RETURN_ERROR:
			break;
		case Game::WIN_RETURN_ADD:
			$this->_priv($irc, $this->_template( RANBAT_PRIV_WINNER_CHECK, Array( 'winner' => $text ) ) , $data->nick );
			break;
		case Game::WIN_RETURN_UPDATE:
			$this->_priv($irc, RANBAT_PRIV_WINNER_UPDATE, $data->nick );
			break;
		case Game::WIN_RETURN_NO_CHANGE:
			$this->_priv($irc, RANBAT_PRIV_WINNER_ERROR, $data->nick );
			break;
		case Game::WIN_RETURN_NEXT:
			$this->_priv($irc, $this->_template( RANBAT_PRIV_WINNER_CHECK, Array( 'winner' => $text ) ) , $data->nick );
			if( $this->game->checkEnd() ){
				//end
				$str = $this->game->getEndMessage();
				$this->_notice($irc, $str, $data->channel );
				$this->scene = enum_ranbat_scene::WAIT;
			}else{
				$str = $this->game->next();
				$this->_notice($irc, $str, $data->channel );
			}
			break;
		}
	}
	
	function leader(&$irc,&$data){
		if( !$this->checkChannel($data) ){return;}	
		if( ! ( $this->scene & enum_ranbat_scene::K_OPEN ) ){
			return;
		}
		$text = str_replace( Array( KOHAKU_LEADER_CMD.' ', KOHAKU_LEADER_CMD_S.' ' ), '', $this->_2utf8($data->message) );
		d($text);
		$args = explode( ' ', $text );
		if( count($args) >= 2  ){
			$str = $this->game->setLeader($args[0],$args[1],isset($args[2])?$args[2]:$data->nick);
			$this->_notice( $irc, $str, $data->channel );
		}
	 }
	 
	 function select(&$irc,&$data){
	 	if( !$this->checkChannel($data) ){return;}
		if( ! ( $this->scene & enum_ranbat_scene::K_OPEN ) ){
			return;
		}
		$text = str_replace( Array( KOHAKU_SELECT_CMD.' ', KOHAKU_SELECT_CMD_S.' ' ), '', $this->_2utf8($data->message) );
		$args = explode( ' ', $text );
		if( count($args) >= 1  ){
			$str = $this->game->setPlayer($args[0],$data->nick);
			$this->_notice( $irc, $str, $data->channel );
		}
 	} 
  // system
  
  function getNextTime(){
    $t = time();
	$h = date('G',$t);
	$m = (int)date('i',$t);
	$s = (int)date('s',$t);
	$w = date('w',$t);

//	$w 
	
	//return 1000*1000;

  }
  
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

  function checkChannel( &$data ){
    return (RANBAT_CHANNEL == $this->_2utf8($data->channel )  );
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
	function next();
	function getTableString( $long = false );
	function checkEnd();
	function getEndMessage();
}

class Tournament extends common_base implements Game{
  var $player = null;
  var $table = null;
  var $winner = null;

  var $current = 0;

  var $battle_cnt = 0; //残りの試合数

  function __construct(){
    $this->player = Array();
	$this->table = Array();
	$this->winner = Array();
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
	return $this->getTableString(true);
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
		else if( $cnt > $s>>1-$i ){
			$this->table[$this->current][$n] = Array(array_shift($l),array_shift($l));
			$this->battle_cnt++;
		}else{ $this->table[$this->current][$n] = Array( array_shift($l) ); }
	  }

	  ksort($this->table[$this->current]);
	}
  }


  function win($name){
    if( !isset($this->player[$name]) ){ return Game::WIN_RETURN_ERROR; }
    if( in_array( $name, $this->winner ) ){ 
	  return Game::WIN_RETURN_NO_CHANGE;
	}
    foreach( $this->table[$this->current] as $key => $match ){
	  if( count( $match ) >= 2 && in_array( $name, $match ) ){
		$ret = Game::WIN_RETURN_ADD;
		if( isset($this->winner[$key])  ){
		  	$ret = Game::WIN_RETURN_UPDATE;
		}else{
		  $this->battle_cnt--;
		  if($this->battle_cnt <= 0 ){
		    $ret = Game::WIN_RETURN_NEXT;
		  }
		}
		$this->winner[$key] = $name;
		return $ret;
	  }
	}
	return Game::WIN_RETURN_ERROR;
  }

  function next(){
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

  function getTableString( $long = false ){
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

    foreach( $this->table[$this->current] as $match ){
	  $str = ++$i.':';
	  switch(count($match)){
	    case 2:
			$str .= $match[0].' vs '.$match[1];
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

  function drawList(&$irc,$channel = RANBAT_CHANNEL ){
    $str = Array();
    foreach( $this->player as $p ){
	  $str[] = $p->getFullString();
	}
	$this->_notice($irc,join("\n",$str),$channel);
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

	const PLAYER_STATUS_NONE = 0;
	const PLAYER_STATUS_SELECT = 1;
	const PLAYER_STATUS_LIFE = 1;
	const PLAYER_STATUS_DEFEAT = 2;

	const KOHAKU_SCENE_LEADER = 0;
	const KOHAKU_SCENE_MEMBER_SELECT = 1;
	const KOHAKU_SCENE_SELECT = 2;
	const KOHAKU_SCENE_BATTLE = 3;
	const KOHAKU_SCENE_URAGIRI = 4;
	const KOHAKU_SCENE_END = 5;

	function __construct(){
		$this->team = Array();
		$this->battle = Array();
		$this->battle_cnt = 0;
		$this->scene = null;
		$this->wait_leader = null;
	}

    function addPlayer( $entry ){
		$entry['status'] = self::PLAYER_STATUS_NONE;
		$this->player[$entry[name]] = new player( $entry );
	}

    function start(){
		$str = "";
	
		//一覧の表示、リーダー決定のアナウンス
		$str .= KOHAKU_MSG_PLACE_LEADER."\n";
		$str .= 'プレイヤー一覧：'.join(',',$this->getRemainderList());
	
		$this->scene = self::KOHAKU_SCENE_LEADER;
		return $str;
	}

	function setLeader( $t, $name, $nick ){
		//a,bのリーダーをセット
		if( $this->scene != self::KOHAKU_SCENE_LEADER || $t != 'A' && $t != 'B' ){
			return "";
		}
		$num = $this->getPlayerNum($name);
		if( $num == -1 ){
			$str = $this->_template( KOHAKU_MSG_NO_ENTRY, Array( 'name' => $name ) );
		}else if( $this->player[$num]->status != self::PLAYER_STATUS_NONE ){
			$str = $this->_template( KOHAKU_MSG_MEMBER_SELECT_ERROR, Array( 'name' => $name ) );
		}else{
			if( isset($this->team[$t]['leader'] ) ){
				$this->player[$this->team[$t]['leader']['num']]->status = self::PLAYER_STATUS_NONE;
				$this->team[$t]['member'] = Array();
				$str = $this->_template( KOHAKU_MSG_LEADER_CHANGE, Array( 't_name' => $t, 'name' => $name, 'nick' => $nick ) );
			}else{
				$str = $this->_template( KOHAKU_MSG_LEADER_CHECK, Array( 't_name' => $t, 'name' => $name, 'nick' => $nick ) );
			}
			$this->team[$t]['leader'] = Array( 'num' => $num  , 'nick' => $nick );
			$this->player[$num]->status = self::PLAYER_STATUS_SELECT; 
			$this->team[$t]['member'][] = $num;
			$str .= "\n";
			if( isset($this->team['A']['leader']) && isset($this->team['B']['leader']) ){
				$this->scene = self::KOHAKU_SCENE_MEMBER_SELECT;
				$this->wait_leader = 'A';
				$str .= $this->_template(KOHAKU_MSG_SELECT_START,Array( 't_name' => $this->wait_leader ) )."\n";
				$str .= '残りプレイヤー:'.join(',',$this->getRemainderList());
			}else{
				$str .= 'プレイヤー一覧：'.join(',',$this->getRemainderList());
			}
		}
		return $str;
	}

	function setPlayer($name,$nick){
		if( $nick != $this->team[$this->wait_leader]['leader']['nick'] ){
			return;
		}
		$num = $this->getPlayerNum($name);
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
						$str .= '残りプレイヤー:'.join(',',$list)."\n";
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
						$str .= $this->getTableString(true)."\n";
						$str .= $this->_template( KOHAKU_MSG_SELECT, Array( 't_name' => $this->wait_leader ) );
					}
				}
				break;
			case self::KOHAKU_SCENE_SELECT:
				if( $this->player[$num]->status != self::PLAYER_STATUS_LIFE ){
					return $this->_template( KOHAKU_MSG_SELECT_ERROR, Array( 'name' => $name ) )."\n";
				}else if( array_search( $num, $this->team[ $this->wait_leader ]['member']) === FALSE ){
					return $this->_template( KOHAKU_MSG_SELECT_ERROR2, Array( 'name' => $name ) )."\n";	
				}else{
					$str = "";
					$this->battle[ $this->battle_cnt ][ $this->wait_leader ] = $num;
				}

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
		}
		return $str;
	}

    function win( $name ){
		//勝者の確定、基本的にnextにしか行かない。
		if( $this->scene != self::KOHAKU_SCENE_BATTLE ){
			return Game::WIN_RETURN_ERROR;
		}
		$num = $this->getPlayerNum($name);
		if( $num == -1 ){
			return Game::WIN_RETURN_ERROR;
		//	 return $this->_template( KOHAKU_MSG_NO_ENTRY, Array( 'name' => $name ) );
		}else if( $this->battle[$this->battle_cnt]['A'] != $num && $this->battle[$this->battle_cnt]['B'] != $num  ){
			return Game::WIN_RETURN_ERROR;
		}
		$this->battle_cnt++;
		foreach( $this->TEAM_LIST as $t ){
			if( array_search( $num, $this->team[$t]['member']) !== FALSE ){
				$this->battle[$this->battle_cnt][$t] = $num;
				$this->wait_leader = $this->TEAM_LIST[$t];
				$l = $this->battle[$this->battle_cnt-1][$this->wait_leader];
				if( $this->team[ $this->wait_leader ]['leader']['num'] == $l && $this->team[ $this->wait_leader ]['leader']['hp'] > 1 ){
					$this->team[ $this->wait_leader ]['leader']['hp']--;
				}else{
					$this->player[$l]->status = self::PLAYER_STATUS_DEFEAT;
					$lifes = 0;	
					foreach( $this->team[ $this->wait_leader ]['member'] as $m ){
						if( $this->player[$m]->status == self::PLAYER_STATUS_LIFE ){
							$lifes++;
						}
					}
					if(!$lifes){ $this->scene = self::KOHAKU_SCENE_END ;}
				}
				break;
			}
		}
		return Game::WIN_RETURN_NEXT;
	}

    function next(){
		//次に戦う人を選んでってアナウンス
		$str .= $this->_template( KOHAKU_MSG_SELECT, Array( 't_name' => $this->wait_leader ) )."\n";
		$str .= $this->getTableString();
		$this->scene = self::KOHAKU_SCENE_SELECT;
		return $str;
	}

    function getTableString( $long = false ){
		//生存者一覧
		$str = "";
		if( $long ){
			foreach( $this->TEAM_LIST as $t ){
				$str .= "○".$t."チーム\n";
				foreach( $this->team[$t]['member'] as $m ){
					$suffix = "";
					if( $m == $this->team[$t]['leader']['num'] ){
						$str .= '☆';
						if( $this->team[$t]['leader']['hp'] > 1 ){
							$suffix = '(HP:'.$this->team[$t]['leader']['hp'].')';
						}
					}else{
						$str .= '　';
					}
					$str .= $this->player[$m]->name.$suffix."\n";
				}
				$str .= "-----------------------------\n";
			}
		}else{
			$t = $this->wait_leader;
			$str .= "○".$t."チーム:";
			$l = Array();
			$d = Array();
			foreach( $this->team[$t]['member'] as $m ){
				$prefix = "";
				$suffix = "";
				if( $m == $this->team[$t]['leader']['num'] ){
					$prefix = '☆';
					if( $this->team[$t]['leader']['hp'] > 1 ){
						$suffix = '('.$this->team[$t]['leader']['hp'].')';
					}
				}
				$s = $prefix.$this->player[$m]->name.$suffix;

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
		return $this->_template( KOHAKU_MSG_GAMESET, Array( 't_name' => $this->wait_leader , 'win_name' => $this->TEAM_LIST[$this->wait_leader] ) );
	}

	private function getRemainderList(){
		$list = Array();
		foreach( $this->player as $k => $p ){
			if( $p->status == self::PLAYER_STATUS_NONE  ){
				$list[$k] = $p->name;
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





?>
