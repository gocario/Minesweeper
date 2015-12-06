#ifndef MINESWEEPER_SCENE_HPP
#define MINESWEEPER_SCENE_HPP

#include <sf2d.h>

#include "scene/scene.hpp"

enum tileType_e
{
	TILE_BOMB,
	TILE_EMPTY,
	TILE_1,
	TILE_2,
	TILE_3,
	TILE_4,
	TILE_5,
	TILE_6,
	TILE_7,
	TILE_8,
};

enum tileState_e
{
	TILE_UNKNOWN,
	TILE_DISCOVERED,
	TILE_FLAGGED,
	TILE_HINTED,
};

struct tile_s
{
	tileType_e type = TILE_EMPTY;
	tileState_e state = TILE_UNKNOWN;
};

struct minesland_s
{
	tile_s* data;
	tile_s tile;
	u16 x;
	u16 y;
	u16 width;
	u16 height;
	u16 tileWidth;
	u16 tileHeight;
	u16 bombCount;
};


void ms_init(u16 width, u16 height, u16 bombCount);
void ms_fini();

void ms_clearTiles();
void ms_generateBombs(u16 bombCount);
u8 ms_countAdjacentBombs(u16 x, u16 y);
void ms_updateAdjacentBombs(u16 x, u16 y);
void ms_updateAdjacentTiles();
void ms_populateTiles();

tile_s* ms_getTile(s16 x, s16 y);
bool ms_isBomb(s16 x, s16 y);
bool ms_isDiscovered(s16 x, s16 y);
void ms_discoverTile(u16 x, u16 y);
void ms_discoverTiles();


void ms_getTexTileType(tile_s* tile, int* tex_x, int* tex_y);
void ms_getTexTileState(tile_s* tile, int* tex_x, int* tex_y);
void ms_drawTile(u16 x, u16 y);
void ms_drawTiles();


extern minesland_s minesland;
extern sf2d_texture* mineTiles;


class MinesweeperScene : public Scene
{
	public:
		MinesweeperScene();
		~MinesweeperScene();
	
		void initialize() override;
		void destroy() override;
		void drawTopScreen() override;
		void drawBottomScreen() override;
		void updateInput(const keystate_s& ks) override;
};


#endif // MINESWEEPER_SCENE_HPP
