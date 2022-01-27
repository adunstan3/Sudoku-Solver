# Sudoku Solver

This is my implementation of Knuth's algorithm x with dancing links to solve sudoku puzzles. The user interface is writen in python with pygame, and the algorithm is writen in c++. The C++ first converts the sudoku into an exact cover problem which it solves with Kunith's Dancing Links process. The core of the algorithm adding and removing connections from a two-dimensional circular double linked list. The solved exact cover is then converted back into sudoku and displayed. 

Later I re-wrote the UI in JS and hosted the code on Heroku. You can try it out [here](https://www.andrewfdunstan.com/code/sudoku)


If you are running into problems here are some tips:

- pygame doesn't like python environments, I have to run it on the base environment to get it to work properly
- pygame has several non python dependencies: brew install sdl sdl_image sdl_mixer sdl_ttf portmidi mercurial
- Sometimes base pygame doesn't work on new operating systems: pip3 install pygame==2.0.0.dev12
- make sure that all the folders and text files are in the same position as they are in the git repository they are writen to by the program and are very important
