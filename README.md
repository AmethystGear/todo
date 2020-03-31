# todo
Many people have a basic TODO file that keeps a list of things to do. But wouldn't it be nicer to have a small terminal program to manage that file?
Enter todo.c, a simple program that lets you keep track of a todo list via terminal.

Unlike complicated todo managers, todo.c keeps things very simple. It only has 4 basic commands:

```add``` -- add something to the list

```finish``` -- remove something from the list

```list``` -- show the list

```clear``` -- remove everything from the list

## using todo
Since todo.c is just a C program, all you need to do is compile it to turn it into an executable on whatever platform you use. 

## todo anywhere!
what if you want to run todo.c without typing in the full path to the program location?

#### linux
using your favorite text editor, add this line to ```~/.bash_aliases```: 
```alias todo='{path to todo executable}'```

then run ```source ~/.bash_aliases``` in the terminal.
Now you can run ```todo {X}``` from anywhere!

#### windows
/* TODO */

#### mac
/* TODO */
