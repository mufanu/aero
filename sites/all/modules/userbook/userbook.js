(function ($) {

Drupal.behaviors.userbook = {
  attach: function (context) {
		$("#departments").find("a.collapsed").live("click", function() {
				$(this).removeClass("collapsed");
				$(this).addClass("expanded");
				// Передаем с методом get адрес ссылки
				var href = $(this).attr('href');
				var output = $(this).parent("li");
				$(output).append("<span class='status'>Загружаю...</span>");
				$.get(Drupal.settings.basePath + 'userbook/get_children', {url:href} , function(response) {
					// Сохраняем ответ в переменной
					$(output).find(".status").remove();
					$(output).append(response.data);
				});
				return false;
		});
		$("#departments").find("a.expanded").live("click", function() {
			$(this).addClass("hidden");
			$(this).removeClass("expanded");
			$(this).nextAll().hide();
			return false;
		});
		$("#departments").find("a.hidden").live("click", function() {
			$(this).addClass("expanded");
			$(this).removeClass("hidden");
			$(this).nextAll().show();
			return false;
		});
	}
};
})(jQuery);