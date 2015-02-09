<?php
/**
 * @file
 * Forms for Creation, Editing, and Deletion of SharpSpring Contextual Rules.
 *
 * @author: Mojiferous
 */

/**
 * Common helper function to provide the add/edit rule form.
 *
 * @param array $form
 *   An array representing the form.
 * @param array $form_state
 *   An array representing the current form state.
 */
function _ss_contextual_blocks_set_rule_form(&$form, &$form_state) {
  $form['#tree'] = TRUE;

  $form['name'] = array(
    '#type' => 'machine_name',
    '#max_length' => 128,
    '#title' => t('Contextual Rule Name'),
    '#description' => t('A unique machine name for this rule'),
    '#machine_name' => array(
      'exists' => '_ss_contextual_block_rule_name_exists',
    ),
  );

  // Get all of the sharpspring blocks.
  $ss_blocks = _ss_contextual_blocks_return_all_drupal_blocks();

  $form['ss_block'] = array(
    '#type' => 'select',
    '#title' => t('SharpSpring Block'),
    '#options' => $ss_blocks,
    '#description' => t('The SharpSpring block this rule will apply to'),
  );

  $form['rules_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rules'),
    '#prefix' => '<div id="rules-fieldset-wrapper">',
    '#suffix' => '</div>',
  );

  $form['rules_fieldset']['table_begin'] = array(
    '#markup' => '<table>',
  );

  if (empty($form_state['num_rules'])) {
    $form_state['num_rules'] = 1;
  }
  for ($n = 0; $n < $form_state['num_rules']; $n++) {
    $class = ($n % 2 == 0) ? 'even' : 'odd';
    $form['rules_fieldset'][$n]['fieldname'] = array(
      '#type' => 'textfield',
      '#title' => t('Field Name'),
      '#description' => t('The name of the SharpSpring lead field.'),
      '#prefix' => '<tr class="' . $class . '"><td>',
      '#suffix' => '</td>',
    );

    $form['rules_fieldset'][$n]['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Value'),
      '#description' => t('The expected value of this field.'),
      '#prefix' => '<td>',
      '#suffix' => '</td>',
    );

    $form['rules_fieldset'][$n]['remove'] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#name' => 'remove_' . $n,
      '#submit' => array('ss_contextual_blocks_add_rule_remove'),
      '#ajax' => array(
        'callback' => 'ss_contextual_blocks_add_more_callback',
        'wrapper' => 'rules-fieldset-wrapper',
      ),
      '#prefix' => '<td>',
      '#suffix' => '</td></tr>',
    );

  }

  $form['rules_fieldset']['table_end'] = array(
    '#markup' => '</table>',
  );

  $form['rules_fieldset']['add_rule'] = array(
    '#type' => 'submit',
    '#value' => t('Add Rule'),
    '#submit' => array('ss_contextual_blocks_add_rule_add_more'),
    '#ajax' => array(
      'callback' => 'ss_contextual_blocks_add_more_callback',
      'wrapper' => 'rules-fieldset-wrapper',
    ),
  );

  // Get a list of all active Drupal blocks.
  $blocks = _ss_contextual_block_get_all_blocks();

  $form['drupal_block'] = array(
    '#type' => 'select',
    '#title' => t('Drupal Block'),
    '#options' => $blocks,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
}

/**
 * Returns a form to add a new SharpSpring contextual rule.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 *
 * @return array
 *   The current form.
 */
function ss_contextual_blocks_add_rule_form($form, &$form_state) {
  _ss_contextual_blocks_set_rule_form($form, $form_state);

  return $form;
}

/**
 * Submit handler for the add rule form.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the submitted form state.
 */
function ss_contextual_blocks_add_rule_form_submit($form, &$form_state) {
  $new_vals = (!empty($form_state['input'])) ? $form_state['input'] : array();
  $machine_name = (!empty($new_vals['name'])) ? $new_vals['name'] : '';
  $ss_bid = (!empty($new_vals['ss_block'])) ? $new_vals['ss_block'] : 0;
  $block_id = (!empty($new_vals['drupal_block'])) ? $new_vals['drupal_block'] : 0;
  $rules = (!empty($new_vals['rules_fieldset'])) ? json_encode($new_vals['rules_fieldset']) : '{}';

  if (!empty($machine_name) && !empty($ss_bid) && !empty($block_id)) {
    _ss_contextual_blocks_insert_rule($ss_bid, $rules, $block_id, $machine_name);
    drupal_set_message('SharpSpring rule ' . $machine_name . ' saved.');
  }
}

