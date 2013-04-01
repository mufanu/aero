<?php

/**
 * @file
 * Administrative page callbacks for the training module.
 */

/*
 * Страница администрирования
 */
function training_admin_page() {
	return render(drupal_get_form('training_admin_page_form'));
}

/*
 * По моему можно без вышеуказанной функции напрямую вызвать эту функцию.
 */
function training_admin_page_form($form, &$form_state) {
	$vocabularies = taxonomy_get_vocabularies();
	$options = array('none' => '<none>');
	
	foreach ($vocabularies as $vocabulary) {
		$options[$vocabulary->vid] = $vocabulary->name; 
	}
	
	$form['training_users_vid'] = array(
		'#type' => 'select',
		'#options' => $options,
		'#default_value' => TRAINING_USERS_VID,
    '#description' => 'Выберите словарь таксономии, который будет использоваться для всех пользователей.',
	);
	$form['training_drivers_vid'] = array(
		'#type' => 'select',
		'#options' => $options,
		'#default_value' => TRAINING_DRIVERS_VID,
    '#description' => 'Выберите словарь таксономии, который будет использоваться для машинистов.',
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
		'#submit' => array('training_admin_page_form_submit'),
	);
	
	return $form;
}

/*
 * Submit
 */
function training_admin_page_form_submit($form, &$form_state) {
	variable_set('training_users_vid', $form_state['values']['training_users_vid']);
	variable_set('training_drivers_vid', $form_state['values']['training_drivers_vid']);
}

/*
 * Форма добавления термина
 */



/*
 * Функция добавления термина
 */
function training_admin() {
	$entity = $_GET['entity'];
	$action = $_GET['action'];
	
	if (($entity == 'block') || ($entity == 'course')) {
		if ($action == 'delete') {
			$output = training_admin_term_delete($entity, $_GET['id']);
		} else {
			$output = render(drupal_get_form('training_admin_term_form'));
		}
	} elseif ($entity == 'lesson') {
		$output = render(drupal_get_form('training_admin_lesson_form'));
	}
	
	return $output;
}

/*
 * Форма добавления и редактирования терминов
 */
function training_admin_term_form($form, &$form_state) {
	$id = 0;
	$action = $_GET['action'];
	$entity = $_GET['entity'];
		
	switch ($action) {
		case 'add':
			switch ($entity) {
				case 'block':
					drupal_set_title(t('Add block'));
					$input_label = t('Block name');
				break;
				case 'course':
					drupal_set_title(t('Add course'));
					$input_label = t('Course name');
				break;
			}
			$submit_value = t('Add');
			$name = '';
		break;
		case 'edit':
			switch ($entity) {
				case 'block':
					drupal_set_title(t('Edit block'));
					$input_label = t('Block name');
				break;
				case 'course':
					drupal_set_title(t('Edit course'));
					$input_label = t('Course name');
				break;
			}			
			$submit_value = t('Save');
			$id = $_GET['id'];
			$term = taxonomy_term_load($_GET['id']);
			$name = $term->name;
		break;
	}

	$form['wrapper'] = array(
		'#prefix' => '<div>',
		'#suffix' => '</div>',
	);
	// Название блока
	$form['wrapper']['name'] = array(
		'#type' => 'textfield',
		'#title' => $input_label,
		'#required' => TRUE,
		'#size' => 80,
		'#default_value' => $name,
	);
	// ID
	$form['wrapper']['id'] = array(
  	'#type' => 'value',
		'#value' => $id,
  );	
	// Кнопка добавить
	$form['wrapper']['submit'] = array(
		'#type' => 'submit',
		'#value' => $submit_value,
		'#submit' => array('training_admin_term_form_submit'),
	);
	
	return $form;
}

/*
 * Обработчик нажатия кнопки добавления и редактирования терминов
 */
