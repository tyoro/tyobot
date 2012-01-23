<?php

/** Zend_Http_Client */
require_once 'Zend/Http/Client.php';

/** Zend_Locale */
require_once 'Zend/Locale.php';

/** Zend_Translate_Adapter */
require_once 'Zend/Translate/Adapter.php';

/**
 * Google translate adapter
 *
 * @package Quino_Translate
 * @subpackage Adapter
 */
class Quino_Translate_Adapter_Google extends Zend_Translate_Adapter
{
    /**
     * Translate language api url
     *
     * @var string
     */
    private $_uri = 'http://ajax.googleapis.com/ajax/services/language/translate';

    /**
     * Api version string
     *
     * @var string
     */
    private $_apiversion = '1.0';

    /**
     * Generates the adapter
     *
     * @param  array               $data     Translation data
     * @param  string|Zend_Locale  $locale   OPTIONAL Locale/Language to set, identical with locale identifier,
     *                                       see Zend_Locale for more information
     * @param  array               $options  OPTIONAL Options to set
     */
    public function __construct($data, $locale = null, array $options = array())
    {
        parent::__construct($data, $locale, $options);
    }

    /**
     * Load translation data
     *
     * @param  string|array  $data
     * @param  string        $locale  Locale/Language to add data for, identical with locale identifier,
     *                                see Zend_Locale for more information
     * @param  array         $options OPTIONAL Options to use
     * @return array
     */
    protected function _loadTranslationData($data, $locale, array $options = array())
    {
		$options = $options + $this->_options;
		if (($options['clear'] === true) ||  !isset($this->_translate[$locale])) {
			$this->_translate[$locale] = array();
		}

		$this->_translate[$locale] = (array) $data + $this->_translate[$locale] + array($locale);

		return array();
    }

    /**
     * Translates the given string
     * returns the translation
     *
     * @see Zend_Locale
     * @param  string|array       $messageId Translation string, or Array for plural translations
     * @param  string|Zend_Locale $locale    (optional) Locale/Language to use, identical with
     *                                       locale identifier, @see Zend_Locale for more information
     * @return string
     */
    public function translate($messageId, $locale = null)
    {
        if (is_null($locale)) {
            $locale = $this->_options['content']['locale'];
        }

        try {
            $locale = Zend_Locale::findLocale($locale);
        } catch (Zend_Locale_Exception $e) {
            // language does not exist, return original string
            return $messageId;
        }
        $locale = new Zend_Locale($locale);
        $locale = $locale->getLanguage();

        $source = null;


        if (isset($this->_options['content']['source'])) {
            try {
                $source = Zend_Locale::findLocale($this->_options['content']['source']);
            } catch (Zend_Locale_Exception $e) {
                $source = null;
            }
            if (!is_null($source)) {
                $source = new Zend_Locale($source);
                $source = $source->getLanguage();
            }
        }

        if ($source == $locale) {
            // return original string
            return $messageId;
        }

        $client = new Zend_Http_Client();
        $client->setUri($this->_uri);
        $client->setConfig(array(
            'maxredirects' => 0,
            'timeout'      => 30,
            'useragent'    => null,
        ));

        $client->setParameterGet(array(
            'v'        => $this->_apiversion,
            'q'        => $messageId,
            'langpair' => $source . '|' . $locale,
        ));

        $response = $client->request();
        if ($response->getStatus() !== 200) {
            require_once 'Zend/Translate/Exception.php';
            throw new Zend_Translate_Exception('Translate failed to load, got response code ' . $response->getStatus());
        }

        $data = json_decode($response->getBody());
        if ($data->responseStatus !== 200) {
            require_once 'Zend/Translate/Exception.php';
            throw new Zend_Translate_Exception('Google translate api error message is "' . $data->responseDetails . '"');
        }

        return $data->responseData->translatedText;
    }

    /**
     * returns the adapters name
     *
     * @return string
     */
    public function toString()
    {
        return 'Google';
    }
}

?>
