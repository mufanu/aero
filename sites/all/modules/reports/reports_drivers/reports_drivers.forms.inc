<?php

/**
 * Форма для отчетов по статистике изучения документа.
 * @param $form
 * @param $form_state
 */
function reports_drivers_stat_form($form, &$form_state) {
  // При первой ajax отправке формы $_GET['direction'] будет пуст и поэтому берем данные с
  //$form_state['values']['fields']['direction']
  if (isset($_GET['direction'])) {
    $direction_id = $_GET['direction'];
  }
  else {
    $direction_id = isset($form_state['values']['fields']['direction']) ? $form_state['values']['fields']['direction']
      : NULL;
  }

  // При первой ajax отправке формы $_GET['doc'] будет пуст и поэтому берем данные с
  // $form_state['values']['fields']['doc']
  if (isset($_GET['doc'])) {
    $doc_id = $_GET['doc'];
  }
  else {
    $doc_id = isset($form_state['values']['fields']['doc']) ? $form_state['values']['fields']['doc'] : NULL;
  }
  // Дата начала отчета
  if (isset($_GET['start_date'])) {
    $start_date = $_GET['start_date'];
  }
  // Дата окончания отчета
  if (isset($_GET['end_date'])) {
    $end_date = $_GET['end_date'];
  }

  // Враппер для селектов
  $form['fields'] = array(
    '#tree' => TRUE,
    '#prefix' => '<div id="fields">',
    '#suffix' => '</div>',
  );

  $directions = array('' => '- Выберите -');
  $directions += reports_drivers_directions();
  $form['fields']['direction'] = array(
    '#type' => 'select',
    '#title' => t('Выберите направление'),
    '#options' => $directions,
    '#default_value' => $direction_id,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'reports_drivers_ajax_callback',
      'wrapper' => 'fields',
      'method' => 'replace',
    ),
  );

  if (is_numeric($direction_id)) {
    $docs = array('' => '- Выберите -');
    $docs += reports_drivers_direction_docs($direction_id);
    $form['fields']['doc'] = array(
      '#type' => 'select',
      '#title' => t('Выберите документ'),
      '#options' => $docs,
      '#default_value' => $doc_id,
      '#required' => TRUE,
      '#ajax' => array(
        'callback' => 'reports_drivers_ajax_callback',
        'wrapper' => 'fields',
        'method' => 'replace',
      ),
    );
  }

  if (is_numeric($direction_id) && is_numeric($doc_id)) {
    $query = db_select('node', 'n')
      ->condition('nid', $doc_id)
      ->fields('n')
      ->execute()
      ->fetchObject();

    $doc_created = $query->created;
    $doc_name = $query->title;

    $form['fields']['start_date'] = array(
      '#type' => 'date_popup',
      '#title' => t('Start date'),
      '#default_value' => date(REPORTS_DATE_FORMAT, $doc_created),
      '#date_format' => REPORTS_DATE_FORMAT,
      '#date_label_position' => 'within',
      '#date_increment' => 15,
      '#date_year_range' => '-3:+3',
      '#datepicker_options' => array(),
      '#required' => TRUE,
    );

    $form['fields']['end_date'] = array(
      '#type' => 'date_popup',
      '#title' => t('End date'),
      '#default_value' => date(REPORTS_DATE_FORMAT, time()),
      '#date_format' => REPORTS_DATE_FORMAT,
      '#date_label_position' => 'within',
      '#date_increment' => 15,
      '#date_year_range' => '-3:+3',
      '#datepicker_options' => array(),
      '#required' => TRUE,
    );
    // Заголовок таблицы
    $caption = 'Отчет по изучению документа "' . $doc_name . '" за период ' . date(REPORTS_DATE_FORMAT, $start_date) . ' по ' . date(
        REPORTS_DATE_FORMAT, $end_date);
    $form['fields']['caption'] = array(
      '#type' => 'value',
      '#value' => $caption,
    );
  }

  // Заголовок столбцов
  $headers = array(
    '№',
    'ФИО машиниста',
    'Дата подписки',
    'Дата сдачи теста',
    'Количество попыток',
    'Количество дней просрочки',
  );
  $form['headers'] = array(
    '#type' => 'value',
    '#value' => $headers,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Сформировать отчет'),
  );
  if (is_numeric($direction_id) && is_numeric($doc_id)) {
    $form['export'] = array(
      '#type' => 'submit',
      '#value' => t('Выгрузить в Excel'),
      '#submit' => array('reports_drivers_stat_export'),
    );
  }

  if (isset($_GET['direction'])) {
    $form['table'] = array(
      '#markup' => reports_drivers_stat_preview($caption, $headers, $doc_id, $start_date, $end_date),
    );
  }

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function reports_drivers_stat_form_submit($form, &$form_state) {
  $start_date = strtotime($form_state['values']['fields']['start_date']);
  $end_date = strtotime($form_state['values']['fields']['end_date']);
  $form_state['redirect'] = array(
    'reports/drivers/stat',
    array(
      'query' => array(
        'direction' => $form_state['values']['fields']['direction'],
        'doc' => $form_state['values']['fields']['doc'],
        'start_date' => $start_date,
        'end_date' => $end_date,
      ),
    ),
  );
}