function training_admin_term_form_submit($form, &$form_state) {
	$action = $_GET['action'];
	$entity = $_GET['entity'];
	$name = $form_state['values']['name'];
	switch ($action) {
		case 'add':
			$term = new stdClass();
			$term->name = $name;
			$term->vid = TRAINING_USERS_VID;
			switch ($entity) {
				case 'block':
					$message = t('Added block <i>@name</i>.', array('@name' => $name));
				break;
				case 'course':
					$term->parent = $_GET['id'];
					$message = t('Added course <i>@name</i>.', array('@name' => $name));
				break;
			}
		break;
		case 'edit':
			$term = taxonomy_term_load($_GET['id']);
			$term->name = $name;
			switch ($entity) {
				case 'block':
					$message = t('Updated block <i>@name</i>.', array('@name' => $name));
				break;
				case 'course':
					$message = t('Updated course <i>@name</i>.', array('@name' => $name));
				break;
			}
		break;
	}
	drupal_set_message($message, 'status');
  taxonomy_term_save($term);
}

/*
 * Функция удаления термина
 */
function training_admin_term_delete($entity, $id){
	$output = '';
	$term = taxonomy_term_load($id);
	$name = $term->name;
	switch ($entity) {
		case 'block':
			// Есть ли у данного блока курсы?
			$has_childrens = taxonomy_get_children($id);
			// Если есть курсы у данного блока, то его нельзя удалить!
			if ($has_childrens) {
				drupal_set_message(t('Нельзя удалить блок <b>@name</b> т.к. он содержит курсы!', array('@name' => $name)), 'error');
				drupal_goto('training-center/learning');
			} else {
			// Иначе ждем подтверждения удаления
				$output = t('<p>Вы действительно хотите удалить блок <b>@name</b>?</p>', array('@name' => $name));
				$output .= l(t('Cancel'), 'training-center/learning');
				$output .= render(drupal_get_form('training_admin_term_delete_form'));
			}
		break;
		case 'course':
			// Блок курса
			$block_id = training_get_parent($id);
			// Есть ли у данного курса есть уроки?
			$has_lessons = db_select('taxonomy_index', 't')
			->condition('t.tid', $id)
			->fields('t', array('nid'))
			->execute()
			->fetchField();
			// Если есть уроки у данного курса, то его нельзя удалить!
			if ($has_lessons) {
				drupal_set_message(t('Нельзя удалить курс <b>@name</b>, т.к. он содержит уроки!', array('@name' => $name)), 'error');
				drupal_goto('taxonomy/term/' . $block_id);		
			} else {
				$output .= t('<p>Вы действительно хотите удалить курс <b>@name</b>?</p>', array('@name' => $name));
				$output .= render(drupal_get_form('training_admin_term_delete_form'));
			}
		break;
	}
	
	return $output;
}

/*
 * Форма подтверждения удаления термина
 */
function training_admin_term_delete_form($form, &$form_state) {
	$form['wrapper']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
		'#submit' => array('training_admin_term_delete_form_submit'),
  );
	
	return $form;
}

/*
 * Обработчик удаления термина
 */
function training_admin_term_delete_form_submit() {
	$entity = $_GET['entity'];
	$id = $_GET['id'];
	$term = taxonomy_term_load($id);
	$name = $term->name;
	taxonomy_term_delete($id);
	switch ($entity) {
		case 'block':
		$message = t('Deleted block <i>@name</i>.', array('@name' => $name));
		break;
		case 'course':
			$message = t('Deleted course <i>@name</i>.', array('@name' => $name));
		break;
	}
	
	drupal_set_message($message, 'status');
}

/*
 * Форма добавления урока
 */
function training_admin_lesson_form($form, &$form_state) {
	$action = $_GET['action'];
	$id = $_GET['id'];
	switch ($action) {
		case 'add':
			$title_value = '';
			$body = '';
			$submit_title = t('Add');
		break;
		case 'edit':
			$node = node_load($id);
			$title_value = $node->title;
			$body = $node->body['ru'][0]['value'];
			$submit_title = t('Save');
		break;
	}
	$form['wrapper'] = array(
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );
	// Название курса
  $form['wrapper']['title'] = array(
  	'#type' => 'textfield',
  	'#title' => t('Название урока'),
		'#required' => TRUE,
    '#size' => 80,
		'#default_value' => $title_value,
		'#description' => t('Название урока нужно вводить в формате <b>Урок 1. Название урока</b>.'),
  );
	// Тело урока
	$form['wrapper']['body'] = array(
		'#type' => 'text_format',
		'#format' => 'full_html',
		'#default_value' => $body,
	);
	// Отправляем скрыто id термина, т.е. курса
	$form['wrapper']['id'] = array(
  	'#type' => 'value',
		'#value' => $_GET['id'],
  );	
	// Кнопка добавить
  $form['wrapper']['submit'] = array(
    '#type' => 'submit',
    '#value' => $submit_title,
		'#submit' => array('training_admin_lesson_form_submit'),
  );
	
	return $form;
}

