# Drush Distro

Some helpful drush commands for developing Drupal distributions

### Installation

    git clone https://github.com/arshad/drush_distro.git $HOME/.drush/drush_distro
    drush cc drush

### Usage

##### Update a project's revision in a make file.

    drush p-ur projectname

This will automatically get the git revision from projectname and update its revision in your distribution's make file.

##### Check if a project's revision has changed.

    drush p-rc projectname

