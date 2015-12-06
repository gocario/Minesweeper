#include "minesweeper_scene.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image_manager.hpp"
#include "fs.h"

minesland_s minesland;
sf2d_texture* mineTiles = NULL;


void ms_init(u16 width, u16 height, u16 bombCount)
{
	ms_fini();

	minesland.width = width;
	minesland.height = height;
	minesland.bombCount = bombCount;
	minesland.data = new tile_s[width * height];

	minesland.x = 0;
	minesland.y = 0;
	minesland.tileWidth = 16;
	minesland.tileHeight = 16;

	minesland.tile.type = TILE_EMPTY;
	minesland.tile.state = TILE_UNKNOWN;

	ms_populateTiles();
}


void ms_fini()
{
	delete[] minesland.data;
	minesland.width = 0;
	minesland.height = 0;
	minesland.bombCount = 0;

	minesland.x = 0;
	minesland.y = 0;
	minesland.tileWidth = 0;
	minesland.tileHeight = 0;
}


void ms_clearTiles()
{
	tile_s* tile;
	for (u16 row = 0; row < minesland.height; row++)
	{
		for (u16 col = 0; col < minesland.width; col++)
		{
			tile = ms_getTile(col, row);
			tile->type = TILE_EMPTY;
			tile->state = TILE_UNKNOWN;
		}
	}
}


void ms_generateBombs(u16 bombCount)
{
	u8 count = minesland.bombCount;
	int x, y;
	tile_s* tile;
	while (count != 0)
	{
		x = rand() % minesland.width;
		y = rand() % minesland.height;
		tile = ms_getTile(x, y);
		printf("Bomb {%2u,%2u} ?", x, y);
		if (!ms_isBomb(x, y))
		{
			printf(" Yes\n");
			tile->type = TILE_BOMB;
			count--;
		}
		else
		{
			printf(" No\n");
		}
	}
}


u8 ms_countAdjacentBombs(u16 x, u16 y)
{
	u8 count = 0;
	for (s16 row = y-1; row < y+2; row++)
	{
		for (s16 col = x-1; col < x+2; col++)
		{
			if (row == y && col == x);
			else
			{
				if (ms_isBomb(col, row))
					count++;
			}
		}
	}
	return count;
}


void ms_updateAdjacentBombs(u16 x, u16 y)
{
	if (!ms_isBomb(x, y))
	{
		u8 count = ms_countAdjacentBombs(x, y);
		
		tile_s* tile = ms_getTile(x, y);
		// printf("{%2u,%2u}: %1u\n", x, y, count);
		switch (count)
		{
			case 1:
				tile->type = TILE_1;
				break;
			case 2:
				tile->type = TILE_2;
				break;
			case 3:
				tile->type = TILE_3;
				break;
			case 4:
				tile->type = TILE_4;
				break;
			case 5:
				tile->type = TILE_5;
				break;
			case 6:
				tile->type = TILE_6;
				break;
			case 7:
				tile->type = TILE_7;
				break;
			case 8:
				tile->type = TILE_8;
				break;
			default:
				tile->type = TILE_EMPTY;
				break;
		}
	}
}


void ms_updateAdjacentTiles()
{
	for (u16 row = 0; row < minesland.height; row++)
	{
		for (u16 col = 0; col < minesland.width; col++)
		{
			ms_updateAdjacentBombs(col, row);
		}
	}
}


void ms_populateTiles()
{
	ms_clearTiles();
	ms_generateBombs(minesland.bombCount);
	ms_updateAdjacentTiles();
}


tile_s* ms_getTile(s16 x, s16 y)
{
	if (x >= 0 && x < minesland.width && y >= 0 && y < minesland.height)
		return &(minesland.data[y * minesland.width + x]);
	else
		return &(minesland.tile);
}


bool ms_isBomb(s16 x, s16 y)
{
	return (ms_getTile(x, y)->type == TILE_BOMB);
}


bool ms_isDiscovered(s16 x, s16 y)
{
	return (ms_getTile(x, y)->state == TILE_DISCOVERED);
}


void ms_discoverTile(u16 x, u16 y)
{
	ms_getTile(x, y)->state = TILE_DISCOVERED;
}


void ms_discoverTiles()
{
	for (u16 row = 0; row < minesland.height; row++)
	{
		for (u16 col = 0; col < minesland.width; col++)
		{
			ms_discoverTile(col, row);
		}
	}
}


