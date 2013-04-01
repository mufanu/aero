/**
 * @file
 * A JavaScript file for the theme.
 *
 * In order for this JavaScript to be loaded on pages, see the instructions in
 * the README.txt next to this file.
 */

// JavaScript should be made compatible with libraries other than jQuery by
// wrapping it with an "anonymous closure". See:
// - http://drupal.org/node/1446420
// - http://www.adequatelygood.com/2010/3/JavaScript-Module-Pattern-In-Depth
(function ($, Drupal, window, document, undefined) {
	Drupal.behaviors.theme_scripts = {
		attach: function (context, settings) {
			// Удаление предпоследнего элемента хлебных крошек на странице курса
			$(".view-display-id-block_lessons").parents("#content").find("a.active").parent("li").remove();
			
			// Сворачивание блоков для div'ов
			$(".view-userbook > .item-list").addClass("depth-0");
			$(".view-userbook").find(".department").click(function() {
				$user = $(this).nextAll().eq(0);
				$div = $(this).nextAll().eq(1);
				if ($div.hasClass("collapsed")) {
					$user.show().removeClass("collapsed")
					$div.show().removeClass("collapsed");
					$(this).css("background-position-y","-14px");
				} else {
					$user.hide().addClass("collapsed");
					$div.hide().addClass("collapsed");
					$(this).css("background-position-y","0");
				}
				
				return false;
			})
			
			$(".page-taxonomy-term").find("#content > p").hide();
			
			$("#tabs").tabs();
			
			$('#views-exposed-form-tasks-page').ajaxStart(function(){
        $('div.view-id-tasks').fadeTo(100, 0.4);
      });			
		}
	}


})(jQuery, Drupal, this, this.document);


