# Drush Distro

Some helpful drush commands for developing Drupal distributions

### Installation

    git clone https://github.com/arshad/drush_distro.git $HOME/.drush/drush_distro
    drush cc drush

### Usage

1. Update a project's revision in a make file. This will automatically get the git revision from projectname and update its revision in your distribution's make file.

    drush p-ur projectname

2. Check if a project's revision has changed.

    drush p-rc projectname

