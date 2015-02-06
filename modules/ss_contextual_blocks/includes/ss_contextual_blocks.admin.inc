<?php
/**
 * @file
 * Admin form handlers for the SharpSpring contextual blocks module.
 *
 * @author: Mojiferous
 */

/**
 * Defines the block config form.
 *
 * @param array $form
 *   An array containing form values
 * @param array $form_state
 *   An array containing current form state values
 *
 * @return array
 *   The final form array
 */
function ss_contextual_blocks_config_form($form, &$form_state) {
  $ss_blocks = _ss_contextual_blocks_return_all_drupal_blocks();

  $form['block_table'] = array(
    '#type' => 'fieldset',
    '#title' => 'SharpSpring Contextual Blocks',
  );

  $form['block_table']['add_block'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add new SharpSpring Block'),
  );
  $form['block_table']['add_block']['new_block_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Block Name'),
    '#maxlength' => 128,
  );
  $form['block_table']['add_block']['block_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add Block',
  );

  $rows = array();

  // Format block table rows.
  foreach ($ss_blocks as $bid => $block) {
    // Build the table rows.
    $rows[$bid] = array(
      'data' => array(
        // Block name column.
        check_plain($block),
        // Operations.
        array(
          'data' => array(
            '#theme' => 'link',
            '#text' => t('delete'),
            '#path' => 'admin/config/system/sharpspring/blocks/delete_block/' . $bid,
            '#options' => array('attributes' => array(), 'html' => FALSE),
          ),
        ),
      ),
    );

  }

  // Add the table to the form.
  $form['block_table']['data_table']['table'] = array(
    '#theme' => 'table',
    '#header' => array(
      // We need two empty columns for the weigth field and the cross.
      t('Sharpspring Block'),
      t('Operations'),
    ),
    '#rows' => $rows,
    '#empty' => t('There are no blocks available.'),
    '#attributes' => array('id' => 'ss-block-order'),
  );

  $form['rules_table'] = array(
    '#type' => 'fieldset',
    '#title' => 'SharpSpring Contextual Rules',
  );

  $form['rules_table']['actions'] = array(
    '#type' => 'actions',
  );
  $form['rules_table']['actions']['new_rule'] = array(
    '#markup' => l(t('Add Rule'), 'admin/config/system/sharpspring/blocks/add'),
  );

  // Load the custom rules.
  $contents = _ss_contextual_blocks_return_all_ss_rules();

  $rows = array();
  $row_elements = array();

  // Format custom rule table rows.
  foreach ($contents as $block) {
    $name = (!empty($block['block_name'])) ? $block['block_name'] : '';
    $machine_name = (!empty($block['machine_name'])) ? $block['machine_name'] : '';
    $weight = (!empty($block['weight'])) ? $block['weight'] : 0;

    if (!empty($block['data_id'])) {
      $id = $block['data_id'];
      // Build the table rows.
      $rows[$id] = array(
        'data' => array(
          // Cell for the cross drag&drop element.
          array('class' => array('entry-cross')),
          // Weight item for the tabledrag.
          array(
            'data' => array(
              '#type' => 'weight',
              '#title' => t('Weight'),
              '#title_display' => 'invisible',
              '#default_value' => $weight,
              '#parents' => array('data_table', $id, 'weight'),
              '#attributes' => array(
                'class' => array('rule-order-weight'),
              ),
            ),
          ),
          // Name and machine name columns.
          check_plain($name),
          check_plain($machine_name),
          // Operations.
          array(
            'data' => array(
              '#theme' => 'link',
              '#text' => t('edit'),
              '#path' => 'admin/config/system/sharpspring/blocks/edit/' . $block['data_id'],
              '#options' => array('attributes' => array(), 'html' => FALSE),
            ),
          ),
          array(
            'data' => array(
              '#theme' => 'link',
              '#text' => t('delete'),
              '#path' => 'admin/config/system/sharpspring/blocks/delete/' . $block['data_id'],
              '#options' => array('attributes' => array(), 'html' => FALSE),
            ),
          ),
        ),
        'class' => array('draggable'),
      );
      // Build rows of the form elements in the table.
      $row_elements[$id] = array(
        'weight' => &$rows[$id]['data'][1]['data'],
      );
    }

  }

  // Add the table to the form.
  $form['rules_table']['data_table']['table'] = array(
    '#theme' => 'table',
    // The row form elements need to be processed and build,
    // therefore pass them as element children.
    'elements' => $row_elements,
    '#header' => array(
      // We need two empty columns for the weigth field and the cross.
      array('data' => NULL, 'colspan' => 2),
      t('Sharpspring Block'),
      t('Rule Name'),
      array('data' => t('Operations'), 'colspan' => 2),
    ),
    '#rows' => $rows,
    '#empty' => t('There are no rules available.'),
    '#attributes' => array('id' => 'rule-order'),
  );
  drupal_add_tabledrag('rule-order', 'order', 'sibling', 'rule-order-weight');

  $form['rules_table']['save_table'] = array(
    '#type' => 'submit',
    '#value' => t('Save Rule Weights'),
  );

  return $form;
}

/**
 * Validation for the contextual blocks config form.
 *
 * @param array $form
 *   An array representing the current form.
 * @param array $form_state
 *   An array of the submitted values for the form.
 */
function ss_contextual_blocks_config_form_validate($form, &$form_state) {
  $new_block = (!empty($form_state['input']['new_block_name'])) ? $form_state['input']['new_block_name'] : '';

  if (!empty($new_block)) {
    $exists = db_select('sharpspring_contextual_blocks', 'scb')
      ->fields('scb')
      ->condition('name', $new_block)
      ->execute()
      ->fetchCol();

    if (!empty($exists)) {
      // This block name already exists.
      form_set_error('new_block_name', 'This SharpSpring block name already exists.');
    }
  }

}

/**
 * Submit handler for the contextual blocks config form.
 *
 * @param array $form
 *   An array that defines the form.
 * @param array $form_state
 *   An array of submitted values for the form.
 */
function ss_contextual_blocks_config_form_submit($form, &$form_state) {
  // First save the existing block information.
  $input_vals = (!empty($form_state['input'])) ? $form_state['input'] : array();

  if (!empty($input_vals['data_table'])) {
    foreach ($input_vals['data_table'] as $data_id => $value) {
      _ss_contextual_blocks_update_rule_weight($data_id, $value);
    }
  }

  drupal_set_message('SharpSpring Contextual Blocks saved');

  // Now check the new block field.
  $new_block_name = (!empty($input_vals['new_block_name'])) ? $input_vals['new_block_name'] : '';

  if (!empty($new_block_name)) {
    $new_bid = db_insert('sharpspring_contextual_blocks')
      ->fields(array(
        'name' => $new_block_name,
      ))
      ->execute();

    drupal_set_message('SharpSpring Contextual Block "' . $new_block_name . '" Added.');
  }
}
