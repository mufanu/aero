<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $options['type'] will either be ul or ol.
 * @ingroup views_templates
 */
?>
<?php
$vid = 1;
$parent = 0;
$max_depth = 1;
$tree = taxonomy_get_tree($vid, $parent, $max_depth);
?>
<?php foreach ($tree as $item): ?>
<?php	if($item->depth == 0): ?>
	<div class='depth-0'><?php print ($item->name); ?>
<?php else: ?>
	<?php if($item->depth == 1): ?>
  	<div class='depth-1'><?php print ($item->name); ?></div>
  <?php endif; ?>
<?php endif; ?>
</div>
<?php endforeach; ?>		
