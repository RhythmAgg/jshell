# JShell
### Rhythm Aggarwal --> 2021101081

---

## Requirements

- GCC compiler
- Linux OS (preferably Ubuntu)

## Running Shell

From the root directory of project, run `make` followed by `./shell`

## File & Functions

- `main.c`
  - Run Shell Loop for taking in commands.
  - Extract Arguments from command
  - Bind functions from different files together
  - Manage history of the commands and background process handling
  - manage promts
- `cd.c`
  - Manages the _cd_ command outputs
  - giving more than 2 arguements with cd as the first arguement gives error
  - It follows the standard implementation of the _cd_ command of the `zsh` shell
  - For the `cd ..` command, the shell will reach to the parent directory of the current directory till the directory is `\`
- `pwd_.c`
  - it emulates the actual _pwd_ command of the `zsh` shell
  - it prints on the terminal absolute path of the current working directory
- `history.c`
  - implements functions to print the lates commands in the history (10 commands)
  - Stores maximum 20 commands

- `echo.c`

  - Prints user input after removing excess spaces/tabs

- `ls.c`

  - Prints all files/sub-directories in a location
  - Flags implemented: `a` and `l`
  - Multiple directories supported

- `pinfo.c`

  - Prints info about process with given PID (current process if none is given)

- `headers.h`
  - contains all the useful headers files 

- `process.c`
  - contains impementation of the forground and background process
  - uses `execvp` buit-in command to implement the commands
  - in case of foreground processes, if the program to more than 1 sec, the prompt will show it for one iteration
  - whenever any background process gets over, the process name along wiht its pid will be outputed
  - the background processes are stored in a linked list
- `user_defined.c`
  - contains the user-defined `jobs`, `sig`, `fg`, `bg` processes
  - work in the way as defined in the question pdf
  
### Other Implementations

###### Commands/functions which don't have a separate file.

- Signals
  - `Ctrl+C` Stops current foreground process.When the process is the parent shell process, the history will be overwritten in the `history_logs.txt`
- Signals
  - `Ctrl+Z` will push the current running foreground proces to background and update its status to stop, it wont affect the terminal execution
- Signals
  - `Ctrl+D` will exit from the program without affecting the main terminal ( zsh or bash)


- Background Processes Manipulations

  - Implementation of background processes list, removing/adding them from/to the list is done in `main.c`.

- Exiting from the Shell
  - `Ctrl+C`
  - `exit` command.

## Assumptions

- It is assumed that the tab will only be pressed to search for possible directories in the current directory or the children directoriesS
- Reasonable assumptions are made for things like length of current path, number of commands, command length, etc. The shell will work perfectly for all "day to day work".

- `user` and `hostname` will be constant throughout the life of current shell instance.

- `echo` will treat everything as normal character.

- File and directory names shouldn't contain spaces or special characters. (Some commands may work even without this.)

- `pinfo` may break on systems not running Ubuntu. This command uses `/proc` files which may not be same for other OS.

- Every user input will be stored as it is in history, given it is not same as the last input and contains at least one character other than space or tab.

- Prompt will display only _terminated normally_ or _terminated abnormally_ when a background process ends.

- Prompt will check exist status of background processes the very moment _SIGCHLD_ signal is called and the message that a particular background process has ended will be printed automatically

- Any error while running a command will result in aborting the command after printing an error message.

- `&` (used for running process in background) can come anywhere in command.

- `history_logs.txt` file is used to save the history of commands after the shell has exited.
It is assumed that shell will be run as a single process and no 2 processes will run the shell concurrently.


> Will try to implement more features soon, Wish me luck!