/**
 * AJAX callback
 */
function reports_drivers_ajax_callback($form, $form_state) {
  return $form['fields'];
}

/**
 * Форма для персональных отчетов
 * @param $form
 * @param $form_state
 */
function reports_drivers_personal_form($form, &$form_state) {

  if (isset($_GET['start_date'])) {
    $start_date = $_GET['start_date'];
  }
  else {
    $start_date = time();
  }

  if (isset($_GET['end_date'])) {
    $end_date = $_GET['end_date'];
  }
  else {
    $end_date = time();
  }

  if (isset($_GET['user'])) {
    $user = $_GET['user'];
  }
  else {
    $user = '';
  }
  // Заголовок таблицы
  $caption = t('Персональный отчет машиниста по изучению документов на ' . date(REPORTS_DATE_FORMAT, time()));
  // Заголовки столбцов
  $headers = array(
    '№',
    'Название документа',
    'Дата подписки',
    'Дата сдачи теста',
    'Количество попыток',
    'Количество дней просрочки',
  );

  $form['caption'] = array(
    '#type' => 'value',
    '#value' => $caption,
  );

  $form['headers'] = array(
    '#type' => 'value',
    '#value' => $headers,
  );

  $form['standard']['first-col']['fullname'] = array(
    '#type' => 'textfield',
    '#title' => t('Укажите ФИО'),
    '#size' => 100,
    '#default_value' => $user,
    '#autocomplete_path' => 'reports/drivers/autocomplete',
    '#required' => TRUE,
  );

  $form['start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start date'),
    '#default_value' => date(REPORTS_DATE_FORMAT, $start_date),
    '#date_format' => REPORTS_DATE_FORMAT,
    '#date_label_position' => 'within',
    '#date_increment' => 15,
    '#date_year_range' => '-3:+3',
    '#datepicker_options' => array(),
  );

  $form['end_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('End date'),
    '#default_value' => date(REPORTS_DATE_FORMAT, $end_date),
    '#date_format' => REPORTS_DATE_FORMAT,
    '#date_label_position' => 'within',
    '#date_increment' => 15,
    '#date_year_range' => '-3:+3',
    '#datepicker_options' => array(),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Сформировать отчет'),
  );


  $form['export'] = array(
    '#type' => 'submit',
    '#value' => t('Выгрузить в Excel'),
    '#submit' => array('reports_drivers_personal_export'),
  );


  if (isset($_GET['start_date'])) {
    $form['table'] = array(
      '#markup' => reports_drivers_personal_preview($caption, $headers, $user, $start_date, $end_date),
    );
  }

  return $form;
}

/**
 * Проверка введенных данных поля ФИО
 * @param $form
 * @param $form_state
 *
 * @return bool
 */
function reports_drivers_personal_form_validate($form, &$form_state) {
  $string = $form_state['values']['fullname'];
  if ($string) {
    $query = db_select('field_data_field_user_full_name', 'ufn')
      ->condition('ufn.field_user_full_name_value', $string)
      ->fields('ufn', array('field_user_full_name_value'))
      ->execute()
      ->fetchField();

    if ($query) {
      return TRUE;
    }
    else {
      drupal_set_message('Вы неверно указали ФИО сотрудника', 'error');
    }
  }
}

/**
 * @param $form
 * @param $form_state
 */
function reports_drivers_personal_form_submit($form, &$form_state) {
  $start_date = strtotime($form_state['values']['start_date']);
  $end_date = strtotime($form_state['values']['end_date']);

  $form_state['redirect'] = array(
    'reports/drivers/personal',
    array(
      'query' => array(
        'user' => $form_state['values']['fullname'],
        'start_date' => $start_date,
        'end_date' => $end_date,
      ),
    ),
  );
}

/**
 * Форма для оперативных отчетов
 * @param $form
 * @param $form_state
 */
function reports_drivers_daily_form($form, &$form_state) {

  if (isset($_GET['date'])) {
    $date = $_GET['date'];
  }
  else {
    $date = time();
  }

  $caption = t('Оперативный отчет по инструктажу за сутки <br > Дата отчета:' . $date);
  $form['caption'] = array(
    '#type' => 'value',
    '#value' => $caption,
  );
  $headers = array(
    '№',
    'ФИО',
    'Название документа',
    'Дата подписки',
    'Дата сдачи теста',
  );
  $form['headers'] = array(
    '#type' => 'value',
    '#value' => $headers,
  );


  $form['date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Date'),
    '#default_value' => date(REPORTS_DATE_FORMAT, $date),
    '#date_format' => REPORTS_DATE_FORMAT,
    '#date_label_position' => 'within',
    '#date_increment' => 15,
    '#date_year_range' => '-3:+3',
    '#datepicker_options' => array(),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Сформировать отчет'),
  );

  if (isset($_GET['date'])) {
    $form['table'] = array(
      '#markup' => reports_drivers_daily_preview($caption, $headers, $date),
    );
  }

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function reports_drivers_daily_form_submit($form, &$form_state) {
  $date = strtotime($form_state['values']['date']);
  $form_state['redirect'] = array(
    'reports/drivers/daily',
    array(
      'query' => array(
        'date' => $date,
      ),
    ),
  );
}
