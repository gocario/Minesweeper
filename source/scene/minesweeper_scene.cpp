#include "minesweeper_scene.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image_manager.hpp"
#include "fs.h"


minesland_s minesland;
sf2d_texture* mineTiles = NULL;


	/*----------*\
	 | Startend |
	\*----------*/

void ms_init(u16 width, u16 height, u16 bombCount)
{
	ms_fini();

	minesland.size = width * height;
	minesland.width = width;
	minesland.height = height;
	minesland.bombCount = bombCount;
	minesland.tileLeft = minesland.size - bombCount;
	minesland.data = new tile_s[minesland.size];

	minesland.x = 0;
	minesland.y = 0;
	minesland.tileWidth = 16;
	minesland.tileHeight = 16;

	minesland.tile.type = TILE_EMPTY;
	minesland.tile.state = TILE_UNKNOWN;
	minesland.tile.checked = true;

	ms_populateTiles();
}


void ms_fini()
{
	delete[] minesland.data;
	minesland.width = 0;
	minesland.height = 0;
	minesland.size = 0;
	minesland.bombCount = 0;
	minesland.tileLeft = 0;

	minesland.x = 0;
	minesland.y = 0;
	minesland.tileWidth = 0;
	minesland.tileHeight = 0;

	minesland.tile.type = TILE_EMPTY;
	minesland.tile.state = TILE_UNKNOWN;
	minesland.tile.checked = true;
}


void ms_win()
{
	printf("You won!\n");

	for (u16 ii = 0; ii < minesland.size; ii++)
	{
		if (minesland.data[ii].type == TILE_MINE)
		{
			minesland.data[ii].state = TILE_DISCOVERED;
		}
	}
}


void ms_boom()
{
	printf("You lose!\n");

	for (u16 ii = 0; ii < minesland.size; ii++)
	{
		if (minesland.data[ii].type == TILE_MINE)
		{
			if (minesland.data[ii].state == TILE_UNKNOWN)
				minesland.data[ii].type = TILE_MINE_BOOM;
		
			minesland.data[ii].state = TILE_DISCOVERED;
		}
	}
}


	/*------------*\
	 | Generation |
	\*------------*/

void ms_clearTiles()
{
	for (u16 ii = 0; ii < minesland.size; ii++)
	{
		minesland.data[ii].type = TILE_EMPTY;
		minesland.data[ii].state = TILE_UNKNOWN;
	}
}


void ms_generateMines(s16 px, s16 py, u16 bombCount)
{
	u8 count = minesland.bombCount;
	s16 x, y;
	while (count != 0)
	{
		x = rand() % minesland.width;
		y = rand() % minesland.height;

		if (px == x && py == y);
		else if (!ms_isMine(x, y))
		{
			ms_getTile(x, y)->type = TILE_MINE;
			count--;
		}
	}
}


u8 ms_countAdjacentMines(u16 x, u16 y)
{
	u8 count = 0;
	for (s16 row = -1; row < 2; row++)
	{
		for (s16 col = -1; col < 2; col++)
		{
			if (row == 0 && col == 0);
			else if (ms_isMine(x + col, y + row))
			{
				count++;
			}
		}
	}
	return count;
}


void ms_updateAdjacentMines(u16 x, u16 y)
{
	if (!ms_isMine(x, y))
	{
		u8 count = ms_countAdjacentMines(x, y);
		tile_s* tile = ms_getTile(x, y);
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
			case 0:
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
			ms_updateAdjacentMines(col, row);
		}
	}
}


void ms_populateTiles()
{
	ms_clearTiles();
	ms_generateMines(-1, -1, minesland.bombCount);
	ms_updateAdjacentTiles();
}


	/*-------*\
	 | Model |
	\*-------*/

tile_s* ms_getTile(s16 x, s16 y)
{
	if (x >= 0 && x < minesland.width && y >= 0 && y < minesland.height)
		return &(minesland.data[y * minesland.width + x]);
	else
		return &(minesland.tile);
}