void ms_getTexTileType(tile_s* tile, int* tex_x, int* tex_y)
{
	switch (tile->type)
	{
		case TILE_BOMB:
			*tex_x = 2; *tex_y = 0;
			break;
		case TILE_EMPTY:
			*tex_x = 3; *tex_y = 0;
			break;
		case TILE_1:
			*tex_x = 0; *tex_y = 1;
			break;
		case TILE_2:
			*tex_x = 1; *tex_y = 1;
			break;
		case TILE_3:
			*tex_x = 2; *tex_y = 1;
			break;
		case TILE_4:
			*tex_x = 3; *tex_y = 1;
			break;
		case TILE_5:
			*tex_x = 0; *tex_y = 2;
			break;
		case TILE_6:
			*tex_x = 1; *tex_y = 2;
			break;
		case TILE_7:
			*tex_x = 2; *tex_y = 2;
			break;
		case TILE_8:
			*tex_x = 3; *tex_y = 2;
			break;
		default:
			*tex_x = 0; *tex_y = 0;
			break;
	}
}


void ms_getTexTileState(tile_s* tile, int* tex_x, int* tex_y)
{
	switch (tile->state)
	{
		case TILE_DISCOVERED:
			ms_getTexTileType(tile, tex_x, tex_y);
			break;
		case TILE_FLAGGED:
			*tex_x = 1; *tex_y = 0;
			break;
		case TILE_HINTED:
			*tex_x = 1; *tex_y = 0;
			break;
		case TILE_UNKNOWN:
		default:
			*tex_x = 0; *tex_y = 0;
			break;
	}
}


void ms_drawTile(u16 x, u16 y)
{
	int tex_x, tex_y;
	ms_getTexTileState(ms_getTile(x, y), &tex_x, &tex_y);

	sf2d_draw_texture_part(mineTiles, minesland.x + x * minesland.tileWidth, minesland.y + y * minesland.tileHeight, tex_x * minesland.tileWidth, tex_y * minesland.tileHeight, minesland.tileWidth, minesland.tileHeight);
}


void ms_drawTiles()
{
	for (u16 row = 0; row < minesland.height; row++)
	{
		for (u16 col = 0; col < minesland.width; col++)
		{
			ms_drawTile(col, row);
		}
	}
}



MinesweeperScene::MinesweeperScene()
{

}


MinesweeperScene::~MinesweeperScene()
{

}


void MinesweeperScene::initialize()
{
	srand(osGetTime());

	ms_init(20, 15, 50);

	if (!mineTiles) mineTiles = sf2d_create_texture_mem_RGBA8(ImageManager::mineTiles_img.pixel_data, ImageManager::mineTiles_img.width, ImageManager::mineTiles_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
}


void MinesweeperScene::destroy()
{
	ms_fini();

	if (mineTiles) sf2d_free_texture(mineTiles);
}


void MinesweeperScene::drawTopScreen()
{

}


void MinesweeperScene::drawBottomScreen()
{
	ms_drawTiles();
}


void MinesweeperScene::updateInput(const keystate_s& ks)
{
	{
		if (ks.down & KEY_A)
		{
			ms_discoverTile(0, 0);
			ms_discoverTile(1, 1);
			ms_discoverTile(2, 1);
			ms_discoverTile(3, 1);
		}

		if (ks.down & KEY_Y)
		{
			ms_discoverTiles();
		}

		if (ks.held & KEY_L && ks.held & KEY_R && ks.down & KEY_X)
		{
			ms_init(20, 15, 50);
			ms_discoverTiles();
		}
	}

	{
		if (ks.down & KEY_TOUCH)
		{
			if (withinRectangle(ks.touch.px, ks.touch.py, 0, 0, 320, 240))
			{
				printf("(%2u;%2u) touched\n", ks.touch.px / minesland.tileWidth + 1, ks.touch.py /minesland.tileHeight + 1);
				ms_discoverTile(ks.touch.px / minesland.tileWidth, ks.touch.py /minesland.tileHeight);
			}
		}

		if (ks.held & KEY_TOUCH)
		{
			if (ks.down & KEY_L)
			{
				if (withinRectangle(ks.touch.px, ks.touch.py, 0, 0, 320, 240))
				{
					ms_getTile(ks.touch.px / minesland.tileWidth, ks.touch.py /minesland.tileHeight)->type = TILE_3;
				}
			}
		}
	}
}
