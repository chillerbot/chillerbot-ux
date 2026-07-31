#!/usr/bin/env python3

import sys

import twmap

if len(sys.argv) != 6:
	print("usage: set_gametile.py IN_MAP OUT_MAP x y index")
	sys.exit(0)

arg_map_in = sys.argv[1]
arg_map_out = sys.argv[2]
arg_x = int(sys.argv[3])
arg_y = int(sys.argv[4])
arg_index = int(sys.argv[5])

m = twmap.Map(arg_map_in)

tiles = m.game_layer().tiles
tiles[arg_y][arg_x][0] = arg_index
m.game_layer().tiles = tiles

m.save(arg_map_out)
