# Custom Shell

## Overview

This **C** program creates a new shell to run commands in.

Shell header: `custom_shell_${whoami}(pid:<PID>):~ ${pwd}$`

A new command, `multi filename.txt [output filename.txt]` is avaiable. This commands executes all of the commands listed in `filename.txt`. By providing the optional `output` argument, `stdout` will be redirected for the duration of the `multi` commands's execution. The filename provided to the `output` argument does not have to exist. Recursive usage of the `multi` command is supported.

## Quickstart

Provided is a test file `commands.txt` that contains a list of commands to execute. Another file, `nested.txt` also contains a list of commands and is executed by use of the `multi` command from within the `commands.txt` file.

Example:

1. Navigate to the project directory
2. Compile the project: `make`
2. Run the project: `./main`
3. Inside the new shell, run: `multi commands.txt output nested_output.txt`

The result is a newly created file `nested_output.txt` which contains the output from all of the commands exectued from within the `commands.txt` file. After execution, `stdout` is restored.

## Limitations

There are a few UNIX commands that do now work yet.

## License
MIT License
