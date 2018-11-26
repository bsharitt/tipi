# TIPIFM usage

## cd

Change directory

syntax: 'cd' (path)

Example: Change to TIPI.GAMES.EA5 folder

```
[1000.TIPI.]
$ cd TIPI.GAMES.EA5

[1000.TIPI.GAMES.EA5.]
$
```

Example: Change to floppy controller disk 2

```
[1000.TIPI.]
$ cd 1100.DSK2.

[1100.DSK2.]
$
```

## dir

List files in current directory or given directory

syntax: 'dir' [path]

Example: List files in current folder

```
[1000.TIPI.]
$ dir

Vol: TIPI
TIPICFG    PRG 0
TIPICFH    PRG 0
TIPICFI    PRG 0
```

Example: List files in root folder of TIPI. device

```
[1100.DSK1.]
$ dir TIPI.

Vol: TIPI
TIPICFG    PRG 0
TIPICFH    PRG 0
TIPICFI    PRG 0
```

Example: List files in floppy drive DSK1 (if both TIPI and Floppy Controller are present)

```
[1000.TIPI.]
$ dir 1100.DSK1.

Vol: INFOCOM
LOAD       PRG 0
DEADLINE   PRG 0
DEAD       D/F 128
```

## drives 

List drives along with disambiguation

syntax: 'drives' 

Example:

```
$ drives

1000 - TIPI DSK0 DSK1 DSK2 DSK3
1100 - DSK1 DSK2 DSK3 DSK4
```

## quit

Exit the program

syntax: 'quit'

Example:

```
[1000.TIPI.]
$ quit
```

## ver

Print information about TIPIFM

syntax: 'ver'

Example:

```
[1000.TIPI.]
$ ver

TIPIFM v1
File Manager for TIPI
www.jedimatt42.com
```
