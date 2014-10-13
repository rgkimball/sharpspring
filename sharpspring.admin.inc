<?php
/**
 * @file
 * Administrative page callbacks for the SharpSpring module.
 *
 * @author: rgkimball, adapted from Alexander Hass
 */

/**
 * Implements hook_admin_settings() for module settings configuration.
 */
function sharpspring_admin_settings_form($form_state) {
  $form['account'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Property Settings'),
  );

  $form['account']['sharpspring_account'] = array(
    '#title' => t('Web Property ID'),
    '#type' => 'textfield',
    '#default_value' => variable_get('sharpspring_account', 'KOI-'),
    '#size' => 15,
    '#maxlength' => 20,
    '#required' => TRUE,
    '#description' => t('This ID is unique to each site you want to track separately, and is in the form of KOI-xxxxxxx.'),
  );

  $form['account']['sharpspring_domain'] = array(
    '#title' => t('Web Property Domain'),
    '#type' => 'textfield',
    '#default_value' => variable_get('sharpspring_domain', 'koi-000000.sharpspring.com'),
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
    '#description' => t('This is the SharpSpring sub-domain to which tracking information is sent. It is defined in the tracking code as \'_setDomain\'. Omit the protocol and path: it should fit the following format: koi-xxxxxx.sharpspring.com'),
  );
  // @todo: Add configuration for pages, roles & users to be omitted from tracking

  return system_settings_form($form);
}

/**
 * Implements _form_validate().
 */
function sharpspring_admin_settings_form_validate($form, &$form_state) {
  // Trim some text values.
  $form_state['values']['sharpspring_account'] = trim($form_state['values']['sharpspring_account']);
  $form_state['values']['sharpspring_domain'] = trim($form_state['values']['sharpspring_domain']);

  // Replace all type of dashes (n-dash, m-dash, minus) with the normal dashes.
  $form_state['values']['sharpspring_account'] = str_replace(array('–', '—', '−'), '-', $form_state['values']['sharpspring_account']);

  if (!_sharpspring_validate_id($form_state['values']['sharpspring_account'])) {
    form_set_error('sharpspring_account', t('A valid SharpSpring Web Property ID is case sensitive and formatted like KOI-xxxxxxx.'));
  }
  if (!_sharpspring_validate_domain($form_state['values']['sharpspring_domain'])) {
    form_set_error('sharpspring_domain', t('A valid SharpSpring Domain is formatted like koi-XXXXXX.sharpspring.com. Try removing the leading protocol (e.g. https://) or trailing path. (e.g. /somepath).'));
  }
}
