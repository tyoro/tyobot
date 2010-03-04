<?php

/**
 * common Bot クラス
 *
 **/

include_once "./conf/common.conf.inc";

include_once "XML/RSS.php";
include_once 'Services/Amazon.php';
include_once "include/simple_html_dom.php";

include_once "./bot/base.class.inc";

class tyobot_common extends tyobot_base{

	var $amazon;
	function __construct(){
		$this->amazon = new Services_Amazon(AMAZON_API_ACCESS_KEY, AMAZON_API_SECRET_KEY, AMAZON_ASSOCIATE_TAG);
		$this->amazon->setLocale( 'JP' );
	}


  /**
   * URL検出 
  */
  function url(&$irc,&$data){
    $message = $this->_2utf8( $data->message );
    if (!(preg_match_all('/'.URL_PATTERN.'/', $message, $matches))) {
      $this->_notice($irc, URL_ERR_MSG, $data->channel );
    } else {
      $urls = $matches[0];
	  $str = "";
      foreach( $urls as $url ){
		$heads = get_headers($url,true);
		$status = strstr($heads[0],' ');
		switch( (int)$status ){
		  case 301: case 302:
			$str .= '▼Redirect to '.$heads['Location']."\n";
			$url = $heads['Location'];
			$heads = get_headers($url,true);
			$status = strstr($heads[0],' ');
			if( (int)$status != 200 ){
				$str .= '■No 200 ..redirecting.. ahead.';
				$str .= '　'.$status;
				break;
			}
		  case 200:
       		if( preg_match_all( URL_AMAZON_ITEM_LINK_REGEX, $url, $matches )){
		      $asin = $matches[2][0];
        	  	$item = $this->amazon->ItemLookup( $asin, Array());
        	  	if( is_array( $item ) ){
        	   		$str .= "■Amazon(".$item['Item'][0]['ItemAttributes']['Title'].") ".URL_AMAZON_TINY_URL.$asin."\n";
					break;
        	  	}
       		}
			$ct = explode(' ',$heads['Content-Type']);
			$types = explode( '/',str_replace(';','',$ct[0]));
			switch( $types[0] ){
			  case 'text':
				if( $types[1] == 'html' ){
					$html = file_get_html($url);
					$titles = $html->find('title',0);
					if( strlen( $titles->innertext ) ){
						$str .= to_short('■'.trim($this->_2utf8($titles->innertext)),URL_TITLE_MAX_LENGTH);
						$str .= '['.$ct[0].']';
						 break;
					}
					$html->clear();
				}
			default:
				$str .= "■".$ct[0];
				if( !empty($heads['Content-Length']) ){
					$str .= '('.ConvertUnit($heads['Content-Length']).'B)';
				}
				break;
			case 'image':
				$size = getimagesize($url);
				$str .= "■".$ct[0];
				if( !empty($heads['Content-Length']) ){
					$str .= '('.ConvertUnit($heads['Content-Length']).'B)';
				}
				if( $size[0] && $size[1] ){
					$str .= '('.$size[0].'*'.$size[1].')';
				}
				break;
			}
			break;
		  default:
	  		$str = '■'.$status;
		}
		
		$this->_notice($irc,$str,$data->channel);
      }
    }

  }

  function google(&$irc,&$data){
    $message = $this->_2utf8( $data->message );
    $text = substr($message,2);
    $html = file_get_html('http://www.google.com/search?ls=ja&hl=ja&oe=utf-8&q='.urlencode($text));
    $title = $html->find('title',0);
	$str = '▼'.$this->_2utf8( $title->innertext ).'('.$html->find('p[id=resultStats]/b',1)->innertext.")\n";
    $list = $html->find('li');
	$i=1;
    foreach( $list as $e ){
		$str .= str_replace( Array('<em>','</em>'), Array('',''),  '■'.$this->_2utf8( $e->find( 'h3[class=r]/a',0 )->innertext ) );
		$str .= ':'.$this->_2utf8( $html->find( 'a[class=l]',0 )->href )."\n";
		$desc = $this->_2utf8( $e->find( 'div[class=s]',0 )->innertext );
		$desc = str_replace( Array('<em>','</em>','<b>','</b>'), '', substr( $desc, 0,  strpos( $desc, '<br>') ) );
		$desc = to_short( $desc, GOOGLE_DESCRIPTION_LENGTH );
		$str .= $desc."\n";
	
		if( $i++ >= GOOGLE_CNT ){ break; }
	}
    $this->_notice($irc, $str, $data->channel );
  }

