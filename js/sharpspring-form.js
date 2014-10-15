/**
 * @file
 * Adds SharpSpring webform tracking.
 *
 * @author: rgkimball
 */

Drupal.behaviors.sharpSpringForm = {
  attach: function(settings, context) {
    var forms = Drupal.settings.sharpspring.forms;

    if(typeof forms == 'object') {
      var __ss_noform = __ss_noform || [];
      // Push form id data to SharpSpring.
      for (var i in forms) {
        __ss_noform.push(['baseURI', forms[i]['base_uri']]);
        __ss_noform.push(['endpoint', forms[i]['endpoint']]);
      }
    }
  }
};
