
2017 Operation Systems Project #1 - A Simple Shell
20144453 / YongJun Kim

 Development and test environment
 * OS : macOs sierra (ver 10.12.1)
 * gcc : 4.2.1
 * Thread model: posix
 * Target: x86_64-apple-darwin16.1.0


There are two ways to use the shell.
./simpleshell & ./simpleshell batchfile ...
The former runs the shell program, and the latter reads and executes the commands in batchfile one line at a time.
When the end of the file is reached, the shell exits.

When you start the shell, it saves <pathname>/simpleshell in the environment variable "shell".
You can run the application by default. The shell fork the child process to run program.
The child processor has the value <pathname>/simpleshell in the environment variable "parent".
The shell has the <,>, >>, |, & functions.
">" is created if the outputfile does not exist, and if it does exist, the contents of the previous outputfile are deleted.
">>" is created if the outputfile does not exist, and if it already exists, it is output after the end of the contents of the previous outputfile.
"<" eads input and passes the data if inputfile exists.
"<" will read the file and pass the data if inputfile exists.
If you are at the end of the "&" command, you can perform background tasks.

Currently implemented funtion in the program are:

 * chdir <directory> [built-in]
    Changes the current default directory to the given value.
    Update the environment variable PWD name and OLDPWD.
    If there is no directory to change, the current directory path is displayed on the screen.

 * clr
    Erase all screens.

 * dir
    Print the directory contents of the screen.
    The -a, -l, and -al options are supported.

 * environ
    Prints all environment strings.


 * echo <comment> 
    Print <comment> on the screen, including newlines. 
    (Multiple spaces and tabs can be abbreviated as a single space.)
    
 * help [built-in]
    Use more filters to display the user's manual.

 * pause
    The message 'Press ENTER to continue' is printed and the shell pauses.

 * sleep <second>
    Pauses the current program execution by <seconds>.
    <second> is a natural number.

 * quit [built-in]
    Exit the shell.

 * proc
    Prints a list of currently running background processes.
    Displays all background attempts attempted.
