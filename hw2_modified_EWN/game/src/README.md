## Notable Structures

### `_cube`

Cubes are the pieces move-able by players in the game.

### `_square`

There are 49 squares on the board (7x7) in the game. 1 cube occupies 1 square, and is move-able by the player.

### `_board`

The board of the game, notable that the game state is represented by 64\*6 bits.

```7x7 board
00 01 02 03 04 05 06
07 08 09 10 11 12 13
14 15 16 17 18 19 20
21 22 23 24 25 26 27
28 29 30 31 32 33 34
35 36 37 38 39 40 41
42 43 44 45 46 47 48
```

```6x6 board
00 01 02 03 04 05 
06 07 08 09 10 11
12 13 14 15 16 17
18 19 20 21 22 23
24 25 26 27 28 29
30 31 32 33 34 35
```

```6x7 board
00 01 02 03 04 05 06 
07 08 09 10 11 12 13
14 15 16 17 18 19 20
21 22 23 24 25 26 27
28 29 30 31 32 33 34
35 36 37 38 39 40 41
```

### `_board_gui`

The display on linux terminal of the game.

- `no_hl` = no highlight

