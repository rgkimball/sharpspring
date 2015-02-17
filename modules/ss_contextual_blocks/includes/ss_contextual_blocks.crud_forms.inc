<?php
/**
 * @file
 * Forms for Creation, Editing, and Deletion of SharpSpring Contextual Rules.
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
    '#type' => 'textfield',
    '#max_length' => 128,
    '#title' => t('Rule Name'),
    '#description' => t('A name for this rule, use something easily identifiable.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#rows' => 3,
    '#title' => t('Rule Description'),
    '#description' => t('Description of the rule.'),
  );

  $form['rules_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rules'),
    '#prefix' => '<div id="rules-fieldset-wrapper">',
    '#suffix' => '</div>',
  );

  $form['rules_fieldset']['and_or'] = array(
    '#type' => 'select',
    '#title' => 'Rule passes if the lead has',
    '#options' => array(
      'and' => t('All of these field values'),
      'or' => t('Any of these field values'),
    ),
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

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
}

/**
 * Common helper function to define the block add/edit form.
 *
 * @param array $form
 *   An array of form elements.
 */
function _ss_contextual_set_block_form(&$form) {
  $form['#tree'] = TRUE;

  $form['block_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Block Name'),
    '#description' => t('A name to identify this block.'),
    '#required' => TRUE,
  );

  $form['block_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Block Description'),
    '#description' => t('A brief description of the function of this block.'),
    '#rows' => 3,
    '#required' => TRUE,
  );

  $form['reactions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reactions'),
  );

  $form['reactions']['table_begin'] = array(
    '#markup' => '<table><tr><th>SharpSpring Rule</th><th>Block Shown</th></tr>',
  );

  $blocks = _ss_contextual_block_get_all_blocks();
  $blocks[-1] = '- None -';
  ksort($blocks);

  $form['reactions'][0]['react_rule'] = array(
    '#type' => 'select',
    '#options' => $blocks,
    '#prefix' => '<tr class="even"><td>Default</td><td>',
    '#suffix' => '</td></tr>',
  );

  $current_rules = _ss_contextual_blocks_return_all_ss_rules();
  $blocks[0] = '- Default -';
  ksort($blocks);

  $count = 1;
  foreach ($current_rules as $rule) {
    $class = ($count % 2 == 0) ? 'even' : 'odd';
    if (!empty($rule['rule_id']) && isset($rule['name'])) {
      $form['reactions'][$rule['rule_id']]['react_rule'] = array(
        '#type' => 'select',
        '#options' => $blocks,
        '#prefix' => '<tr class="' . $class . '"><td>' . $rule['name'] . '</td><td>',
        '#suffix' => '</td></tr>',
      );
      $count++;
    }
  }

  $form['reactions']['table_end'] = array(
    '#markup' => '</table>',
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
  $new_vals = (!empty($form_state['values'])) ? $form_state['values'] : array();
  $name = (!empty($new_vals['name'])) ? $new_vals['name'] : '';
  $description = (!empty($new_vals['description'])) ? $new_vals['description'] : 0;
  $rules = (!empty($new_vals['rules_fieldset'])) ? $new_vals['rules_fieldset'] : array();
  $and_or = (!empty($rules['and_or'])) ? $rules['and_or'] : 'and';
  unset($rules['and_or']);
  $rules = array(
    'and_or' => $and_or,
    'rules' => $rules,
  );
  $rules = json_encode($rules);

  $new_rule = _ss_contextual_blocks_insert_rule($rules, $name, $description);
  drupal_set_message(t('SharpSpring rule @name saved.'), array('@name' => $name));

  $form_state['redirect'] = 'admin/config/system/sharpspring/blocks/edit_rule/' . $new_rule;
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

    $new_vals = array(
      'and_or' => (!empty($current_vals['and_or'])) ? $current_vals['and_or'] : 'any',
    );
    unset($current_vals['and_or']);

    foreach ($current_vals as $rule) {
      $new_vals[] = $rule;
    }

    $form_state['input']['rules_fieldset'] = $new_vals;
    $form_state['num_rules']--;

    if ($form_state['num_rules'] == 0) {
      // If this is the last rule, clear out the value.
      $form_state['input']['rules_fieldset'][0] = array(
        'fieldname' => '',
        'value' => '',
      );
      $form_state['num_rules'] = 1;
    }

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
 * @param int $rule_id
 *   Value of the rule_id field representing this rule in the database.
 *
 * @return array
 *   The current form.
 */
function ss_contextual_blocks_delete_rule_form($form, &$form_state, $rule_id) {
  $exists = db_select('sharpspring_contextual_block_rules', 'sbr')
    ->fields('sbr', array('name'))
    ->condition('rule_id', $rule_id)
    ->execute()
    ->fetchCol();

  if (!empty($exists)) {
    $rule_name = (!empty($exists[0])) ? $exists[0] : '';

    drupal_set_title('Are you sure you want to delete the rule ' . $rule_name . '?');

    $form['rule_id'] = array(
      '#type' => 'hidden',
      '#value' => $rule_id,
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
    drupal_set_message(t('Unknown rule number, unable to delete.'), 'error');
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
  if (!empty($form_state['values']['rule_id']) && is_numeric($form_state['values']['rule_id'])) {
    $rule_name = (!empty($form_state['values']['rule_name'])) ? $form_state['values']['rule_name'] : '';

    $deleted = db_delete('sharpspring_contextual_block_rules')
      ->condition('rule_id', $form_state['values']['rule_id'])
      ->execute();

    drupal_set_message(t('SharpSpring Contextual Rule @rule_name deleted'), array('@rule_name' => $rule_name));
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
    drupal_set_message(t('Unknown block number, unable to delete.'), 'error');
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
  if (!empty($form_state['values']['ss_bid']) && is_numeric($form_state['values']['ss_bid'])) {
    $block_name = (!empty($form_state['values']['block_name'])) ? $form_state['values']['block_name'] : '';

    db_delete('sharpspring_contextual_blocks')
      ->condition('ss_bid', $form_state['values']['ss_bid'])
      ->execute();

    // Also delete all rules associated with this block.
    db_delete('sharpspring_contextual_block_reactions')
      ->condition('ss_bid', $form_state['values']['ss_bid'])
      ->execute();

    drupal_set_message(t('SharpSpring block @block_name deleted'), array('@block_name' => $block_name));
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
 * @param int $rule_id
 *   Value of the data_id field representing this rule in the database.
 *
 * @return array
 *   The current form.
 */
function ss_contextual_blocks_edit_rule_form($form, &$form_state, $rule_id) {
  $this_rule = _ss_contextual_blocks_return_rule_from_data_id($rule_id);

  if (!empty($this_rule)) {
    $raw_rules = (!empty($this_rule['segment_rule'])) ? json_decode($this_rule['segment_rule'], TRUE) : array();
    $and_or = (!empty($raw_rules['and_or'])) ? $raw_rules['and_or'] : 'and';
    $rules = (!empty($raw_rules['rules'])) ? $raw_rules['rules'] : array();

    if (empty($form_state['num_rules'])) {
      // If this is the first load of the form, set up the default rules.
      if (count($rules) > 0) {
        $form_state['num_rules'] = (!empty($rules)) ? count($rules) : 0;
      }
    }

    _ss_contextual_blocks_set_rule_form($form, $form_state);

    $form['name']['#default_value'] = (!empty($this_rule['name'])) ? $this_rule['name'] : '';
    $form['description']['#default_value'] = (!empty($this_rule['description'])) ? $this_rule['description'] : '';
    $form['rules_fieldset']['and_or']['#default_value'] = $and_or;

    foreach ($rules as $rule_num => $rule_content) {
      $form['rules_fieldset'][$rule_num]['fieldname']['#default_value'] = (!empty($rule_content['fieldname'])) ? $rule_content['fieldname'] : '';
      $form['rules_fieldset'][$rule_num]['value']['#default_value'] = (!empty($rule_content['value'])) ? $rule_content['value'] : '';
    }

    $form['rule_id'] = array(
      '#type' => 'hidden',
      '#value' => $rule_id,
    );

  }
  else {
    // This rule does not exist, redirect back to the admin page.
    drupal_set_message(t('Unknown rule.'), 'error');
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
  $new_vals = (!empty($form_state['values'])) ? $form_state['values'] : array();
  $name = (!empty($new_vals['name'])) ? $new_vals['name'] : '';
  $description = (!empty($new_vals['description'])) ? $new_vals['description'] : '';
  $rules = (!empty($new_vals['rules_fieldset'])) ? $new_vals['rules_fieldset'] : array();
  $and_or = (!empty($rules['and_or'])) ? $rules['and_or'] : 'and';
  unset($rules['and_or']);
  $rules = array(
    'and_or' => $and_or,
    'rules' => $rules,
  );
  $rules = json_encode($rules);
  $rule_id = (!empty($new_vals['rule_id'])) ? $new_vals['rule_id'] : 0;

  if (!empty($rule_id) && is_numeric($rule_id)) {
    _ss_contextual_blocks_update_rule($rule_id, $rules, $name, $description);
    drupal_set_message(t('SharpSpring rule updated.'));
  }
}

/**
 * Returns a form to add a new SharpSpring contextual block.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 *
 * @return array
 *   The current form.
 */
function ss_contextual_blocks_add_block_form($form, &$form_state) {
  _ss_contextual_set_block_form($form);

  return $form;
}

/**
 * Handles saving a new SharpSpring contextual block.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 */
function ss_contextual_blocks_add_block_form_submit($form, &$form_state) {
  $new_vals = (!empty($form_state['values'])) ? $form_state['values'] : array();
  $name = (!empty($new_vals['block_name'])) ? $new_vals['block_name'] : '';
  $description = (!empty($new_vals['block_description'])) ? $new_vals['block_description'] : '';
  $reactions = (!empty($new_vals['reactions'])) ? $new_vals['reactions'] : array();

  if (!empty($name) && !empty($description)) {
    $new_bid = _ss_contextual_blocks_insert_block($name, $description, $reactions);
    drupal_set_message(t('SharpSpring block @name saved.'), array('@name' => $name));
    $form_state['redirect'] = 'admin/config/system/sharpspring/blocks/edit_block/' . $new_bid;
  }
}

/**
 * Returns the edit SharpSpring block form.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 * @param int $ss_bid
 *   SharpSpring block id.
 *
 * @return array
 *   The current form.
 */
function ss_contextual_blocks_edit_block_form($form, &$form_state, $ss_bid) {
  _ss_contextual_set_block_form($form);

  $this_block = _ss_contextual_blocks_return_block_from_block_id($ss_bid);
  $reactions = _ss_contextual_blocks_return_reactions_from_block_id($ss_bid);

  $form['block_name']['#default_value'] = (!empty($this_block['name'])) ? $this_block['name'] : '';
  $form['block_description']['#default_value'] = (!empty($this_block['description'])) ? $this_block['description'] : '';

  foreach ($reactions as $react_id => $saved_reaction) {
    if (isset($form['reactions'][$react_id]) && isset($saved_reaction->bid)) {
      $form['reactions'][$react_id]['react_rule']['#default_value'] = $saved_reaction->bid;
    }
  }

  $form['ss_bid'] = array(
    '#type' => 'hidden',
    '#value' => $ss_bid,
  );

  return $form;
}

/**
 * Handles saving of and edited SharpSpring block.
 *
 * @param array $form
 *   An array of form elements.
 * @param array $form_state
 *   An array representing the current form state.
 */
function ss_contextual_blocks_edit_block_form_submit($form, &$form_state) {
  $values = (!empty($form_state['values'])) ? $form_state['values'] : array();
  $ss_bid = (!empty($values['ss_bid'])) ? $values['ss_bid'] : 0;
  $name = (!empty($values['block_name'])) ? $values['block_name'] : '';
  $description = (!empty($values['block_description'])) ? $values['block_description'] : '';
  $reactions = (!empty($values['reactions'])) ? $values['reactions'] : array();

  if (!empty($ss_bid) && !empty($name) && !empty($description)) {
    _ss_contextual_blocks_update_block($ss_bid, $name, $description, $reactions);
    drupal_set_message(t('SharpSpring block @name updated.'), array('@name' => $name));
  }
}