/*
 * Обработчик добавления и редактирования урока.
 */
function training_admin_lesson_form_submit($form, &$form_state) {
	global $user;
	$action = $_GET['action'];
	$node_type = 'lesson';
	$node_title = $form_state['values']['title'];
	switch ($action) {
		case 'add':
			$node = new stdClass();
			$node->type = $node_type;
			node_object_prepare($node);
			$node->uid = $user->uid;
			$node->language = 'ru';
			$node->field_course['und'][0]['target_id'] = $form_state['values']['id'];
		break;
		case 'edit':
			$node = node_load($form_state['values']['id']);
		break;
	}
	$node->title = $form_state['values']['title'];
	$node->body[$node->language][0] = $form_state['values']['body'];
	
	node_save($node);
}


/*
 * Форма добавления и редактирования теста
 */
function training_quiz_form($form, &$form_state) {
	$lesson_id = $_GET['nid'];
	$quiz_id = db_select('field_data_field_lesson', 'f')
	->condition('f.field_lesson_target_id', $lesson_id)
	->fields('f', array('entity_id'))
	->execute()
	->fetchField();
	dpm($quiz_id);
	$action = $_GET['action'];
	
	switch ($action) {
		case "add":
		$submit_value = t('Add');
		$title = '';
		break;
		case "edit":
		$submit_value = t('Save');
		$quiz = node_load($quiz_id);
		$title = $quiz->title;
		//dpm($quiz);
		break;
	}
	
	$form['wrapper'] = array(
			'#prefix' => '<div>',
			'#suffix' => '</div>',
		);
		// Название курса
		$form['wrapper']['title'] = array(
			'#type' => 'textfield',
			'#title' => t('Quiz name'),
			'#required' => TRUE,
			'#size' => 80,
			'#default_value' => $title,
		);
		$form['wrapper']['lesson'] = array(
			'#type' => 'value',
			'#value' => $lesson_id,
		);
		$form['wrapper']['quiz_id'] = array(
			'#type' => 'value',
			'#value' => $quiz_id,
		);
		// Кнопка добавить
		$form['wrapper']['submit'] = array(
			'#type' => 'submit',
			'#value' => $submit_value,
			'#submit' => array('training_quiz_form_submit'),
		);
	
	return $form;
}

/*
 * Функция добавления теста
 */
function training_quiz_add() {
	return render(drupal_get_form('training_quiz_form'));
}

function training_quiz_edit() {
	return render(drupal_get_form('training_quiz_form'));
}

function training_quiz_form_submit($form, &$form_state) {
	$action = $_GET['action'];
	switch ($action) {
		case "add":
			global $user;
			$node = new stdClass();
			$node->type = 'quiz';
			node_object_prepare($node);
			$node->title = $form_state['values']['title'];
			$node->uid = $user->uid;
			$node->language = 'ru';
			$node->field_lesson['und'][0]['target_id'] = $form_state['values']['lesson'];
		break;
		case "edit":
			$node = node_load($form_state['values']['quiz_id']);
			$node->title = $form_state['values']['title'];
			dpm($node);
		break;
	}
	
	node_save($node);
}

/*
 * Получаем имя термина, т.е. названия блока или курса. Удалить
 */
function training_get_taxonomy_name($tid) {
	$name = db_select('taxonomy_term_data', 't')
	->condition('t.tid', $tid)
	->fields('t', array('name'))
	->execute()
	->fetchField();
	
	return $name;
}

/*
 * Получает родителя термина.
 * Возвращает:
 * tid - если есть родитель
 * 0 - если нет
 */
