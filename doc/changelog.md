## Version 0.2.1:
- Added config options to change movement keys. you can now choose between "nvim", "arrow", and "ijkl". The option has been added to the settings menu
- fixed issues with deleting terms
- added an indicator to typing quizes to show which definition/term it wants

## Version 0.2:
- BREAKING: new flashcard list file format (.list). Opening a list should automatically convert it to the new format.

- multiple flashcard definitions! add a new definition with A, and remove a definition with D
- you can now access flashcard lists in folders. Currently they cannot be moved into and out of those folders except by manually moving the files. This will be fixed.
- clearer flashcard vector chooser that allows you to choose which vectors get quized and in which direction
- new flashcard file format, files will automatically be converted
- study vectors and direction now are saved when redoing mistakes
- added progress headers (how far you are through something) to more menus that need them, and to all study modes.
- use --version to print version
- select question count before study
