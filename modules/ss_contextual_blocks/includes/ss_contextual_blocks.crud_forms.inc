<?php
/**
 * @file
 * Forms for Creation, Editing, and Deletion of SharpSpring Contextual Rules.
 *
 * @author: Mojiferous
 */

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

  return $form;
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
    ->fields('sbr')
    ->condition('data_id', $data_id)
    ->execute()
    ->fetchCol(1);

  if (!empty($exists)) {
    $vals = (!empty($exists[0])) ? json_decode($exists[0]) : array();
    $rule_name = (!empty($vals->name)) ? $vals->name : '';

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
  if (!empty($form_state['input']['data_id'])) {
    $rule_name = (!empty($form_state['input']['rule_name'])) ? $form_state['input']['rule_name'] : '';

    $deleted = db_delete('sharpspring_contextual_block_rules')
      ->condition('data_id', $form_state['input']['data_id'])
      ->execute();

    drupal_set_message('SharpSpring Contextual Rule ' . $rule_name . ' deleted');
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
  return $form;
}
