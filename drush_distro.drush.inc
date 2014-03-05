<?php

 /**
 * @file
 * Some helpful drush commands for developing drush distributions.
 */

 /**
  * Implements hook_drush_command().
  */
function drush_distro_drush_command() {
  $items = array();

  $items['project-update-revision'] = array(
    'description' => "This command will update the project revision in a .make file to the latest git commit.",
    'examples' => array(
      'drush pur panopoly_core' => 'Updates panopoly_core in drupal-org.make to latest revision',
    ),
    'aliases' => array('p-ur'),
    'arguments' => array(
      'project' => 'The name of the project.',
    ),
    'options' => array(
      'file' => array(
        'description' => 'The name of the make file.',
        'example-value' => 'drupal-org.make',
      ),
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function drush_distro_drush_help($section) {
  switch ($section) {
    case 'drush:distro':
      return dt('Some drush helpers for developing distributions.');
  }
}

/**
 * Update a project revision in .make file.
 */
function drush_drush_distro_project_update_revision($project = FALSE) {

  // Return if no project is set.
  if (!$project) {
    return drush_set_error('DRUSH_DISTRO_PUR_FAILED', dt('Error: project name missing.'));
  }

  // Get the make file, defaults to drupal-org.make.
  $file = (drush_get_option('file'));
  if (!$file) {
    $file = 'drupal-org.make';
  }

  // Get path to .make file.
  $make_file_path = drush_cwd() . '/' . $file;

  // Get project location.
  $path = drupal_get_path('module', $project);

  // If a path to the project is not found, return.
  if (!$path) {
    return drush_set_error('DRUSH_DISTRO_PUR_FAILED', dt('Error: project @project not found.', array('@project' => $project))); 
  }

  // Pattern to match git SHA1.
  $pattern = '/(^projects\[' . $project . '\]\[download\]\[revision\][\s]*=[\s]*)([a-f0-9]*)$/m';

  // Do the replacement.
  drush_op('drush_distro_regex_replace', $make_file_path, $pattern, '$1I WAS HERE');
}

/**
 * Helper function for regex replace in a file.
 */
function drush_distro_regex_replace($file_path, $pattern, $replace) {
  $file_path = drush_normalize_path($file_path);
  $file_contents = file_get_contents($file_path);
  $file_contents = preg_replace($pattern, $replace, $file_contents);
  file_put_contents($file_path, $file_contents);
}
