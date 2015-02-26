<?php
/**
 * @file
 * Admin form handlers for the SharpSpring contextual blocks module.
 */

/**
 * Defines the block config form.
 *
 * @param array $form
 *   An array containing form values.
 * @param array $form_state
 *   An array containing current form state values.
 *
 * @return array
 *   The final form array
 */
function sharpspring_contextual_blocks_config_form($form, &$form_state) {
  $ss_blocks = _sharpspring_contextual_blocks_return_all_drupal_blocks();

  $form['block_table'] = array(
    '#type' => 'fieldset',
    '#title' => 'SharpSpring Contextual Blocks',
  );

  $form['block_table']['actions'] = array(
    '#type' => 'actions',
  );
  $form['block_table']['actions']['new_block'] = array(
    '#markup' => l(t('Add Block'), 'admin/config/system/sharpspring/blocks/add_block',array('query' => array('destination' => 'admin/config/system/sharpspring/blocks')))
  );

  $rows = array();

  // Format block table rows.
  foreach ($ss_blocks as $bid => $block) {
    $name = (!empty($block->name)) ? $block->name : '';
    $description = (!empty($block->description)) ? $block->description : '';

    // Build the table rows.
    $delta = 'sharpspring_contextual_block_' . $bid;
    $rows[$bid] = array(
      'data' => array(
        // Block name column.
        check_plain($name),
        check_plain($description),
        // Operations.
        array(
          'data' => array(
            '#theme' => 'link',
            '#text' => t('edit'),
            '#path' => 'admin/structure/block/manage/sharpspring_contextual_blocks/' . $delta . '/configure',
            '#options' => array('attributes' => array(), 'html' => FALSE, 'query' => array('destination' => 'admin/config/system/sharpspring/blocks')),
          ),
        ),
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
      t('Sharpspring Block Name'),
      t('Description'),
      array('data' => t('Operations'), 'colspan' => 2),
    ),
    '#rows' => $rows,
    '#empty' => t('There are no blocks available.'),
    '#attributes' => array('id' => 'ss-block-order'),
  );

  return $form;
}

/**
 * Defines the block rules config form.
 *
 * @param array $form
 *   An array containing form values.
 * @param array $form_state
 *   An array containing current form state values.
 *
 * @return array
 *   The final form array
 */
function sharpspring_contextual_blocks_rules_config_form($form, &$form_state) {

  $form['rules_table'] = array(
    '#type' => 'fieldset',
    '#title' => 'SharpSpring Contextual Rules',
  );

  $form['rules_table']['actions'] = array(
    '#type' => 'actions',
  );
  $form['rules_table']['actions']['new_rule'] = array(
    '#markup' => l(t('Add Rule'), 'admin/config/system/sharpspring/blocks/add_rule', array('query' => array('destination' => 'admin/config/system/sharpspring/blocks/rules'))),
  );

  // Load the custom rules.
  $contents = _sharpspring_contextual_blocks_return_all_sharpspring_rules();

  $rows = array();
  $row_elements = array();

  // Format custom rule table rows.
  foreach ($contents as $rule) {
    $name = (!empty($rule['name'])) ? $rule['name'] : '';
    $description = (!empty($rule['description'])) ? $rule['description'] : '';
    $weight = (!empty($rule['weight'])) ? $rule['weight'] : 0;

    if (!empty($rule['rule_id'])) {
      $id = $rule['rule_id'];
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
          check_plain($description),
          // Operations.
          array(
            'data' => array(
              '#theme' => 'link',
              '#text' => t('edit'),
              '#path' => 'admin/config/system/sharpspring/blocks/edit_rule/' . $rule['rule_id'],
              '#options' => array('attributes' => array(), 'html' => FALSE, 'query' => array('destination' => $_SERVER['REDIRECT_URL'])),
            ),
          ),
          array(
            'data' => array(
              '#theme' => 'link',
              '#text' => t('delete'),
              '#path' => 'admin/config/system/sharpspring/blocks/delete_rule/' . $rule['rule_id'],
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
      t('Rule Name'),
      t('Description'),
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
 * Submit handler for the contextual blocks config form.
 *
 * @param array $form
 *   An array that defines the form.
 * @param array $form_state
 *   An array of submitted values for the form.
 */
function sharpspring_contextual_blocks_config_form_submit($form, &$form_state) {
  // First save the existing block information.
  $input_vals = (!empty($form_state['values'])) ? $form_state['values'] : array();

  if (!empty($input_vals['data_table'])) {
    foreach ($input_vals['data_table'] as $data_id => $value) {
      _sharpspring_contextual_blocks_update_rule_weight($data_id, $value);
    }
  }

  drupal_set_message(t('SharpSpring Contextual Blocks saved'));
}
