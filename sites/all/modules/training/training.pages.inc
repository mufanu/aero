<?php

/*
// Отображение блоков.
function training_drivers_page(){
	$vocabularies = taxonomy_get_vocabularies();
	//dpm($output);
	foreach ($vocabularies as $vocabulary) {
		$names[] = array('dsff' => $vocabulary->name);
	}
	
	$header = array(
		'title' => 'Направление',
	);
	
	return theme('table', array('header' => $header, 'rows' => $names));
}
/*
// Отображение курсов.
function training_courses_page($term){
	$output = '';
	$path = 'admin/structure/taxonomy/' . $term->vocabulary_machine_name  . '/add';
	$menu = menu_get_item($path);
	$link = l('Добавить', $menu['href'], array('query' => array('tid' => $term->tid) + drupal_get_destination()));
	// Если есть доступ, то добавляем ссылку "Добавить".
	if ($menu['access']) {
		$output .= $link;
	}
	$output .= views_embed_view('blocks', 'courses');
	return $output;
}









/*
 * Функция выводит список блоков и курсов
 * Блоки выводятся на двух страницах: training-center/learning и training-center/drivers
 * Для них $vid передается из training_menu как параметр 'page arguments'
 * Курсы отображаются внутри блоков. На самом деле страница со списком курсов это
 * страница термина "Блок" и мы отображаем дочерние термины.
 * $vid вычисляем зная $tid данного термина.
 *
function training_term_page($vid, $tid = 0){
	// Вычисляем $vid если он не указан
	if (!$vid) {
		$vid = db_select('taxonomy_term_data', 't')
		->condition('t.tid', $tid)
		->fields('t', array('vid'))
		->execute()
		->fetchField();
	}
	$output = '';
	$admin_add_term = '';
	// Если администратор, то добавляем ссылку "Добавить".
	if ((IS_ADMIN) && ($vid == TRAINING_USERS_VID)) {
		$destination = drupal_get_destination();
		// Если $id == 0, то это блоки, иначе это курсы.
		if ($tid == 0) {
			$page_title = t('Add block');
			$table_header_title = t("Block's name");
			$entity = 'block';
			$query = array('entity' => $entity);
		} else {
			$page_title = t('Add course');
			$table_header_title = t("Course's name");
			$entity = 'course';
			$query = array('entity' => $entity, 'id' => $tid);
		}
		$admin_add_term = l($page_title, ADMIN_PATH, array('query' => array('action' => 'add') + $query + $destination));
	}
	
	// Если администратор, то добавляем ссылку "Добавить".
	if ((IS_DRIVERS_ADMIN) && ($vid == TRAINING_DRIVERS_VID)) {
		$destination = drupal_get_destination();
		// Если $id == 0, то это блоки, иначе это курсы.
		if ($tid == 0) {
			$page_title = t('Add block');
			$table_header_title = t("Block's name");
			$entity = 'block';
			$query = array();
		} else {
			$page_title = t('Add course');
			$table_header_title = t("Course's name");
			$entity = 'course';
			$query = array('tid' => $tid);
		}
		$admin_add_term = l($page_title, 'admin/structure/taxonomy/drivers/add', array('query' => $query + $destination));
	}
	
	// Получаем список терминов.
	$terms = taxonomy_get_tree($vid, $tid, MAX_DEPTH);
		
	foreach ($terms as $term) {
		$term_name = l($term->name, 'taxonomy/term/' . $term->tid);
		// Если администратор
		if ((IS_ADMIN) || (IS_DRIVERS_ADMIN)) {
			// Создаем параметр запроса для каждого термина
			$query = array('entity' => $entity, 'id' => $term->tid) + $destination;
			$admin_links = l(t('Edit'), ADMIN_PATH, array('query' => array('action' => 'edit') + $query));
			$admin_links .= ' | ' . l(t('Delete'), ADMIN_PATH, array('query' => array('action' => 'delete') + $query));
			
			// Названия заголовков таблицы
			$header = array(
				'title' => array(
					'data' => $table_header_title,
					'class' => 'training-term-name'
				),
				'actions' => t('Actions'));
			
			$rows[] = array('title' => $term_name, 'signup' => $admin_links);
			
			$output = theme('table', array(
				'header' => $header,
				'rows' => $rows,
				'attributes' => array('id' => 'training-table'),
			));
		} else {
			// Атрибуты для списка, нам пока они не нужны. Поэтому указываем пустой массив.
			$type = "ul";
			$attributes = array('id' => 'block-list');
			
			// Ссылка на курсы
			$items[] = $term_name;
			
			$output = theme_item_list(array(
				'items' => $items,
				// Заголовок списка не печатаем.
				'title' => null,
				'type' => $type,
				'attributes' => $attributes
			));
		}	
	}
	
	return $admin_add_term . $output;
}

// Разрабатываемая функция для машинистов, потом нужно объеденить с общим.
function training_term_page2($vid, $tid = 0){
	
	$output = '';
	$admin_add_term = '';
	$destination = drupal_get_destination();
	$path = 'admin/structure/taxonomy/drivers/add';
	$menu = menu_get_item($path);
	$link = l('Добавить', $menu['href'], array('query' => $destination));
	// Если есть доступ, то добавляем ссылку "Добавить".
	if ($menu['access']) {
		$admin_add_term = $link;
	}
	
	// Получаем список терминов.
	$terms = taxonomy_get_tree($vid, $tid, MAX_DEPTH);
		
	foreach ($terms as $term) {
		$term_name = l($term->name, 'taxonomy/term/' . $term->tid);
		$menu = menu_get_item('taxonomy/term/' . $term->tid . '/edit');
		// Если есть доступ
		if ($menu['access']) {
			$admin_links = l(t('Edit'), $menu['href'], array('query' => $destination));			
			// Названия заголовков таблицы
			$header = array(
				'title' => array(
					'data' => 'Название',
					'class' => 'training-term-name'
				),
				'actions' => t('Actions'));
			
			$rows[] = array('title' => $term_name, 'signup' => $admin_links);
			
			$output = theme('table', array(
				'header' => $header,
				'rows' => $rows,
				'attributes' => array('id' => 'training-table'),
			));
		} else {
			// Атрибуты для списка, нам пока они не нужны. Поэтому указываем пустой массив.
			$type = "ul";
			$attributes = array('id' => 'block-list');
			
			// Ссылка на курсы
			$items[] = $term_name;
			
			$output = theme_item_list(array(
				'items' => $items,
				// Заголовок списка не печатаем.
				'title' => null,
				'type' => $type,
				'attributes' => $attributes
			));
		}	
	}
	
	return $admin_add_term . $output;
}
*/

