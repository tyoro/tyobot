<?php
namespace tyobot;

/**
 * common Bot クラス
 *
 **/

include_once "conf/common.conf.inc";

include_once "XML/RSS.php";
include_once 'Services/Amazon.php';
require_once 'XML/Unserializer.php';
include_once "include/simple_html_dom.php";
include_once 'HTTP/OAuth/Consumer.php';

require_once 'Zend/Loader.php';
	\Zend_Loader::loadClass('Zend_Gdata');
	\Zend_Loader::loadClass('Zend_Gdata_AuthSub');
	\Zend_Loader::loadClass('Zend_Gdata_ClientLogin');
	//Zend_Loader::loadClass('Zend_Gdata_Docs');
	\Zend_Loader::loadClass('Zend_Gdata_Spreadsheets');
	\Zend_Loader::loadClass('Zend_Gdata_YouTube');
	\Zend_Loader::loadClass('Zend_Service_Twitter');

include_once "./bot/base.class.inc";

class tyobot_common extends tyobot_base {

	var $amazon;
	var $google_client;
	var $google_spreadseets;
	var $google_docs;
	var $google_youtube;

	var $twitter_oauth;

	var $conn = null;
	
	function __construct( &$conn ) {
		parent::__construct();
		$this->amazon = new \Services_Amazon(AMAZON_API_ACCESS_KEY, AMAZON_API_SECRET_KEY, AMAZON_ASSOCIATE_TAG);
		$this->amazon->setLocale('JP');

		$this->google_client = \Zend_Gdata_ClientLogin::getHttpClient( GOOGLE_USER, GOOGLE_PASS, \Zend_Gdata_Spreadsheets::AUTH_SERVICE_NAME );
		//$this->google_docs = new \Zend_Gdata_Docs($this->google_client);
		$this->google_spreadseets = new \Zend_Gdata_Spreadsheets($this->google_client);
		$this->google_youtube = new \Zend_Gdata_YouTube();
		$this->conn = $conn;
	}

	function _setCommand( &$irc ){
		// BOT を終了する$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^' . BOT_NICK . DELIMITER . COMMAND_QUIT, $bot_c, COMMAND_QUIT);

		// URLのタイトルを表示
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, URL_PATTERN , $this, COMMAND_URL );

