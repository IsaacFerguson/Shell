HELP MANUAL

This is a guide to help run my basic shell

###############
# general use #
###############

The shell works by reading commands that the user enters:
    Ex. myshell> ls

This shell supports two types of commands:
    Bulit-in -> these commands are supported within the shell 
                instead of going outside the shells code to execute

    External -> these commands are not run by the shell but use External
                commands to execute

All arguments are seperated by whitespace
    Ex. myshell> wc text.txt

To execute the command you press enter

#####################
# Bulit-in Commands #
#####################

a. cd <directory> - Change the current default directory to
<directory>. If the <directory> argument is not present, report the current directory. If the directory does not exist an appropriate error should be reported. This command should also change the PWD environment variable.
b. clr - Clear the screen.
c. dir <directory> - List the contents of directory <directory>.
d. environ - List all the environment strings.
e. echo <comment> - Display <comment> on the display followed by a new line (multiple spaces/tabs may be reduced to a single space).
f. help - Display the user manual using the more filter.
g. pause - Pause operation of the shell until 'Enter' is pressed.
h. quit - Quit the shell.

##################
# Other Features #
##################

1. Input and Output Redirection:
    Input and Output redirection or IO redirection allows you to change
    where a commands input and out put go

    Without redirection the input for a command comes from the keyboard
    and the Output goes to the terminal

    To redirect input:
        If you want to change the input of a command use
        "<" char after the command args followed by where you
        want the input to come from 
        Ex. myshell> ls < input.txt

    To redirect output:
        IF you want to change the output of a command use
        ">" or ">>" chars after teh command args followed by where
        you wabt the output to end up
        Ex. myshell> echo hi > output.txt 
        or  myshell> ls >> newOut.txt

2. Piping:
    Piping allows for the output of one command to become the
    input of another command

    This is like IO redirection but using the redirection to
    run another command

    To pipe a command you would use the "|" char between the two commands
    Ex. myshell> ls | wc

3. Batch File:
    This shell allows for the use of a batch File

    A batch file allows for a list of commands to be run without input
    from the keyboard. If you want to run several files quickly batch will be useful

    To run a batch file you need to specifiy it when starting the shell
    Ex. ./myshell batchfile

4. background Execution:
    background execution allows for a command to run while also allowing 
    for the user to enter the next command before the first finishes
    Ex. myshell> dir &