/**
 * Ajax callback for the 'add another rule' button.
 *
 * @param array $form
 *   Array representing the form.
 * @param array $form_state
 *   Array representing the current form state.
 *
 * @return array
 *   Returns form element for rules fieldset.
 */
function ss_contextual_blocks_add_more_callback($form, $form_state) {
  return $form['rules_fieldset'];
}

/**
 * Submit callback for 'Add Rule' button, adds rule row.
 *
 * @param array $form
 *   Array representing the form.
 * @param array $form_state
 *   Array representing the current form state.
 */
function ss_contextual_blocks_add_rule_add_more($form, &$form_state) {
  $form_state['num_rules']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Removes rule from list.
 *
 * @param array $form
 *   Array representing the form.
 * @param array $form_state
 *   Array representing the current form state.
 */
function ss_contextual_blocks_add_rule_remove($form, &$form_state) {
  $remove_num = (isset($form_state['triggering_element']['#parents'][1])) ? $form_state['triggering_element']['#parents'][1] : '';

  if (is_numeric($remove_num)) {
    $current_vals = (!empty($form_state['values']['rules_fieldset'])) ? $form_state['values']['rules_fieldset'] : array();

    unset($current_vals['add_rule']);
    unset($current_vals[$remove_num]);

    $new_vals = array();

    foreach ($current_vals as $rule) {
      $new_vals[] = $rule;
    }

    $form_state['input']['rules_fieldset'] = $new_vals;
    $form_state['num_rules']--;

  }

  $form_state['rebuild'] = TRUE;
}

/**
 * Returns a form to confirm deletion of the passed contextual rule.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 * @param int $data_id
 *   Value of the data_id field representing this rule in the database.
 *
 * @return array
 *   The current form.
 */
function ss_contextual_blocks_delete_rule_form($form, &$form_state, $data_id) {
  $exists = db_select('sharpspring_contextual_block_rules', 'sbr')
    ->fields('sbr', array('machine_name'))
    ->condition('data_id', $data_id)
    ->execute()
    ->fetchCol();

  if (!empty($exists)) {
    $rule_name = (!empty($exists[0])) ? $exists[0] : '';

    drupal_set_title('Are you sure you want to delete the rule ' . $rule_name . '?');

    $form['data_id'] = array(
      '#type' => 'hidden',
      '#value' => $data_id,
    );
    $form['rule_name'] = array(
      '#type' => 'hidden',
      '#value' => $rule_name,
    );

    // This rule exists, show the delete button.
    $form['actions'] = array(
      '#type' => 'actions',
    );

    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#title' => t('Are you sure you want to delete the rule?'),
      '#value' => 'Delete',
    );

    $form['actions']['cancel'] = array(
      '#markup' => l(t('Cancel'), 'admin/config/system/sharpspring/blocks'),
    );

  }
  else {
    // This rule does not exist, redirect back to the admin page.
    drupal_set_message('Unknown rule number, unable to delete.', 'error');
    drupal_goto('admin/config/system/sharpspring/blocks');
  }

  return $form;
}

/**
 * Handles submission of the delete rule form.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array of submitted form values.
 */
function ss_contextual_blocks_delete_rule_form_submit($form, &$form_state) {
  if (!empty($form_state['input']['data_id']) && is_numeric($form_state['input']['data_id'])) {
    $rule_name = (!empty($form_state['input']['rule_name'])) ? $form_state['input']['rule_name'] : '';

    $deleted = db_delete('sharpspring_contextual_block_rules')
      ->condition('data_id', $form_state['input']['data_id'])
      ->execute();

    drupal_set_message('SharpSpring Contextual Rule ' . $rule_name . ' deleted');
    drupal_goto('admin/config/system/sharpspring/blocks');
  }
}

/**
 * Returns a form to confirm deletion of the passed SharpSpring block.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 * @param int $ss_bid
 *   Value of the ss_bid field representing this block in the database.
 *
 * @return array
 *   The current form.
 */
