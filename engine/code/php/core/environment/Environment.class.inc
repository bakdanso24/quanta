<?php
define("DIR_INACTIVE", '_');
define("DIR_ALL", '');

/**
 * Class Environment
 * This class represents an Environment with his directories etcetera.
 */
class Environment extends DataContainer {
  public $host = array();
  public $dir = array();
  public $request_uri;
  public $request_json;
  public $request = array();
  private $modules = array();
  private $includes = array();
  private $items = array();

  // Construct the Environment.
  public function __construct($host = NULL, $request_uri = NULL) {
    $this->host = ($host == NULL) ? strtolower($_SERVER['HTTP_HOST']) : $host;
    if ($request_uri == NULL) {
      $this->request_uri = (str_replace('/', '', $_SERVER['REQUEST_URI']) == '') ? 'home/' : $_SERVER['REQUEST_URI'];
    } else {
      $this->request_uri = $request_uri;
    }
    $this->request = explode('/', $this->request_uri);
    $this->request_wiki = $this->request[count($this->request) - 2];
    $this->dir['docroot'] = $_SERVER['DOCUMENT_ROOT'] . '/sites/' . $this->host;
    $this->dir['tmp'] = $_SERVER['DOCUMENT_ROOT'] . '/engine/tmp/' . $this->host;
    $this->dir['tmp_files'] = $this->dir['tmp'] . '/files';
    $this->dir['wiki'] = $_SERVER['DOCUMENT_ROOT'] . '/engine/tmp/' . $this->host . '/wiki';
    $this->dir['log'] = $_SERVER['DOCUMENT_ROOT'] . '/engine/tmp/' . $this->host . '/log';
    $this->dir['users'] = $this->dir['docroot'] . '/_users';
    if (isset($_REQUEST['json'])) {
      $this->request_json = json_decode($_REQUEST['json']);
    }
    $this->load();
  }

  /**
   * Returns the current requested path.
   * @return string mixed
   */
  public function getRequestedPath() {
    return $this->request_wiki;
  }

  private function loadCoreModules() {
    $core_modules = $this->scanDirectory('core');
    $this->setModules('core', $core_modules);
  }

  private function loadContribModules() {
    $contrib_modules = $this->scanDirectory($this->dir['docroot'] . '/_modules');
    $this->setModules('contrib', $contrib_modules);
  }

  /**
   * Setup modules
   * @param $modules
   */
  public function setModules($mod_type, $modules) {
    $this->modules[$mod_type] = $modules;
  }

  public function getModules($mod_type = NULL) {
    $modules = array();
    if ($mod_type == NULL) {
      foreach($this->modules as $modules_dir) {
        foreach ($modules_dir as $module) {
          $modules[] = $module;
        }
      }
    } else {
      $modules = $this->modules[$mod_type];
    }
    return $modules;
  }

  public function runModules() {
    foreach ($this->modules as $mod_type => $modules) {
      foreach ($modules as $module) {
        $module_path = $mod_type . '/' . $module . '/' . $module . '.module';
        if (is_file($module_path)) {
          require_once($module_path);
        } else {
          die('Error: ' . $module_path . ' is not a file. Check includes!');
        }
      }
    }
  }

  /**
   * Get all dirs inside a given dir.
   */
  public function scanDirectory($base_dir = '', $exclude_dirs = DIR_INACTIVE) {
    if (!is_dir($base_dir)) {
      return array();
    }
    $dirs = array_diff(scandir($base_dir), array('.', '..'));
    if ($exclude_dirs != NULL) {
      foreach ($dirs as $k => $dir) {
        if (substr($dir, 0, 1) == $exclude_dirs) {
          unset ($dirs[$k]);
        }
      }
    }
    return $dirs;
  }

  private function load() {
    foreach ($this->dir as $folder) {
      if (!is_dir($folder)) {
        mkdir($folder) or die("Error: can not create " . $folder . '. Check permission and path.');
      }
    }
    $this->loadCoreModules();
    $this->loadContribModules();
  }

  public function dir($variable) {
    return $this->dir[$variable];
  }

  public function get($variable) {
    return $this->{$variable};
  }

  public function hook($function, $vars = array()) {
    $vars['env'] = &$this;
    foreach ($this->getModules() as $module) {
      $hook = $module . '_' . $function;
      if (function_exists($hook)) {
        $hook($vars);
      }
    }
    return TRUE;
  }


  public function startSession() {
    session_start();
  }

  public function getIncludes() {
    return $this->includes;
  }

  public function addInclude($include, $type = NULL) {
    if ($type == NULL) {
      $ext = explode('.', $include);
      $type = $ext[count($ext) - 1];
    }
    $this->includes[] = array('path' => $include, 'type' => $type);
  }

  /**
   * Adds an item to the environment. An item can be everything
   * (messages, nodes, ...)
   * @param $item
   */
  public function addItem($item, $module) {
    if (!is_object($item)) {
      die('Error: ' . var_export($item, 1) . ' is not a valid engine object');
    }
    $this->items[strtolower($module)][] = $item;
  }

  /**
   * Get all items of one type / module (i.e. message)
   * @param $module
   * @return object
   */
  public function getItems($module) {
    if (isset($this->items[strtolower($module)])) {
      $items = $this->items[strtolower($module)];
    } else {
      $items = array();
    }
    return $items;
  }


  public function getCandidatePath($title) {
    $candidate_path = strtolower(str_replace('_', '-', preg_replace('/[\s\W]+/', '', str_replace(' ', '_', trim($title)))));
    if (!file_exists($this->dir['wiki'] . '/' . $candidate_path)) {
      $path = $candidate_path;
    } else {
      $i = 1;
      while (file_exists($this->dir['wiki'] . '/' . $candidate_path . '-' . $i)) {
        $i++;
      }
      $path = $candidate_path . '-' . $i;
    }
    return $path;
  }

}