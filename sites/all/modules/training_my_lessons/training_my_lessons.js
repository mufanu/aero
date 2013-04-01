(function ($) {

Drupal.behaviors.training_my_lessons = {
  attach: function (context) {
		var block_item = $("#training-my-lessons").find("li.training-block-item");
		var course_item = $("#training-my-lessons").find("li.training-course-item");
		
		$(block_item).find("span.block-title").bind("click", function() {
			var parent = $(this).parent("li");
				if ($(parent).hasClass("collapsed")) {
					$(parent).removeClass("collapsed");
					$(parent).addClass("expanded");
					$(parent).find("ul.training-block-item").show();
				} else {
					$(parent).removeClass("expanded");
					$(parent).addClass("collapsed");
					$(parent).find("ul.training-block-item").hide();
				}
				return false;
		});
		
		$(course_item).find("span.course-title").bind("click", function() {
			var parent = $(this).parent("li");
				if ($(parent).hasClass("collapsed")) {
					$(parent).removeClass("collapsed");
					$(parent).addClass("expanded");
					$(parent).find("ul.training-course-item").show();
				} else {
					$(parent).removeClass("expanded");
					$(parent).addClass("collapsed");
					$(parent).find("ul.training-course-item").hide();
				}
				return false;
		});
	}
};
})(jQuery);