function ss_contextual_blocks_delete_block_form($form, &$form_state, $ss_bid) {
  $exists = db_select('sharpspring_contextual_blocks', 'scb')
    ->fields('scb')
    ->condition('ss_bid', $ss_bid)
    ->execute()
    ->fetchAssoc();

  if (!empty($exists)) {
    $block_name = (!empty($exists['name'])) ? $exists['name'] : '';

    drupal_set_title('Are you sure you want to delete the SharpSpring block ' . $block_name . '?');

    $form['ss_bid'] = array(
      '#type' => 'hidden',
      '#value' => $ss_bid,
    );
    $form['block_name'] = array(
      '#type' => 'hidden',
      '#value' => $block_name,
    );

    // This rule exists, show the delete button.
    $form['actions'] = array(
      '#type' => 'actions',
    );

    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#title' => t('Are you sure you want to delete the block?'),
      '#description' => t('This will also delete all rules associated with the block'),
      '#value' => 'Delete',
    );

    $form['actions']['cancel'] = array(
      '#markup' => l(t('Cancel'), 'admin/config/system/sharpspring/blocks'),
    );

  }
  else {
    // This block does not exist, redirect back to the admin page.
    drupal_set_message('Unknown block number, unable to delete.', 'error');
    drupal_goto('admin/config/system/sharpspring/blocks');
  }

  return $form;
}

/**
 * Handles submission of the delete block form.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array of submitted form values.
 */
function ss_contextual_blocks_delete_block_form_submit($form, &$form_state) {
  if (!empty($form_state['input']['ss_bid']) && is_numeric($form_state['input']['ss_bid'])) {
    $block_name = (!empty($form_state['input']['block_name'])) ? $form_state['input']['block_name'] : '';

    $deleted = db_delete('sharpspring_contextual_blocks')
      ->condition('ss_bid', $form_state['input']['ss_bid'])
      ->execute();

    // Also delete all rules associated with this block.
    $deleted = db_delete('sharpspring_contextual_block_rules')
      ->condition('ss_bid', $form_state['input']['ss_bid'])
      ->execute();

    drupal_set_message('SharpSpring block ' . $block_name . ' deleted');
    drupal_goto('admin/config/system/sharpspring/blocks');
  }
}

/**
 * Returns a form to edit the passed contextual rule.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 * @param int $data_id
 *   Value of the data_id field representing this rule in the database.
 *
 * @return array
 *   The current form.
 */
function ss_contextual_blocks_edit_rule_form($form, &$form_state, $data_id) {
  $this_rule = _ss_contextual_blocks_return_rule_from_data_id($data_id);

  if (!empty($this_rule)) {
    if (empty($form_state['num_rules'])) {
      // If this is the first load of the form, set up the default rules.
      $rules = (!empty($this_rule['segment_rule'])) ? json_decode($this_rule['segment_rule'], TRUE) : array();

      if (count($rules) > 0) {
        $form_state['num_rules'] = count($rules);
      }
    }

    _ss_contextual_blocks_set_rule_form($form, $form_state);

    $form['name']['#disabled'] = TRUE;
    $form['name']['#required'] = FALSE;
    $form['name']['#default_value'] = (!empty($this_rule['machine_name'])) ? $this_rule['machine_name'] : '';

    $form['ss_block']['#default_value'] = (!empty($this_rule['ss_bid'])) ? $this_rule['ss_bid'] : 0;

    $form['drupal_block']['#default_value'] = (!empty($this_rule['block_id'])) ? $this_rule['block_id'] : 0;

    foreach ($rules as $rule_num => $rule_content) {
      $form['rules_fieldset'][$rule_num]['fieldname']['#default_value'] = (!empty($rule_content['fieldname'])) ? $rule_content['fieldname'] : '';
      $form['rules_fieldset'][$rule_num]['value']['#default_value'] = (!empty($rule_content['value'])) ? $rule_content['value'] : '';
    }

    $form['data_id'] = array(
      '#type' => 'hidden',
      '#value' => $data_id,
    );

  }
  else {
    // This rule does not exist, redirect back to the admin page.
    drupal_set_message('Unknown rule.', 'error');
    drupal_goto('admin/config/system/sharpspring/blocks');
  }

  return $form;
}

/**
 * Handles edit rule form submission.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 */
function ss_contextual_blocks_edit_rule_form_submit($form, &$form_state) {
  $new_vals = (!empty($form_state['input'])) ? $form_state['input'] : array();
  $ss_bid = (!empty($new_vals['ss_block'])) ? $new_vals['ss_block'] : 0;
  $block_id = (!empty($new_vals['drupal_block'])) ? $new_vals['drupal_block'] : 0;
  $rules = (!empty($new_vals['rules_fieldset'])) ? json_encode($new_vals['rules_fieldset']) : '{}';
  $data_id = (!empty($new_vals['data_id'])) ? $new_vals['data_id'] : 0;

  if (!empty($data_id) && !empty($ss_bid) && !empty($block_id)) {
    _ss_contextual_blocks_update_rule($ss_bid, $rules, $block_id, $data_id);
    drupal_set_message('SharpSpring rule updated.');
  }
}
