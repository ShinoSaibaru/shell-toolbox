# shell-toolbox
A toolbox for user-made scripts for quick edition and access.

Livestream at [link](http://www.livecoding.tv/shino/).

The toolbox, known as the `shtb` command, provides a convenience for Unix administrators and any users in general in a form of a single command that allows to execute the scripts placed in `~/.shtb` folder. It is created automatically by `make`.

The options the program will feature are still in development.

The scripts can have a `#description` "directive". Anything after that until the end of the line will be considered as a description for a given tool. Only the first `#description` is taken into account.