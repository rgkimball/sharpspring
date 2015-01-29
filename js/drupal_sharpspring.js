/**
 * @file
 * Sharpspring specific javascript
 *
 * @author: Mojiferous
 */

/**
 * Handles sharpspring response
 *
 * @param resp
 *   response object returned from Sharpspring
 */
function ssDrupalCallback(resp) {
  if (resp && resp.contact) {

    (function ($, Drupal, window, document) {
      // This allows additional modules to define their own JS handler functions.
      if (Drupal.settings.sharpspring.calledFuncs) {
        for(var n = 0; n < Drupal.settings.sharpspring.calledFuncs.length; n++) {
          var fn = window[Drupal.settings.sharpspring.calledFuncs[n]];

          if (typeof fn === 'function') {
            // SharpSpring module handler functions should accept
            // a response object as a variable.
            fn(resp.contact);
          }
        }
      }
    })(jQuery, Drupal, this, this.document);
  }
}
