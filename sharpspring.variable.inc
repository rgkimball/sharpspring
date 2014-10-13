<?php
/**
 * @file
 * Definition of variables for Variable API module.
 * @author: rgkimball, adapted from Alexander Hass
 */

/**
 * Implements hook_variable_info().
 */
function sharpspring_variable_info($options) {
  $variables['sharpspring_account'] = array(
    'type' => 'string',
    'title' => t('Web Property ID', array(), $options),
    'default' => 'KOI-',
    'description' => t('This ID is unique to each site you want to track separately, and is in the form of KOI-xxxxxxx.'),
    'required' => TRUE,
    'group' => 'sharpspring',
    'localize' => TRUE,
    'multidomain' => TRUE,
    'validate callback' => 'sharpspring_validate_sharpspring_account',
  );

  $variables['sharpspring_domain'] = array(
    'type' => 'string',
    'title' => t('Web Property Domain', array(), $options),
    'default' => 'koi-',
    'description' => t('This is the SharpSpring sub-domain to which tracking information is sent.'),
    'required' => TRUE,
    'group' => 'sharpspring',
    'localize' => TRUE,
    'multidomain' => TRUE,
    'validate callback' => 'sharpspring_validate_sharpspring_domain',
  );

  return $variables;
}

/**
 * Implements hook_variable_group_info().
 */
function sharpspring_variable_group_info() {
  $groups['sharpspring'] = array(
    'title' => t('SharpSpring'),
    'description' => t('Configure tracking behavior to get insights into your website traffic and marketing effectiveness.'),
    'access' => 'administer sharpspring',
    'path' => array('admin/config/system/sharpspring'),
  );

  return $groups;
}

/**
 * Validate Web Property ID variable.
 */
function sharpspring_validate_sharpspring_account($variable) {
  // Replace all type of dashes (n-dash, m-dash, minus) with the normal dashes.
  $variable['value'] = str_replace(array('–', '—', '−'), '-', $variable['value']);

  if (!_sharpspring_validate_id( $variable['value'])) {
    return t('A valid SharpSpring Web Property ID is case sensitive and formatted like KOI-xxxxxxx.');
  }
}

/**
 * Validate Web domain variable.
 */
function sharpspring_validate_sharpspring_domain($variable) {
  if (!_sharpspring_validate_domain( $variable['value'])) {
    return t('A valid SharpSpring Domain is formatted like koi-XXXXXX.sharpspring.com. Try removing the leading protocol (e.g. https://) or trailing path. (e.g. /somepath).');
  }
}
