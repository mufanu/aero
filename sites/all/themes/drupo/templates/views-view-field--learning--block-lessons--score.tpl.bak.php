<?php
// Флаги - Подписаться и Изучил урок
$flag_signup = flag_get_flag('signup');
$flag_learned = flag_get_flag('learned');
// Если пользователь подписался на урок и изучил его
if (($flag_signup->is_flagged($row->node_taxonomy_index_nid)) && ($flag_learned->is_flagged($row->node_taxonomy_index_nid))) {
	// Получаем id пользователя
	global $user;
	$uid = $user->uid;
	// Id теста
	$test_nid = $row->node_field_data_field_test_nid;
	// Версия ответа
	$results_vid = $row->node_field_data_field_test__quiz_node_results_vid;
	// Проверяем, сдан ли данные тест
	if (quiz_is_passed($uid, $test_nid, $results_vid)) {
		print ("Тест пройден" . "<br />" . date('d.m.Y - H:i',$row->node_field_data_field_test__quiz_node_results_time_end));
	} else {
		print l("Пройти тест", 'node/' . $test_nid);
	}
}
?>