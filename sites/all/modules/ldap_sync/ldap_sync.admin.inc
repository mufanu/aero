<?php

/*
 * Административный файл
 */

function ldap_sync_index() {
	return render(drupal_get_form('ldap_sync_query_form'));
}

/*
 * Форма выбора и сохранения запроса к LDAP.
 */
function ldap_sync_query_form($form, &$form_state) {
	
	$current = variable_get(CURRENT_USERS);
	$fired = variable_get(FIRED_USERS);
	
	$ldap_queries = ldap_query_get_queries(0, 'all');
	
	$options = array('none' => '<none>');
	
	foreach ($ldap_queries as $key => $ldap_query) {
		$options[$ldap_query->qid] = $ldap_query->name;
	}
	
	$form['current'] = array(
		'#type' => 'select',
		'#options' => $options,
		'#default_value' => $current,
    '#description' => 'Выберите запрос, который получает актуальный список пользователей LDAP.',
	);
	$form['fired'] = array(
		'#type' => 'select',
		'#options' => $options,
		'#default_value' => $fired,
		'#description' => 'Выберите запрос, который получает список уволенных сотрудников',
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
		'#submit' => array('ldap_sync_query_form_submit'),
	);
	
	return $form;
}

/*
 * Устанавливаем выбранный запрос
 */

function ldap_sync_query_form_submit($form, &$form_state) {
	variable_set(CURRENT_USERS, $form_state['values']['current']);
	variable_set(FIRED_USERS, $form_state['values']['fired']);
}