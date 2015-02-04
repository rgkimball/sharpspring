/**
 * @file
 * Contextual Block handler javascript.
 *
 * @author: Mojiferous
 */

/**
 * Replaces block contextual block content with segment-specific blocks.
 *
 * @param resp
 *   response object returned from Sharpspring
 */
function ssContextualBlock(resp) {
  (function ($, Drupal, window, document) {
    if (Drupal.settings.sharpspring.segmentationField && resp[Drupal.settings.sharpspring.segmentationField]) {
      var segmentVal = resp[Drupal.settings.sharpspring.segmentationField];

      // Loop through each contextual block.
      $('.ss-contextual-wrapper').each(function() {
        var this_bid = $(this).attr('ssbid');

        // Perform an ajax call to get the block contents
        var request = $.ajax({
          type: "POST",
          url: "sharpspring/contextual/block_callback/" + this_bid + "/" + segmentVal,
          ss_wrapper: this,
          success: function(data, text_status) {
            $(this.ss_wrapper).html(data);
          }
        });
      });
    }
  })(jQuery, Drupal, this, this.document);
}
