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
  // Replace Windows line endings.
  $raw_segments = str_replace("\n\r", "\n", variable_get('sharpspring_segment_values'));
  $segments = explode("\n", $raw_segments);

  $ss_blocks = _ss_contextual_blocks_return_all_drupal_blocks();
  $contents = _ss_contextual_blocks_return_all_ss_blocks();

  // Get a list of all active blocks.
  global $theme;
  $blocks = db_select('block', 'b')
    ->fields('b', array('bid', 'delta'))
    ->condition('theme', $theme)
    ->execute()
    ->fetchAllKeyed();

  foreach ($ss_blocks as $bid => $block_name) {
    $form['ss_block_' . $bid] = array(
      '#type' => 'fieldset',
      '#title' => $block_name,
    );

    $n = 0;
    foreach ($segments as $segment) {
      $default_val = 0;
      if (!empty($contents[$bid]) && !empty($contents[$bid][trim($segment)])) {
        $default_val = $contents[$bid][trim($segment)];
      }

      $form['ss_block_' . $bid]['segment_' . $bid . ';' . trim($segment)] = array(
        '#type' => 'select',
        '#title' => trim($segment),
        '#options' => $blocks,
        '#empty_value' => '',
        '#default_value' => $default_val,
      );

      $n++;
    }

  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for the contextual blocks config form.
 *
 * @param array $form
 *   An array that defines the form.
 * @param array $form_state
 *   An array of values for the form.
 */
function ss_contextual_blocks_config_form_submit($form, &$form_state) {
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

}