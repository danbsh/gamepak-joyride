#include <stdio.h>
#include <tonc.h>

#include "../build/ground.h"
#include "../build/player.h"
#include "tonc_core.h"
#include "tonc_memdef.h"
#include "tonc_memmap.h"
#include "tonc_oam.h"
#include "tonc_types.h"
#include "tonc_video.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE *)obj_buffer;

struct _player {
  OBJ_ATTR obj;
  int pos_x;
  int pos_y;
  int velocity;
  int tid;
  int pb;
} player;

void load_sprites();

int main() {
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);

  load_sprites();

  REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;

  u32 tid = 0, pb = 0;
  OBJ_ATTR *player = &obj_buffer[0];

  // Draw floor
  for (int i = 1, c = 0; i <= 24; i++, c += 16) {
    OBJ_ATTR *ground = &obj_buffer[i];
    obj_set_attr(ground, ATTR0_SQUARE, ATTR1_SIZE_16, ATTR2_PALBANK(1) | 4);
    obj_set_pos(ground, c, 148);
  }

  obj_set_attr(player, ATTR0_SQUARE, ATTR1_SIZE_16, ATTR2_PALBANK(pb) | tid);
  obj_set_pos(player, 96, 32);

  while (1) {
    VBlankIntrWait();

    vid_vsync();
    oam_copy(oam_mem, obj_buffer, 64);
  }

  return 0;
}

void load_sprites() {
  memcpy16(&tile_mem[4][0], playerTiles, playerTilesLen);
  memcpy16(pal_obj_bank[0], playerPal, playerPalLen / 2);

  memcpy16(&tile_mem[4][4], groundTiles, groundTilesLen);
  memcpy16(pal_obj_bank[1], groundPal, groundPalLen / 2);

  pal_bg_mem[0] = CLR_SKYBLUE;

  oam_init(obj_buffer, 128);
}