  function calculator(&$irc,&$data){
    $message = $this->_2utf8( $data->message );
    $text = substr($message,2);
	$m=Array();
	if( preg_match('/[^0123456789()+-\/\*\%]/',$text,$m) ){
	  $this->_notice($irc,CALCULATOR_MSG,$data->channel);
	  return;
	}
	eval( '$ret = '."$text;" );  
	$this->_notice($irc, $ret,$data->channel);
  }

  function twitter(&$irc,&$data){
    $message = $this->_2utf8( $data->message );
    $text = substr($message,2);
	$c = explode(' ',$text);
	$id = $c[0];
	$str = "";
	try{
	  $twitter =& new Services_Twitter( TWITTER_ID,TWITTER_PASS ); 
	  if( isset($c[1]) && is_numeric($c[1]) ){
	    $cnt = $c[1];
		if( $cnt > TWITTER_TIMELINE_MAX ){ $cnt = TWITTER_TIMELINE_MAX; }
	    $timelines = $twitter->statuses->user_timeline(Array('id' => $id, 'count' => $cnt ));
	    foreach( $timelines as $timeline ){
		  $str .= date('m/d h:i',strtotime($timeline->created_at)).':'.$timeline->text."\n";
		}
	  }else{
        $user = $twitter->users->show($id);
	    $str .= $user->name.' ('.$user->screen_name.') '.$user->url.' http://twitter.com/'.$user->screen_name.' bio:'.to_short($user->description,40)."\n".
	    	"followers:".$user->followers_count.' following:'.$user->friends_count.' status:'.$user->statuses_count.' fav:'.$user->favourites_count;
      }
      $this->_notice($irc, $str,$data->channel);
	} catch ( Services_Twitter_Exception $e){
	  switch( $e->getCode() ){
	    case 404:
		  $this->_notice($irc, 'this user is '. $e->getMessage(),$data->channel);
		  break;
		default:
		//  d($e->getFile().'('.$e->getLine().')'.$e);
	  }
	}
  }
 
 function ustream(&$irc,&$data){
   $message = $this->_2utf8( $data->message );
   $text = substr($message,2); 
   $url = USTREAM_URL.$text;
   $heads = get_headers($url,true);
   $status = strstr($heads[0],' ');
   if( (int)$status != 200 ){
     $this->_notice( $irc, USTREAM_MSG_ERROR, $data->channel );
   }else{
     $this->_notice( $irc, $url, $data->channel );
   }
 }

	/**
	* Amazon 検索
	*
	*/

	function amazon(&$irc,&$data){
		$message = $this->_2utf8( $data->message );
		$text = substr($message,2);
		$str = "";
		$result = $this->amazon->ItemSearch( 'All', Array( 'Keywords' => $text) );
		if( is_array($result)){
			foreach( array_splice( $result['Item'], 0, AMAZON_CNT ) as $item ){
				$str .= '■['.$item['ItemAttributes']['ProductGroup'].']'.$item['ItemAttributes']['Title'];
				$str .= ' '.URL_AMAZON_TINY_URL.$item['ASIN']."\n";
			}
		}else{
			$str .= '■'.AMAZON_MSG_ERROR;
		}
		$this->_notice( $irc, $str, $data->channel );
	}


  /*
   * BOT を終了する
   *
   *
   */

  function quit (&$irc) {
    $irc->quit($this->_convert(QUIT_MSG));
  }

}

?>
