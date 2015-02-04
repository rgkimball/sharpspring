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
  $form['add_block'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add new SharpSpring Block'),
  );

  $form['add_block']['new_block_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Block Name'),
    '#maxlength' => 128,
  );

  $form['add_block']['block_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add Block',
  );

  // Replace Windows line endings.
  $raw_segments = str_replace("\n\r", "\n", variable_get('sharpspring_segment_values'));
  $segments = explode("\n", $raw_segments);

  // $ss_blocks = _ss_contextual_blocks_return_all_drupal_blocks();
  $contents = _ss_contextual_blocks_return_all_ss_blocks();

  $header = array(
    array('data' => 'SharpSpring Block', 'field' => 'name', 'sort' => 'ASC'),
    array('data' => 'Segmentation Name', 'field' => 'segment', 'sort' => 'ASC'),
    array('data' => 'Operations'),
  );

  $rows = array();
  foreach ($contents as $block) {
    $name = (!empty($block['name'])) ? $block['name'] : '';
    $segment = (!empty($block['segment']['name'])) ? $block['segment']['name'] : '';

    if (!empty($block['data_id'])) {
      $rows[] = array(
        $name,
        $segment,
        '<ul class="links inline"><li class="edit first"><a href="/sharpspring/contextual/edit/' . $block['data_id'] . '">edit</a></li><li class="delete last"><a href="/sharpspring/contextual/delete/' . $block['data_id'] . '">delete</a></li></ul>',
      );
    }

  }

  $form['block_table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
  );

  // Get a list of all active blocks.
  // global $theme;
  // $blocks = db_select('block', 'b')
  // ->fields('b', array('bid', 'delta'))
  // ->condition('theme', $theme)
  // ->execute()
  // ->fetchAllKeyed();

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

  foreach ($input_vals as $key => $val) {
    if (strpos($key, 'segment_') === 0) {
      $bid_seg = explode(';', str_replace('segment_', '', $key));

      if (count($bid_seg) > 1) {
        $bid = $bid_seg[0];
        $segment = trim($bid_seg[1]);

        _ss_contextual_blocks_upsert_block($bid, $segment, $val);

      }

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
