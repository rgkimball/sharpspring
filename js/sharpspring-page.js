/**
 * @file
 * Adds SharpSpring page tracking.
 *
 * @author: rgkimball
 */

Drupal.behaviors.sharpSpring = {
  attach: function(settings, context) {
    var domain = Drupal.settings.sharpspring.domain,
      id = Drupal.settings.sharpspring.id;

    var _ss = _ss || [];
    _ss.push(['_setDomain', 'https://' + domain + '/net']);
    _ss.push(['_setAccount', id]);
    _ss.push(['_trackPageView']);
    (function() {
      var ss = document.createElement('script');
      ss.type = 'text/javascript'; ss.async = true;
      ss.src = ('https:' == document.location.protocol ? 'https://' : 'http://') + domain + '/client/ss.js?ver=1.1.1';
      var scr = document.getElementsByTagName('script')[0];
      scr.parentNode.insertBefore(ss, scr);
    })();
  }
};
