<?php
namespace tyobot;

/**
 * common Bot クラス
 *
 **/

include_once "conf/db_search.conf.inc";

include_once "./bot/base.class.inc";

class tyobot_db_search extends tyobot_base {

	var $conn = null;

	var $search_conf = null;
	
	function __construct( &$conn ) {
		global $db_search_conf;
		parent::__construct();
		$this->conn = $conn;

		$this->search_conf = $db_search_conf;
	}

	function _setCommand( &$irc ){

		// search
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. SEARCH_CMD . ' ', $this, COMMAND_SEARCH );
	}


	/**
	 * search 
	 */
	function table_search(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), SEARCH_CMD.' ' );
		$str = "";

		$search_type = array_shift( $args );

		if( !isset( $this->search_conf[ $search_type ] ) ){
			$this->_notice($irc, SEARCH_MSG_ERROR, $data->channel );
			return;
		}
	 	
		// db が記述されていない場合はbotのものを利用。
		if( !isset($this->search_conf[ $search_type ][ 'db' ] ) ){
			$db = $this->conn;
		}else{

			$host = 'localhost';
			if( isset($this->search_conf[ $search_type ][ 'db' ][ 'host' ] ) ){
				$host = $this->search_conf[ $search_type ][ 'db' ][ 'host' ];
			}

			$db =& ADONewConnection('mysql');
			$db->PConnect($host,
				$this->search_conf[ $search_type ][ 'db' ][ 'user' ],
				$this->search_conf[ $search_type ][ 'db' ][ 'pass' ],
				$this->search_conf[ $search_type ][ 'db' ][ 'dbname' ]
			);
			$db->Execute('set names utf8');
		}

		$where = array();
		foreach( $args as $word ){
			$where[] = $this->search_conf[ $search_type ][ 'search' ][ 'colmun' ]. ' like '.$db->qstr('%'.$word.'%').'';
		}
		
		$str = "";
		$rs = $db->Execute('select * from '.$this->search_conf[ $search_type ][ 'search' ][ 'table' ].' where '. join( $where, ' AND '));
		if(!$rs){ $this->sendDBError($irc); }
		else{
			if( $cnt = $rs->RecordCount() ){
				$str .= $cnt.SEARCH_MSG_HIT_COUNT."\n";
				if( $cnt > SEARCH_CNT ){
					$cnt = SEARCH_CNT;
				}

				for( $i=0;$i<$cnt;$i++){
					$rec = $rs->FetchRow();
					$row = "・";
					foreach( $this->search_conf[ $search_type ][ 'return' ] as $return_code ){
						switch( $return_code['type'] ){
							case 'col':
								$row .= $rec[$return_code['name']];
								break;
							case 'txt':
								$row .= $rec[$return_code['src']];
								break;
						}
					}
					$str .= $row."\n";
				}
			}else{
				$str = SEARCH_MSG_NOT_FOUND;
			}
		}

		$this->_notice($irc, $str, $data->channel );
	}

}
?>
