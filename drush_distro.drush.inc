<?php

 /**
 * @file
 * Some helpful drush commands for developing Drupal distributions.
 */

 /**
  * Implements hook_drush_command().
  */
function drush_distro_drush_command() {
  $items = array();

  $items['project-update-revision'] = array(
    'description' => "This command will update the project revision in a .make file to the latest git commit.",
    'examples' => array(
      'drush p-ur panopoly_core' => 'Updates panopoly_core in drupal-org.make to latest revision',
    ),
    'aliases' => array('p-ur'),
    'arguments' => array(
      'project' => 'The name of the project.',
    ),
    'topics' => array('distro'),
    'options' => array(
      'file' => array(
        'description' => 'The name of the make file.',
        'example-value' => 'drupal-org.make',
      ),
    ),
  );

  $items['project-revision-changed'] = array(
    'description' => "This command will check if a project's revision has changed.",
    'examples' => array(
      'drush p-rc panopoly_core' => "Checks if panopoly_core's revision has changed in drupal-org.make",
    ),
    'aliases' => array('p-rc'),
    'arguments' => array(
      'project' => 'The name of the project.',
    ),
    'topics' => array('distro'),
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

  // Check if project is a git repository.
  if (!is_dir($path . '/.git')) {
    return drush_set_error('DRUSH_DISTRO_PUR_FAILED', dt('Error: @project is not a git repository.', array('@project' => $project))); 
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

/**
 * Check if a project's revision has changed.
 */
function drush_drush_distro_project_revision_changed($project = FALSE) {

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

  // Check if project is a git repository.
  if (!is_dir($path . '/.git')) {
    return drush_set_error('DRUSH_DISTRO_PUR_FAILED', dt('Error: @project is not a git repository.', array('@project' => $project))); 
  }

  // Get project git hash.
  $current_dir = drush_cwd();
  chdir($path);
  if (drush_shell_exec("git log --pretty=format:'%h' -n 1")) {
    $response = drush_shell_exec_output();
    if ($project_revision = $response[0]) {
      // Pattern to match git SHA1.
      $pattern = '/(^projects\[' . preg_quote($project) . '\][\[download\]]*\[revision\][\s]*=[\s]*)([a-f0-9]*)$/m';

      // Get revision from .make file.
      $file_path = drush_normalize_path($make_file_path);
      $file_contents = file_get_contents($file_path);

      if (preg_match($pattern, $file_contents, $matches)) {
        $make_revision = $matches[2];
        if ($make_revision != $project_revision) {
          drush_print(dt('Revision for @project has changed from @make_revision to @project_revision.', array(
            '@project' => $project,
            '@make_revision' => $make_revision,
            '@project_revision' => $project_revision
          )));
        } else {
          drush_print(dt("No revision change."));
        }
      }

      // Set working directory back to profile root.
      chdir($current_dir);
      return;
    }
  }

  // Print error message.
  return drush_set_error('DRUSH_DISTRO_PUR_FAILED', dt('Error: revision could not be found.')); 
}
