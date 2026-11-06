## Version 0.3:
- Windows react to resizing <br />
- Background animations! <br />
    - Can be toggled in settings <br />
- Definition names! <br />
- At long last, test mode has been added! <br />
- Vectors save after Multiple Choice and Type <br />
## Version 0.2.2: <br />
- running `$ make bootstrapConfig` will create a config directory and place an example list inside <br />
- get_lists can show no lists without prompting user to add a list <br />
- get_lists now includes functionality for moving, renaming lists! <br />
- cleaned up codebase a bit <br />
- Bug fixes <br />
## Version 0.2.1: <br />
- Added config options to change movement keys. you can now choose between "nvim", "arrow", and "ijkl". The option has been added to the settings menu <br />
- fixed issues with deleting terms <br />
- added an indicator to typing quizes to show which definition/term it wants <br />
## Version 0.2: <br />
- BREAKING: new flashcard list file format (.list). Opening a list should automatically convert it to the new format. <br />
 <br />
- multiple flashcard definitions! add a new definition with A, and remove a definition with D <br />
- you can now access flashcard lists in folders. Currently they cannot be moved into and out of those folders except by manually moving the files. This will be fixed. <br />
- clearer flashcard vector chooser that allows you to choose which vectors get quized and in which direction <br />
- new flashcard file format, files will automatically be converted <br />
- study vectors and direction now are saved when redoing mistakes <br />
- added progress headers (how far you are through something) to more menus that need them, and to all study modes. <br />
- use --version to print version <br />
- select question count before study
