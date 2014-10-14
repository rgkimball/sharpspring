<?php
/**
 * @file
 * Administrative page callbacks for the SharpSpring module.
 *
 * @author: rgkimball
 */

/**
 * Implements hook_form().
 */
function sharpspring_admin_config_form($form_state) {
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

  $form['tracking'] = array(
    '#type' => 'vertical_tabs',
  );

  $form['tracking']['webforms'] = array(
    '#title' => t('Webform Lead Generation'),
    '#description' => t('<p>Add unique tracking IDs to each webform on the site. For each webform you want to track, enter the form\'s endpoint ID as it appears in the unique tracking code. It should be formatted like xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (8-4-4-4-12).</p>'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );

  // Get list of all webforms
  $wf_query = db_select('webform', 'nid')
    ->fields('nid')
    ->orderBy('nid', 'ASC')
    ->execute();

  // Get list of all SharpSpring form endpoints
  if (db_table_exists('sharpspring')) {
    $ss_query = db_select('sharpspring', 'nid')
      ->fields('nid')
      ->orderBy('nid', 'ASC')
      ->execute();

    $webform_codes = array();
    foreach ($ss_query as $row) {
      $webform_codes[$row->nid] = $row;
    }

  }
  else {
    drupal_set_message(t('SharpSpring DB Table does not exist. Please disable and re-enable the module before using webform components.'), 'error');
    exit();
  }

  $forms_count = 0;
  foreach ($wf_query as $row) {
    $forms_count++;
    $webform = node_load($row->nid);

    $thisrow = 'webform_row_' . $row->nid;
    $unpublished = !$webform->status;

    $enabled = (isset($webform_codes[$row->nid])) ? $webform_codes[$row->nid]->status : 0;
    $endpoint = (isset($webform_codes[$row->nid])) ? $webform_codes[$row->nid]->endpoint : '';
    $baseURI = (isset($webform_codes[$row->nid])) ? $webform_codes[$row->nid]->base_uri : '';

    $form['tracking']['webforms']['enable'][$row->nid] = array(
      '#title' => l($webform->title, 'node/' . $row->nid . '/webform'),
      '#default_value' => $enabled,
      '#disabled' => $unpublished, // disable checkbox if form is unpublished
      '#type' => 'checkbox',
      '#size' => 50,
      '#maxlength' => 40,
    );

    $form['tracking']['webforms'][$row->nid] = array(
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $row->nid . '"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form['tracking']['webforms'][$row->nid]['header'] = array(
      '#markup' => '<h4 style="margin-top: -10px;">"' . $webform->title . '" Form</h4>',
    );

    $form['tracking']['webforms'][$row->nid]['base_uri_' . $row->nid] = array(
      '#title' => 'Base URI',
      '#description' => t('The form\'s baseURI will appear in the tracking code as: <pre>__ss_noform.push([\'baseURI\', \'<strong>https://app-XXXXXX.sharpspring.com/webforms/receivePostback/XXXXXXXX/</strong>\']);</pre>'),
      '#default_value' => $baseURI,
      '#type' => 'textfield',
      '#size' => 50,
      '#maxlength' => 255,
    );

    $form['tracking']['webforms'][$row->nid]['endpoint_' . $row->nid] = array(
      '#title' => 'Endpoint',
      '#description' => t('The form\'s endpoint will appear in the tracking code as: <pre>__ss_noform.push([\'endpoint\', \'<strong>xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx</strong>\']);</pre>'),
      '#default_value' => $endpoint,
      '#type' => 'textfield',
      '#size' => 70,
      '#maxlength' => 40,
    );

  }

  $form['tracking']['webforms']['count'] = array(
    '#title' => $webform->title,
    '#type' => 'hidden',
    '#default_value' => $forms_count,
  );

  /** @todo: These tabs:
  $form['tracking']['pages'] = array(
    '#title' => t('Pages'),
    '#description' => t('@todo: Textarea to add or omit certain paths à la Context'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );

  $form['tracking']['users'] = array(
    '#title' => t('Users & Roles'),
    '#description' => t('@todo: List all user roles w/ a checkbox for each'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );
  */

  $form['save'] = array(
    '#value' => 'Save configuration',
    '#type' => 'submit',
  );
  return $form;
}

/**
 * Implements hook_form_validate().
 */
function sharpspring_admin_config_form_validate($form, &$form_state) {

  // Trim some text values.
  $form_state['values']['sharpspring_account'] = trim($form_state['values']['sharpspring_account']);
  $form_state['values']['sharpspring_domain'] = trim($form_state['values']['sharpspring_domain']);

  // Replace all type of dashes (n-dash, m-dash, minus) with the normal dashes.
  $form_state['values']['sharpspring_account'] = str_replace(array('–', '—', '−'), '-', $form_state['values']['sharpspring_account']);

  if (!_sharpspring_validate_id($form_state['values']['sharpspring_account'])) {
    form_set_error('sharpspring_account', t('A valid SharpSpring Web Property ID is case sensitive and formatted like KOI-xxxxxxx.'));
  }
  else {
    variable_set('sharpspring_account', $form_state['values']['sharpspring_account']);
  }
  if (!_sharpspring_validate_domain($form_state['values']['sharpspring_domain'])) {
    form_set_error('sharpspring_domain', t('A valid SharpSpring Domain is formatted like koi-XXXXXX.sharpspring.com. Try removing the leading protocol (e.g. https://) or trailing path. (e.g. /somepath).'));
  }
  else {
    variable_set('sharpspring_domain', $form_state['values']['sharpspring_domain']);
  }

  // Only get numeric array keys within webforms group (i.e. webform nids)
  $keys = array_filter(array_keys($form['tracking']['webforms']), 'is_int');

  $i = 0;
  foreach ($keys as $wid) {

    $status = $form['tracking']['webforms']['enable'][$wid]['#value'];
    $base_uri = $form['tracking']['webforms'][$wid]['base_uri_' . $wid]['#value'];
    $endpoint = $form['tracking']['webforms'][$wid]['endpoint_' . $wid]['#value'];

    // If the form tracking isn't active, it doesn't matter if these are wrong.
    if($status == 1) {
      // Check whether the base URI and endpoint will actually work.
      if(!_sharpspring_validate_endpoint($endpoint)) {
        form_set_error('endpoint_' . $wid, t('A valid SharpSpring Endpoint is formatted like xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx. Please check for any whitespace in your entry.'));
        continue;
      }
      if(!_sharpspring_validate_uri($base_uri)) {
        form_set_error('base_uri_' . $wid, t('A valid SharpSpring Base URI is formatted like https://app-XXXXXX.sharpspring.com/webforms/receivePostback/XXXXXXXX.'));
        continue;
      }
    }

    if (isset($wid) && isset($status) && isset($base_uri) && isset($endpoint)) {

      $result_check = db_select('sharpspring', 'nid')
        ->fields('nid')
        ->condition('nid', $wid, '=')
        ->execute();

      // If there's any results, delete them.
      foreach($result_check as $result) {
        db_delete('sharpspring')
        ->condition('nid', $wid, '=')
        ->execute();
      }

      // Now save new info
      db_insert('sharpspring')
        ->fields(array(
          'nid' => check_plain($wid),
          'base_uri' => check_plain($base_uri),
          'endpoint' => check_plain($endpoint),
          'status' => check_plain($status),
        ))
        ->execute();

      $webform = node_load($wid);

      if($status == 1) {
        drupal_set_message('Successfully saved data for ' . $webform->title . ' form');
      }
    }
    else {
      form_set_error($wid, t('Unable to save form data for @id'), array('@id' => $wid));
    }

    $i++;
  }

}