bool ms_isMine(s16 x, s16 y)
{
	return (ms_getTile(x, y)->type == TILE_MINE);
}


bool ms_isDiscovered(s16 x, s16 y)
{
	return (ms_getTile(x, y)->state == TILE_DISCOVERED);
}


	/*------------*\
	 | Controller |
	\*------------*/

void ms_checkWin()
{
	if (minesland.tileLeft == 0)
	{
		ms_win();
	}
}


void ms_clearCheckTiles()
{
	for (u16 ii = 0; ii < minesland.size; ii++)
	{
		minesland.data[ii].checked = false;
	}
}


void ms_checkTile(s16 x, s16 y)
{
	tile_s* tile = ms_getTile(x, y);

	if (!tile->checked && tile->state != TILE_DISCOVERED)
	{
		tile->checked = true;

		if (tile->type != TILE_MINE)
		{
			ms_discover(tile);
		}

		if (tile->type == TILE_EMPTY)
		{
			for (s16 row = -1; row < 2; row++)
			{
				for (s16 col = -1; col < 2; col++)
				{
					if (row == 0 && col == 0);
					else
					{
						ms_checkTile(x + col, y + row);
					}
				}
			}
		}
	}
}


void ms_discover(tile_s* tile)
{
	if (tile->state != TILE_DISCOVERED)
	{
		tile->state = TILE_DISCOVERED;
		minesland.tileLeft--;

		ms_checkWin();
	}
}


void ms_discoverTile(u16 x, u16 y)
{
	tile_s* tile = ms_getTile(x, y);
	if (tile->state == TILE_UNKNOWN)
	{
		if (tile->type == TILE_MINE)
		{
			ms_boom();
		}
		else
		{
			ms_clearCheckTiles();
			ms_checkTile(x, y);
		}
	}
}


void ms_discoverTiles()
{
	for (u16 ii = 0; ii < minesland.size; ii++)
	{
		ms_discover(&minesland.data[ii]);
	}
}


void ms_stateUnknownTile(u16 x, u16 y)
{
	ms_getTile(x, y)->state = TILE_UNKNOWN;
}


void ms_stateFlagTile(u16 x, u16 y)
{
	tile_s* tile = ms_getTile(x, y);
	if (tile->state == TILE_FLAGGED)
		ms_stateUnknownTile(x, y);
	else if (tile->state != TILE_DISCOVERED)
		tile->state = TILE_FLAGGED;

}


void ms_stateHintTile(u16 x, u16 y)
{
	tile_s* tile = ms_getTile(x, y);
	if (tile->state == TILE_HINTED)
		ms_stateUnknownTile(x, y);
	else if (tile->state != TILE_DISCOVERED)
		tile->state = TILE_HINTED;
}


void ms_stateHeldUp(u16 x, u16 y)
{
	tile_s* tile = ms_getTile(x, y);
	if (tile->state == TILE_UNKNOWN_HELD)
		tile->state = TILE_UNKNOWN;
	else if (tile->state == TILE_FLAGGED_HELD)
		tile->state = TILE_FLAGGED;
	else if (tile->state == TILE_HINTED_HELD)
		tile->state = TILE_HINTED;
}


void ms_stateHeldDown(u16 x, u16 y)
{
	tile_s* tile = ms_getTile(x, y);
	if (tile->state == TILE_UNKNOWN)
		tile->state = TILE_UNKNOWN_HELD;
	else if (tile->state == TILE_FLAGGED)
		tile->state = TILE_FLAGGED_HELD;
	else if (tile->state == TILE_HINTED)
		tile->state = TILE_HINTED_HELD;
}


	/*------*\
	 | View |
	\*------*/