		// twitter
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. TWITTER_CMD . ' ', $this, COMMAND_TWITTER );

		// facebook
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. FACEBOOK_CMD . ' ', $this, COMMAND_FACEBOOK );

		// Google
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. GOOGLE_CMD . ' ', $this, COMMAND_GOOGLE );

		// Google transit
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. GOOGLE_TRANSIT_CMD . ' ', $this, COMMAND_GOOGLE_TRANSIT );

		// ustream
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. USTREAM_CMD .' ', $this, COMMAND_USTREAM );

		// amazon
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. AMAZON_CMD . ' ', $this, COMMAND_AMAZON );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. AMAZON_KINDLE_CMD . ' ', $this, COMMAND_AMAZON_KINDLE );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. ASIN_CMD . ' ', $this, COMMAND_ASIN );

		//amazon world
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. AMAZON_CMD_COM . ' ', $this, COMMAND_AMAZON_WORLD );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. AMAZON_CMD_DE . ' ', $this, COMMAND_AMAZON_WORLD );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. AMAZON_CMD_FR . ' ', $this, COMMAND_AMAZON_WORLD );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. AMAZON_CMD_UK . ' ', $this, COMMAND_AMAZON_WORLD );

		// iTunes
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. ITUNES_CMD . ' ', $this, COMMAND_ITUNES );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. ITUNES_MUSIC_CMD . ' ', $this, COMMAND_ITUNES_MUSIC );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. ITUNES_APP_CMD . ' ', $this, COMMAND_ITUNES_APP );
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. ITUNES_IPAD_CMD . ' ', $this, COMMAND_ITUNES_IPAD );

		// WindowsPhone
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. WP7_CMD. ' ', $this, COMMAND_ZUNE_WP7_APP );

		// Wikipedia
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. WIKIPEDIA_CMD. ' ', $this, COMMAND_WIKIPEDIA );
		
		// alias
		$irc->registerActionhandler(SMARTIRC_TYPE_CHANNEL, '^'. ALIAS_CMD . ' ', $this, COMMAND_ALIAS );

		// inviteされた
		$irc->registerActionhandler(SMARTIRC_TYPE_INVITE, '.*', $this, '__invited__' );


		// 外部システムとの連携用に特定ファイルに書き込まれた文字をircに書き込み
		$irc->registerTimehandler( 10000, $this, 'checkPost');

		//再接続時のチャンネルへの復帰処理
		$irc->registerTimehandler( 60*1000, $this, 'checkChannelJoin');
	}

	/**
	 * URL検出
	 */
	function url(&$irc, &$data) {
		$message = $this->_2utf8($data->message );
		if (!(preg_match_all('/'.URL_PATTERN.'/', $message, $matches))) {
			$this->_notice($irc, URL_ERR_MSG, $data->channel );
		} else {
			$urls = $matches[0];
			d(memory_get_usage());
			foreach ($urls as $url) {
				$str = $this->get_url_string( $url );
				d(memory_get_usage());
				if( $str ){
					$this->_notice($irc, $str, $data->channel);
				}
			}
		}
	}

	function get_url_string($url) {
		try{
			if( preg_match( URL_WIKIPEDIA_TOPPAGE_LINK_REGEX, $url ) ){
				//日本語URLが取得できていない場合。
				return false;
			}
			$str = "";
			if( strpos( $url, URL_AMAZON_TINY_URL ) !== false ){
				//指定のAmazon短縮URLだった場合、転送処理を行なわずに結果を返す
				$asin = substr( $url, strlen( URL_AMAZON_TINY_URL ) );
				if( $p = strpos( $asin, '/' ) ){
					$asin = substr( $asin, 0, $p );
				}
				if( $asin != 'gift' && strlen( $asin) == 10  ){
					$item = $this->amazon->ItemLookup($asin, Array('ResponseGroup' => 'Medium'));
					if (is_array($item)) {
						$str .= "【A】".$this->amazon_string($item['Item'][0],URL_AMAZON_TINY_URL)."\n";
					}
					return $str;
				}
			}
			
			//ログインを促されるパターンを回避(TODO: 後のやつとやってる事は同じなのでまとめる
			if (preg_match_all(URL_TWITTER_STATUS_LINK_REGEX, $url, $matches)) {
            	$twitter = new \Zend_Service_Twitter($this->twitter_oauth);
				$status = $twitter->statuses->show($matches[5][0]);
				$str = '【Tw】'.$status->user->name.'「'.$status->text.'  」';
			}else if (preg_match_all(URL_TWITTER_USER_LINK_REGEX, $url, $matches)) {
            	$twitter = new \Zend_Service_Twitter($this->twitter_oauth);
				$user = $twitter->user->show($matches[3][0]);
				$str = $user->name.' ('.$user->screen_name.') '.$user->url.' http://twitter.com/'.$user->screen_name.' bio:'.to_short($user->description, 40)."\n"."followers:".$user->followers_count.' following:'.$user->friends_count.' status:'.$user->statuses_count.' fav:'.$user->favourites_count;
			}else{

				$heads = my_get_headers($url, 1);
				if( $heads === FALSE  ){ return '■get response error.'; }
				if( $heads['count'] > 0 ){
					if( $heads['last-status'] == 301 || $heads['last-status'] == 302 ){
						if( $heads['loop']){
							return '■Redirecting has looped.';
						}
						return '■It redirects it three times or more. ';
					}
					$url = $heads[ 'last-location'];
					$str .= '▼Redirect to '.$url."\n";
				}
				if( $heads['last-status'] != 200 ){
					return '■'.$heads[ $heads['count'] ]['status-line'];
				}



				if (preg_match_all(URL_AMAZON_ITEM_LINK_REGEX, $url, $matches)) {
					$asin = $matches[2][0];
					$item = $this->amazon->ItemLookup($asin, Array('ResponseGroup' => 'Medium'));
					if (is_array($item)) {
						$str .= "【A】".$this->amazon_string($item['Item'][0],URL_AMAZON_TINY_URL)."\n";
					}
				} else if (preg_match_all(URL_NICONICO_WATCH_LINK_REGEX, $url, $matches)) {
					$api_url = 'http://ext.nicovideo.jp/api/getthumbinfo/'.$matches[1][0];
					$xml = file_get_contents($api_url);
					$Unserializer =& new \XML_Unserializer(Array('parseAttributes' => TRUE));
					$xml_status = $Unserializer->unserialize($xml);
					if ($xml_status) {
						$api_array = $Unserializer->getUnserializedData();
						if ($api_array['status'] == 'ok') {
							$str .= '【ニコ】'.$api_array['thumb']['title'].'['.$api_array['thumb']['length'].'](再生:'.$api_array['thumb']['view_counter'].")\n";
						}
					}
				} else if (preg_match_all(URL_YOUTUBE_WATCH_LINK_REGEX, $url, $matches)) {
					$entry = $this->google_youtube->getVideoEntry( $matches[1][0] );
					$str .= '【Y】'.$entry->getVideoTitle().'['.$entry->getVideoCategory().']';
				}else if (preg_match_all(URL_GOOGLE_SPREADSHEETS_LINK_REGEX, $url, $matches)) {
					$query = new \Zend_Gdata_Spreadsheets_DocumentQuery();
					$query->setSpreadsheetKey($matches[1][0] );
					$worksheet_feed = $this->google_spreadseets->getWorksheetFeed( $query );
					$str = '【G-s】'.$worksheet_feed->title->text;
				}else if (preg_match_all(URL_TWITTER_STATUS_LINK_REGEX, $url, $matches)) {
					$twitter = new \Zend_Service_Twitter($this->twitter_oauth);
					$status = $twitter->statuses->show($matches[5][0]);
					$str = '【Tw】'.$status->user->name.'「'.$status->text.'  」';
				}else if (preg_match_all(URL_TWITTER_USER_LINK_REGEX, $url, $matches)) {
					$twitter = new \Zend_Service_Twitter($this->twitter_oauth);
					$user = $twitter->users->show($matches[3][0]);
					$str = $user->name.' ('.$user->screen_name.') '.$user->url.' http://twitter.com/'.$user->screen_name.' bio:'.to_short($user->description, 40)."\n"."followers:".$user->followers_count.' following:'.$user->friends_count.' status:'.$user->statuses_count.' fav:'.$user->favourites_count;
				}else{
					$head = $heads[$heads['count']];
					$ct = explode(' ', $head['content-type']);
					$types = explode('/', str_replace(';', '/', $ct[0]));
					switch ($types[0]) {
						case 'text':
							if ($types[1] == 'html') {
								$html = file_get_html($url);
								d(memory_get_usage());
								$titles = $html->find('title', 0);
								if (strlen($titles->innertext )) {
									$str .= to_short('■'.preg_replace('/\s\s+/', ' ', str_replace(array("\r\n", "\n", "\r"), '', trim($this->_2utf8($titles->innertext)))), URL_TITLE_MAX_LENGTH);
									$str .= '['.$ct[0].']';
									$html->clear();
									break;
								}
								$html->clear();
							}
						default:
							$str .= "■".$ct[0];
							if (!empty($head['content-length'])) {
								$str .= '('.ConvertUnit($head['content-length'],0).'B)';
							}
							break;
						case 'image':
							$size = getimagesize($url);
							$str .= "■".$ct[0];
							if (!empty($head['content-length'])) {
								$str .= '('.ConvertUnit($head['content-length'],0).'B)';
							}
							if ($size[0] && $size[1]) {
								$str .= '('.$size[0].'*'.$size[1].')';
							}
							break;
					}
				}
			}
		} catch (\Services_Twitter_Exception $e) {
			switch ($e->getCode()) {
				case 404:
				case 403:
					$str = 'this twitter request is '.$e->getMessage();
					break;
				default:
					$str = 'this twitter request to code:'.$e->getCode();
			}
		}
		return $str;
	}

	function google_transit(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), GOOGLE_TRANSIT_CMD.' ' );



	}

	function google(&$irc, &$data) {
		$message = $this->_2utf8($data->message );
		if( $data->nick  == 'kakuge_anon' ){
			$matches = array();
			if( preg_match( '/(.*)\(CV\:.*\)/i', $message ,$matches ) ){
				$message = $matches[1];
			}
		}
		$text = substr($message, 2);
		d(memory_get_usage());

		$html = str_get_html(
			file_get_contents('http://www.google.co.jp/search?q='.urlencode($text),false,
			stream_context_create(
					array(
			  'http'=>array(
				      'method'=>"GET",
					  'header'=> "Accept: */*\r\n".
					  "User-Agent: Mozilla/4.0 (compatible; MSIE 10.0; Windows NT 6.2; Trident/6.0)\r\n".
					  "Accept-language: ja,en-US;q=0.8,en;q=0.6\r\n".
					  "Cache-Control: no-cache"
				  )
			  )
				//,array( "q"=> urlencode($text))
		  )));

		//BOXが出ている時はそちらのみ表示する。
		if( strlen($str = $html->find( 'div[id=cwmcwd]//span[class=cwcot]',0)->plaintext) );
		else if( strlen( $str = $html->find( 'div[id=search]//div[class=answer_predicate]',0 )->plaintext ));
		else if( strlen( ( $ans_set = $html->find('li[id=tdu-main]/div[id=tdu-result]/div[class=tdu-open]',0) )) ){
			$str = trim($ans_set->find('div[class=tdu-total-time]',0)->plaintext).' ('. $ans_set->find('div[class=tdu-time-delta]',0)->plaintext.') '
				.$ans_set->find('div[class=tdu-total-fare]',0)->plaintext.' / '
				.$ans_set->find('ul[class=tdu-transportation-summary]',0)->plaintext;
			
			$long = $html->find('div[class=tdu-footer]/div[class=tdu-maps]/a',0)->href;
			$short =  json_decode(file_get_contents('https://www.googleapis.com/urlshortener/v1/url',false,
				stream_context_create(
					array(
						'http'=>array(
							"method"  => "POST",
							"header"  => "Content-Type: application/json",
							"content" => '{"longUrl": "'.htmlspecialchars_decode($long).'"}' //http_build_query(array('longUrl'=>$long), "", "&",PHP_QUERY_RFC3986 )
						)))));
			$str .= ' '. $short->id;
		}
		else if( strlen( ( $ans_set = $html->find('div[id=topstuff]/div[class=leg_calc]',0) )) ){
			$str = $ans_set->find('div[class=vk_sh]',0)->plaintext.$ans_set->find('div[class=vk_ans]',0)->plaintext;
		} else {
		//BOXが出ていないので通常検索 
			$title = $html->find('title', 0);
			$str = '▼'.$this->_2utf8($title->innertext )."\n";//.'('.$html->find('p[id=resultStats]/b', 1)->innertext.")\n";

			$calc = $html->find('div[id=topstuff]//h2',0);
			$calc_text = trim( $calc->innertext );

			if( strlen( $calc_text ) ){
				$str .= $calc_text;
			}else{
				$list = $html->find('div[id=ires]//li');
				$i = 1;

				foreach ($list as $e) {
					$a = $e->find('h3[class=r]/a', 0);
					if( is_null($a) ){ continue; }
					$str .= strip_tags('■'.$this->_2utf8($a->plaintext ));
					$url = $a->href;
					$url = substr($url,7,strpos($url,"&amp;sa=U")-7);
					$str .= ':'.$this->_2utf8($url )."\n";
					$desc = $this->_2utf8($e->find('div[class=s]', 0)->innertext );
					$desc = strip_tags(substr($desc, 0, strpos($desc, '<br>')));
					$desc = to_short($desc, GOOGLE_DESCRIPTION_LENGTH);
					$str .= $desc."\n";

					if ($i++ >= GOOGLE_CNT) {
						break;
					}
				}
			}
		}

		$this->_notice($irc, $str, $data->channel );
		d(memory_get_usage());
		$html->clear();
	}

	function wikipedia(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), WIKIPEDIA_CMD.' ' );
		$word = trim(join($args,' '));
		$php = file_get_html('https://ja.wikipedia.org/w/api.php?action=query&list=search&srprop=size&format=php&srsearch='.urlencode($word));
		$d = unserialize($php);
		$hit=$d['query']['searchinfo']['totalhits'];
		if( $hit){
			$first = $d['query']['search'][0];
			$str = '■'
			 . "[$hit hit] top:".$first['title']." (".$first['size']."byte)";

			// Link用のpageidを取得
			$php = file_get_html('http://ja.wikipedia.org/w/api.php?action=query&format=php&prop=info&titles='.urlencode($first['title']));
			$d = unserialize($php);
			$d = array_pop($d['query']['pages']);
			$pageid = $d['pageid'];
			$str .= " http://ja.wikipedia.org/?curid=$pageid\n";
			
			$php = file_get_html('http://ja.wikipedia.org/w/api.php?action=parse&section=0&prop=text&format=php&page='.urlencode($first['title']));
			$d = unserialize($php);
			$text = $d['parse']['text']['*'];
			//重複回避の削除する？

			//templateの排除
			while(( $p = strpos($text,'</table>')) !== false ){
				$text = substr($text,$p+8);
			}

			//末尾の削除
			$p = strpos($text,'<br />');
			if( $p !== false ){
				$text = substr($text,0,$p);
			}
			d($text);
			//サムネ画像(のキャプション)の削除
			$p = strpos($text, '<div class="thumbcaption">');
			if( $p !== false ){
				$text = substr_replace($text,'', $p, strpos( $text, '<p>', $p )-$p);
			}
			d($text);

			//注釈の排除する？

			//タグの排除
			$text = trim(strip_tags($text));
			$text = str_replace(array("\r\n","\n","\r"), "", $text);

			d($text);
			$str .= to_multiple($text,180);
			$this->_notice($irc, $str, $data->channel);
		}else{
			$this->_notice($irc, WIKIPEDIA_MSG_NOT_FOUND, $data->channel );
		}
	}

	function twitter(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), TWITTER_CMD.' ' );
		$id = $args[0];
		$str = "";
		if( !preg_match("/^[a-zA-Z0-9]+$/", $id ) )
		{
			$this->_notice($irc, TWITTER_ERROR, $data->channel);
			return;
		}
		try {
            $twitter = new \Zend_Service_Twitter($this->twitter_oauth);
			if (isset($args[1]) && is_numeric($args[1])) {
				$cnt = $args[1];
				if ($cnt > TWITTER_TIMELINE_MAX) {
					$cnt = TWITTER_TIMELINE_MAX;
				}
				$timelines = $twitter->statuses->userTimeline(Array('id' => $id, 'count' => $cnt));
				foreach ($timelines as $timeline) {
					$str .= date('m/d h:i', strtotime($timeline->created_at)).':'.$timeline->text."\n";
				}
			} else {
				$user = $twitter->users->show($id);
				$str .= $user->name.' ('.$user->screen_name.') '.$user->url.' http://twitter.com/'.$user->screen_name.' bio:'.to_short($user->description, 40)."\n"."followers:".$user->followers_count.' following:'.$user->friends_count.' status:'.$user->statuses_count.' fav:'.$user->favourites_count;
			}
			$this->_notice($irc, $str, $data->channel);
		} catch ( Zend_Service_Twitter_Exception $e) {
			switch ($e->getCode()) {
				case 404:
				case 403:
					$this->_notice($irc, 'this user is '.$e->getMessage(), $data->channel);
					break;
				default:
					$this->_notice($irc, 'this twitter request to code:'.$e->getCode(), $data->channel);
			}
		}
	}

	function facebook(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), FACEBOOK_CMD.' ' );
		$id = $args[0];
		$str = "";
		try {
			$user =  file_get_contents( 'https://graph.facebook.com/'.$id.'?access_token='.FACEBOOK_APP_ACCESS_TOKEN);
			$user_data = json_decode( $user );
			if( !isset($user_data->error) ){
				$str = $id.'('.$user_data->name.') '. $user_data->link.' '.$user_data->gender.' '.mb_ereg_replace("\n",' ',$user_data->about);
			}else{
				$str = FACEBOOK_MSG;
			}
			$this->_notice($irc, $str, $data->channel);

		} catch (FacebookApiException $e) {
			d($e);
		}
	}

	function ustream(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), USTREAM_CMD.' ' );
		$url = USTREAM_URL.$args[0];
		$heads = get_headers($url, 1);
		$status = strstr($heads[0], ' ');
		if ((int) $status != 200) {
			$this->_notice($irc, USTREAM_MSG_ERROR, $data->channel );
		} else {
			$this->_notice($irc, $url, $data->channel );
		}
	}

	/**
	 * Amazon 検索
	 *
	 */

	function amazon(&$irc, &$data, $url = "a", $args = null, $type = "All") {
		if( is_null( $args )){
			$args = $this->_command( $this->_2utf8($data->message), AMAZON_CMD.' ' );
		}
		$str = "";
		if( $args[0] == URL_AMAZON_GIFT_KEY )
		{
			$this->_notice($irc, URL_AMAZON_GIFT_URL_STR, $data->channel );
			return;
		}
		$result = $this->amazon->ItemSearch($type, Array('Keywords' => join(' ',$args),'ResponseGroup' => 'Medium'));
		if (is_array($result) ) {
			foreach (array_splice($result['Item'], 0, AMAZON_CNT) as $item) {
				$str .= '■'.$this->amazon_string($item,URL_AMAZON_TINY_URL_PRE.$url.URL_AMAZON_TINY_URL_SUF)."\n";
			}
		} else {
			$str = '■'.AMAZON_MSG_ERROR;
			if( count($args) == 1 && ctype_alnum( $args[0] ) ){
				$s = $this->getAsin( $args[0], $url );
			 	if( $s ){
					$str = $s;
				}
			}
		}
		$this->_notice($irc, $str, $data->channel );
	}
	function amazon_string( $item, $url ){
		$str = '['.$item['ItemAttributes']['ProductGroup'].']'.$item['ItemAttributes']['Title'].' '.$item['OfferSummary']['LowestNewPrice']['FormattedPrice'];
		$str .= ' '.$url.$item['ASIN'];
		return $str;
	}
	function amazon_kindle(&$irc, &$data  ) {
		$args = $this->_command( $this->_2utf8($data->message), AMAZON_KINDLE_CMD. ' ' );
		array_push( $args, "kindle");
		$this->amazon( $irc, $data, 'a' , $args, "Books" );
	}
	function asin(&$irc, &$data ) {
		$args = $this->_command( $this->_2utf8($data->message), ASIN_CMD.' ' );
		$str = $this->getAsin( $args[0] );
		if( ! $str ){
			$str = '■'.AMAZON_MSG_ERROR;
		}
		$this->_notice($irc, $str, $data->channel );
	}
	function getAsin( $id, $url = 'a' ){
		$str = "";
		$result = $this->amazon->ItemLookup( $id );
		if (is_array($result) ) {
			foreach (array_splice($result['Item'], 0, AMAZON_CNT) as $item) {
				$str .= '■['.$this->amazon_string($item,URL_AMAZON_TINY_URL_PRE.$url.URL_AMAZON_TINY_URL_SUF)."\n";
				return $str;
			}
		}
		return false;
	}

	function amazon_world(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), AMAZON_CMD );
		$domain = array_shift($args);
		if( $domain == "com" ){ $locale = "US"; }
		else{ $locale = strtoupper($domain); }
		$this->amazon->setLocale($locale);
		$this->amazon($irc,$data,$domain,$args);
		$this->amazon->setLocale('JP');
	}

	/**
	 * iTunes検索
	 *
	 */
	function itunes(&$irc, &$data, $category='' ) {
		$args = $this->_command( $this->_2utf8($data->message), ITUNES_CMD.' ' );
		$URL = ITUNES_BASE_URL.'term='.rawurlencode( join(' ',$args) ).'&country=JP&lang=ja_jp&limit=3'.$category;
		$json = json_decode(file_get_contents( $URL ) );
		if( $json->resultCount ){
			foreach( $json->results as $item ){
				switch( $item->wrapperType ){
					case 'software':
						if( isset( $item->trackCensoredName ) ){
							$name = $item->trackCensoredName;
						}else{
							$name = $item->trackName;
						}
						$str = '■[app]'.$name.' '.$item->version.' \\'.$item->price.' [ '.$item->primaryGenreName.'] '.$this->getITunesAfiliateURLPart( $item->trackViewUrl );
						break;
					case 'track':
						$str = '■[music]'.$item->collectionNamee.' - '.$item->artistName.' \\'.$item->collectionPrice.' '.$this->getITunesAfiliateURLPart( $item->collectionViewUrl );
						break;
					default:
						$str = '■['.$item->wrapperType.']'.$item->trackName.' \\'.$item->price.'';
						break;
				}
				$this->_notice($irc, $str, $data->channel );
			}
		}else{
			$this->_notice($irc, ITUNES_MSG_NOT_FOUND, $data->channel );
		}
	}
	function itunes_m(&$irc, &$data) {
		$data->message = substr_replace( $data->message, 's ', 1,2);
		$this->itunes( $irc, $data, '&media=music&entity=musicTrack&sort=recent' );
	}
	function itunes_a(&$irc, &$data) {
		$data->message = substr_replace( $data->message, 's ', 1,2);
		$this->itunes( $irc, $data, '&media=software&entity=software' );
	}
	function itunes_ipad(&$irc, &$data) {
		$data->message = substr_replace( $data->message, 's ', 1,4);
		$this->itunes( $irc, $data, '&media=software&entity=iPadSoftware' );
	}
	private function getITunesAfiliateURLPart( $url ){
		$url = str_replace( 'http://itunes.apple.com/jp/', '', $url );
		return ITUNES_TINY_URL.str_replace( '%', '%25', ($url) );
		//return ITUNES_TINY_URL.str_replace( '%', '%25', rawurlencode($url) );
	}

	function zune_wp7_app(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), WP7_CMD.' ' );
		$URL = WP7_BASE_URL. urlencode( join(' ',$args));

		$xml = file_get_contents( $URL );

		$Unserializer =& new \XML_Unserializer(Array('parseAttributes' => TRUE));
		$xml_status = $Unserializer->unserialize($xml);
		if ($xml_status) {
			$xml_result= $Unserializer->getUnserializedData();

			if ($xml_result['os:totalResults'] >= 1) {
				if( isset($xml_result['a:entry']['a:updated']) ){
					//1行
					$entry_array = array($xml_result['a:entry']);
				}else{
					$entry_array = $xml_result['a:entry'];
				}
				foreach( $entry_array as $key => $entry ){
					$name = $entry["a:title"]["_content"];
					if( isset($entry["offers"]["offer"][0]["displayPrice"])){
						$price = $entry["offers"]["offer"][0]["displayPrice"];
					}else{
						$price = $entry["offers"]["offer"]["displayPrice"];
					}

					$genres = array();

					if( isset($entry["categories"]["category"]["title"])){
						$genreName = $entry["categories"]["category"]["title"];
					}else{
						foreach( $entry["categories"]["category"] as $category ){
							$genres[] = $category["title"];
						}
						$genreName = join( " > ", $genres );
					}
					$appUrl = WP7_APP_URL.substr($entry["a:id"],9);

					$str = '■'.$name.'  \\'.$price.' [ '.$genreName.'] '.$appUrl ;
					$this->_notice($irc, $str, $data->channel );
					
					if( $key == WP7_CNT-1 ){
						break;
					}
				}
			}else{
				$this->_notice($irc, WP7_MSG_NOT_FOUND, $data->channel );
			}
		}else{
			$this->_notice($irc, WP7_MSG_ERROR, $data->channel );
		}
	}

	/**
	 * Alias
	 *
	 */
	function alias(&$irc, &$data) {
		$args = $this->_command( $this->_2utf8($data->message), ALIAS_CMD.' ' );
		$text = $args[0];
		if(strlen($text)){
			$rs = &$this->conn->Execute('select * from alias WHERE alias = '.$this->conn->qstr($text));
			d( 'select * from alias WHERE alias = '.$this->conn->qstr($text));
			if(!$rs){ d($this->conn->ErrorMsg());}
			else if( $rs->RecordCount() ){
				$row = $rs->FetchRow();
				$this->_notice( $irc, $row['string'], $data->channel );
			}else{
				$this->_notice( $irc, ALIAS_MSG_ERROR, $data->channel );
			}
		}

	}

	function checkPost( &$irc ){
		if( defined( 'AUTO_IRC_MSG_FILE' ) ){
			$fp = fopen(AUTO_IRC_MSG_FILE,'r+');
			if( !$fp ){ return; } 
			$buffer = array();
			while ($data=fgets($fp, 10000)){
				$buffer[] = $data; 
			}
			$first = array_shift( $buffer );

			ftruncate($fp,0);
			fseek( $fp, 0 );

			foreach( $buffer as $row ){
				fputs( $fp, $row );
			}

			fclose( $fp );
			if( !is_null( $first ) ){
				$channel = substr( $first, 0, strpos( $first, ' ' ) );
				$str = substr( $first, strpos( $first, ' ' )+1 );
				$this->_notice( $irc, $str, $channel );
			}
		}
	}

	function checkChannelJoin( &$irc ){
		global $channels;

		foreach( $channels as $c ){
			if( !$irc->isJoined( $c ) ){
				$irc->join($c);
			}
		}
	}


	/*
	 * BOT を終了する
	 *
	 *
	 */

	function quit(&$irc) {
		$irc->quit($this->_convert(QUIT_MSG));
	}

}
?>