function training_get_parent($tid) {
	$parent = db_select('taxonomy_term_hierarchy', 't')
	->condition('t.tid', $tid)
	->fields('t', array('parent'))
	->execute()
	->fetchField();
	
	return $parent;
}

function training_form_quiz_questions_form_alter(&$form, &$form_state, $form_id) {
  if ($form_state['rebuild']) {
    // Save the active filters in $_SESSION
    $filters = $form_state['values']['browser']['table']['header']['filters'];
    _quiz_questions_store_filters($filters);
  }

  $types = _quiz_get_question_types();

  _quiz_add_fields_for_creating_questions($form, $types, $quiz);

  // Display questions in this quiz.
  $form['question_list'] = array(
    '#type' => 'fieldset',
    '#title' => t('Questions in this quiz'),
    '#theme' => 'question_selection_table',
    '#collapsible' => TRUE,
    '#attributes' => array('id' => 'mq-fieldset'),
    'question_status' => array('#tree' => TRUE),
  );

  $form['#attached']['js'] = array(drupal_get_path('module', 'quiz') .'/theme/quiz_question_browser.js');

  // Add randomization settings if this quiz allows randomized questions
  _quiz_add_fields_for_random_quiz($form, $quiz);

  // Build up a list of questions
  $questions_to_add = array();

  // We use $form_state[post] to avoid validation failures when questions are added using AJAX
  if (isset($form_state['post']['weights'])) {
    $questions = _quiz_get_questions_from_form_state($form_state, $questions_to_add);
  }
  else {
    // We are coming in fresh and fetches the questions currently on the quiz from the database...
    $include_random = $quiz->randomization == 2;
    $questions = quiz_get_questions($quiz->nid, $quiz->vid, TRUE, FALSE, FALSE, $include_random);
  }

  if (empty($questions)) {
    $form['question_list']['no_questions'] = array(
      '#markup' => '<div id = "no-questions">' . t('There are currently no questions in this quiz. Assign existing questions by using the question browser below. You can also use the links above to create new questions.') . '</div>',
    );
  }

  // We add the browser and allows the browser to give us information on what questions are displayed in the browser...
  $hidden_questions = array();
  $form['question_list']['browser'] = _quiz_question_browser_form($hidden_questions, $questions_to_add, $form_state, $quiz, $types);
  // We add the questions from the browser as hidden question rows in the question list. Doing this we can have
  // the question show up in the question list instantly when a question is chosen in the browser(using js).
  _quiz_add_hidden_questions($questions, $hidden_questions, $form_state, $quiz);

  // We add the questions to the form array
  _quiz_add_questions_to_form($form, $questions, $quiz, $types);

  // Show the number of questions in the table header.
  $always_count = 0;
  foreach ($form['question_list']['stayers'] as $stayer) {
    if ($stayer['#default_value'] === 1) {
      $always_count++;
    }
  }
  $form['question_list']['#title'] .= ' (' . $always_count . ')';

  // Give the user the option to create a new revision of the quiz
  _quiz_add_revision_checkbox($form, $quiz);

  // Timestamp is needed to avoid multiple users editing the same quiz at the same time.
  $form['timestamp'] = array('#type' => 'hidden', '#default_value' => REQUEST_TIME);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#submit' => array('quiz_questions_form_submit'),
  );
	dpm($form);
  return $form;
}

function training_quiz_add_fields_for_creating_questions(&$form, &$types, &$quiz) {
  // Display links to create other questions.
  $form['additional_questions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create new question'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $url_query = drupal_get_destination();
  $url_query['quiz_nid'] = $quiz->nid;
  $url_query['quiz_vid'] = $quiz->vid;
  foreach ($types as $type => $info) {
    $url_type = str_replace('_', '-', $type);
    $options = array(
      'attributes' => array('title' => t('Create @name', array('@name' => $info['name']))),
      'query' => $url_query,
    );
    $form['additional_questions'][$type] = array(
      '#markup' => '<div class="add-questions">' . l($info['name'], "node/add/$url_type", $options) . '</div>',
    );
  }
}