void ms_getTexTileType(tile_s* tile, int* tex_x, int* tex_y)
{
	switch (tile->type)
	{
		case TILE_MINE:
			*tex_x = 2; *tex_y = 0;
			break;
		case TILE_EMPTY:
			*tex_x = 3; *tex_y = 0;
			break;
		case TILE_1:
			*tex_x = 0; *tex_y = 2;
			break;
		case TILE_2:
			*tex_x = 1; *tex_y = 2;
			break;
		case TILE_3:
			*tex_x = 2; *tex_y = 2;
			break;
		case TILE_4:
			*tex_x = 3; *tex_y = 2;
			break;
		case TILE_5:
			*tex_x = 0; *tex_y = 3;
			break;
		case TILE_6:
			*tex_x = 1; *tex_y = 3;
			break;
		case TILE_7:
			*tex_x = 2; *tex_y = 3;
			break;
		case TILE_8:
			*tex_x = 3; *tex_y = 3;
			break;
		case TILE_MINE_BOOM:
			*tex_x = 2; *tex_y = 1;
			break;
		case TILE_EMPTY_BOOM:
		default:
			*tex_x = 3; *tex_y = 2;
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
		// case TILE_HINTED:
		// 	*tex_x = 1; *tex_y = 0;
		// 	break;
		case TILE_UNKNOWN_HELD:
			*tex_x = 0; *tex_y = 1;
			break;
		case TILE_FLAGGED_HELD:
			*tex_x = 1; *tex_y = 1;
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


	/*-------*\
	 | Scene |
	\*-------*/

MinesweeperScene::MinesweeperScene()
{

}


MinesweeperScene::~MinesweeperScene()
{

}


void MinesweeperScene::initialize()
{
	srand(osGetTime());

	ms_init(20, 15, 20);

	if (!mineTiles) mineTiles = sf2d_create_texture_mem_RGBA8(ImageManager::mineTiles_img.pixel_data, ImageManager::mineTiles_img.width, ImageManager::mineTiles_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
}


void MinesweeperScene::destroy()
{
	ms_fini();

	if (mineTiles) sf2d_free_texture(mineTiles);
}


void MinesweeperScene::drawTopScreen()
{
	printf("\x1B[10;1HTileleft: %3u\n", minesland.tileLeft);
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

		if (ks.down & KEY_X)
		{
			ms_init(20, 15, 20);
			// ms_discoverTiles();
			consoleClear();
		}
	}

	{
		if (ks.down & KEY_TOUCH)
		{
			if (withinRectangle(ks.touch.px, ks.touch.py, 0, 0, 320, 240))
			{
				originalTouch = ks.touch;

				u16 x = ks.touch.px / minesland.tileWidth;
				u16 y = ks.touch.py /minesland.tileHeight;

				ms_stateHeldDown(x, y);

				// printf("(%2u;%2u) down\n", x, y);
			}
		}

		if (ks.held & KEY_TOUCH)
		{
			if (withinRectangle(ks.touch.px, ks.touch.py, 0, 0, 320, 240))
			{
				currentTouch = ks.touch;

				u16 ox = originalTouch.px / minesland.tileWidth;
				u16 oy = originalTouch.py /minesland.tileHeight;

				u16 x = currentTouch.px / minesland.tileWidth;
				u16 y = currentTouch.py /minesland.tileHeight;

				if (ox == x && oy == y)
					ms_stateHeldDown(ox, oy);
				else
					ms_stateHeldUp(ox, oy);
			}
		}

		if (ks.up & KEY_TOUCH)
		{
			if (withinRectangle(currentTouch.px, currentTouch.py, 0, 0, 320, 240))
			{
				u16 ox = originalTouch.px / minesland.tileWidth;
				u16 oy = originalTouch.py /minesland.tileHeight;

				u16 x = currentTouch.px / minesland.tileWidth;
				u16 y = currentTouch.py /minesland.tileHeight;

				// printf("(%2u;%2u) up\n", x, y);

				ms_stateHeldUp(x, y);

				if (ox == x && oy == y)
				{
					if (ks.held & KEY_L)
					{
						ms_stateFlagTile(x, y);
					}
					else if (ks.held & KEY_R)
					{
						ms_stateHintTile(x, y);
					}
					else
					{
						ms_discoverTile(x, y);
					}
				}
			}

			originalTouch = ks.touch;
			currentTouch = ks.touch;
		}
	}
}
