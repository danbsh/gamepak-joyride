#include <stdio.h>
#include <tonc.h>

#include "../build/ground.h"
#include "../build/player.h"
#include "tonc_input.h"
#include "tonc_memdef.h"
#include "tonc_oam.h"

#define PLAYER_SPEED 2

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE *)obj_buffer;

// TODO: tidy everything (later)
typedef struct _player {
  OBJ_ATTR *obj;
  int pos_x;
  int pos_y;
  int vel_x;
  int vel_y;
  int tid;
  int pb;
  int falling;
} player;

void load_sprites();
void check_collision();
void draw_ground();

int main() {
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);

  load_sprites();
  // TODO: Tilemap bg?
  pal_bg_mem[0] = CLR_SKYBLUE;

  REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;

  player p = {.obj = &obj_buffer[0],
              .pos_x = 96,
              .pos_y = 132,
              .vel_x = 0,
              .vel_y = 0,
              .tid = 0,
              .pb = 0,
              .falling = 0};

  draw_ground();

  obj_set_attr(p.obj, ATTR0_SQUARE, ATTR1_SIZE_16, ATTR2_PALBANK(p.pb) | p.tid);
  obj_set_pos(p.obj, p.pos_x, p.pos_y);

  while (1) {
    VBlankIntrWait();

    vid_vsync();
    key_poll();
    oam_copy(oam_mem, obj_buffer, 64);

    if (key_hit(KEY_LEFT) || key_is_down(KEY_LEFT)) {
      p.vel_x = -1;
    } else if (key_hit(KEY_RIGHT) || key_is_down(KEY_RIGHT)) {
      p.vel_x = 1;
    } else {
      p.vel_x = 0;
    }

    if (p.vel_x != 0) {
      int try_mov = p.pos_x + (p.vel_x * PLAYER_SPEED);

      if (try_mov > 0 && try_mov < 224) {
        p.pos_x = try_mov;
      }

      obj_set_pos(p.obj, p.pos_x, p.pos_y);
    }
  }

  return 0;
}

void load_sprites() {
  memcpy16(&tile_mem[4][0], playerTiles, playerTilesLen);
  memcpy16(pal_obj_bank[0], playerPal, playerPalLen / 2);

  memcpy16(&tile_mem[4][4], groundTiles, groundTilesLen);
  memcpy16(pal_obj_bank[1], groundPal, groundPalLen / 2);

  // TODO: 128???
  oam_init(obj_buffer, 128);
}

void draw_ground() {
  // Probably a better way to do this
  for (int i = 1, c = 0; i <= 24; i++, c += 16) {
    OBJ_ATTR *ground = &obj_buffer[i];
    obj_set_attr(ground, ATTR0_SQUARE, ATTR1_SIZE_16, ATTR2_PALBANK(1) | 4);
    // y is 148 because I messed up the sprite, might fix
    obj_set_pos(ground, c, 148);
  }
}
