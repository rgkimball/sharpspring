<?php
/**
 * @file
 * Handles Drupal blocks for SharpSpring contextual blocks
 *
 * @author: Mojiferous
 */

/**
 * Implements hook_block_info().
 */
function ss_contextual_blocks_block_info() {
  $blocks = array();
  $ss_blocks = _ss_contextual_blocks_return_all_drupal_blocks();

  foreach ($ss_blocks as $bid => $block) {
    $blocks['ss_contextual_block_' . $bid] = array(
      'info' => (!empty($block->name)) ? 'SharpSpring: ' . $block->name : 'SharpSpring block ' . $bid,
      'cache' => DRUPAL_CACHE_PER_ROLE,
    );
  }

  return $blocks;
}

/**
 * Implements hook_block_view().
 */
function ss_contextual_blocks_block_view($delta = '') {
  $block = array();

  if (strpos($delta, 'ss_contextual_block_') === 0) {
    // This is a SharpSpring contextual block.

    $bid = str_replace('ss_contextual_block_', '', $delta);

    $block['subject'] = '';
    $block['content'] = _ss_contextual_blocks_return_block_contents($bid);
  }

  return $block;
}

/**
 * Returns the contents of a SharpSpring contextual block.
 *
 * @param int $bid
 *   An integer representing the SharpSpring block.
 *
 * @return string
 *   The contents of the block.
 */
function _ss_contextual_blocks_return_block_contents($bid) {
  // Add javascript to handle Sharpspring callback.
  $path = drupal_get_path('module', 'ss_contextual_blocks');

  $has_added_block_js = &drupal_static('ss_c_block_has_added_js');
  if (empty($has_added_block_js)) {
    drupal_add_js($path . '/js/ss_contextual_blocks.js');
    drupal_add_js(array('sharpspring' => array('calledFuncs' => array('ssContextualBlock'))), 'setting');
    $has_added_block_js = TRUE;
  }

  return '<div id="ss-contextual-block-' . $bid . '" class="ss-contextual-wrapper" ssbid="' . $bid . '"></div>';
}
