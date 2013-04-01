<?php
/*
 * Вывод уроков в таблице
 */
?>
<?php //dpm($result);?>
<?php
// Флаги - Подписаться и Изучил урок
$flag_signup = flag_get_flag('signup');
$flag_learned = flag_get_flag('learned');
global $user;
$uid = $user->uid;
// Переменная хранить информацию о последнем сданном тесте.
$is_passed_prev_quiz = 0;
?>

<table <?php if ($classes) { print 'class="'. $classes . '" '; } ?><?php print $attributes; ?>>
  <?php if (!empty($title)) : ?>
    <caption><?php print $title; ?></caption>
  <?php endif; ?>
  <?php if (!empty($header)) : ?>
    <thead>
      <tr>
        <?php foreach ($header as $field => $label): ?>
          <th <?php if ($header_classes[$field]) { print 'class="'. $header_classes[$field] . '" '; } ?>>
            <?php print $label; ?>
          </th>
        <?php endforeach; ?>
      </tr>
    </thead>
  <?php endif; ?>
  <tbody>
    <?php foreach ($rows as $row_count => $row): ?>
    	<?php
				// Записан ли на урок
				$is_signup = $flag_signup->is_flagged($result[$row_count]->node_taxonomy_index_nid);
				// Изучил ли урок
				$is_learned = $flag_learned->is_flagged($result[$row_count]->node_taxonomy_index_nid);
				// Сдал ли тест
				// Получеам id теста из БД
				$test_nid = db_select('field_data_field_lesson', 'f')
				->condition('f.field_lesson_target_id', $result[$row_count]->node_taxonomy_index_nid)
				->fields('f', array('entity_id'))
				->execute()
				->fetchField();
				// Получаем vid и time_end теста
				$query = db_select('quiz_node_results', 'q')
				->condition('q.nid', $test_nid)
				->condition('q.uid', $uid)
				->fields('q', array('vid', 'time_end'))
				->execute()
				->fetchAll();
				$test_vid = 0;
				$test_time_end = 0;
				foreach ($query as $item) {
					$test_vid = $item->vid;
					$test_time_end = $item->time_end;
				}
				// Сдан ли данный тест
				$is_passed_quiz = quiz_is_passed($uid, $test_nid, $test_vid);
			?>
      
      <tr <?php if ($row_classes[$row_count]) { print 'class="' . implode(' ', $row_classes[$row_count]) .'"';  } ?>>
      
      	<?php foreach ($row as $field => $content): ?>
        	<?php //dpm($field); ?>
          <td <?php if ($field_classes[$field][$row_count]) { print 'class="'. $field_classes[$field][$row_count] . '" '; } ?><?php print drupal_attributes($field_attributes[$field][$row_count]); ?>>
            <?php
							// Название урока
							if ($field == 'title') {
								if (!$is_signup || $is_learned) {
									print $result[$row_count]->node_taxonomy_index_title;
								} else {
									print $content;
								}
							}
							
							// Подписка
							if ($field == 'ops') {
								// Если не первый урок
								if (!$row_count == 0) {
									// Если записан на урок
									if ($is_signup) {
										print ("Дата подписки" . '<br />' . date('d.m.Y - H:i',$result[$row_count]->flag_content_node_timestamp));
									} elseif ($is_passed_prev_quiz) {
										print $content;
									}
								// Если первый урок и не записан на него
								} elseif ($row_count == 0 && !$is_signup) {
									print $content;
								} else {
									print ("Дата подписки" . '<br />' . date('d.m.Y - H:i',$result[$row_count]->flag_content_node_timestamp));
								}
							}
							
							// Тестирование
							if ($field == 'quiz_state') {
								// Если записан на урок
								if ($is_signup) {
									// и изучил его
									if ($is_learned) {
										// и сдал его
										if ($is_passed_quiz) {
											print ("Тест пройден" . "<br />" . date('d.m.Y - H:i',$test_time_end));
										} elseif (!empty($test_nid)) {
											print l("Пройти тест", drupal_get_path_alias('/node/' . $test_nid));
										} else {
											print "Тест отсутствует";
										}
									} elseif (!$is_learned) {
										print ("Изучите урок");
									}
								}
							}
							?>
          </td>
        <?php endforeach; ?>
      </tr>
      <?php
				// Если сдан текущий тест, записываем true в переменную, чтобы открыть доступ к следующему уроку.
				if ($is_passed_quiz) {
					$is_passed_prev_quiz = true;
				} else {
					$is_passed_prev_quiz = false;
				}
				?>
    <?php endforeach; ?>
  </tbody>
</table>