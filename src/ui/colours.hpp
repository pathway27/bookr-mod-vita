// TODO: Namespace this

#ifndef COLOURS_HPP
#define COLOURS_HPP

#define RGBA8(r,g,b,a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))

#define BLACK RGBA8(0,0,0,255)
#define WHITE RGBA8(255,255,255,255)

// grays

#define DIM_GRAY RGBA8(105,105,105,155)
#define WOOD_BARK RGBA8(29,22,22,255)
#define WHITE_SMOKE RGBA8(235,235,235,255)
#define NERO RGBA8(34,34,34,255)

// reds
#define LIGHT_RED RGBA8(240, 0, 0, 255)

  // transparents

  #define ALIZARIN RGBA8(240, 34, 34, 34) // 0xf0222222
  #define TOMATO RGBA8(255, 85, 85, 85) // 0xff555555
  #define CHERUB RGBA8(240, 204, 204, 204) // 0xf0cccccc
#endif