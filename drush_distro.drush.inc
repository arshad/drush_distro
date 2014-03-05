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

  // Get project git hash.
  $current_dir = drush_cwd();
  chdir($path);
  if (drush_shell_exec("git log --pretty=format:'%h' -n 1")) {
    $response = drush_shell_exec_output();
    if ($hash = $response[0]) {
      // Pattern to match git SHA1.
      $pattern = '/(^projects\[' . preg_quote($project) . '\][\[download\]]*\[revision\][\s]*=[\s]*)([a-f0-9]*)$/m';

      // Do the replacement in the make file.
      $file_path = drush_normalize_path($make_file_path);
      $file_contents = file_get_contents($file_path);
      $file_contents = preg_replace_callback($pattern, function ($match) use ($hash) {
        return $match[1] . $hash;
      }, $file_contents);
      file_put_contents($file_path, $file_contents);

      // Set working directory back to profile root.
      chdir($current_dir);

      // Print success message and return.
      drush_print(dt('Revision for @project successfully updated.', array('@project' => $project)));

      return;
    }
  }

  // Print error message.
  return drush_set_error('DRUSH_DISTRO_PUR_FAILED', dt('Error: last revision could not be found.')); 
}
