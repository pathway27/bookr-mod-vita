/**
 * Control the screen and rendering. This class is very biased
 * towards libgu on the PSP. The GL backend is just for devel;
 * it is slow and incomplete.
 */

// 100% replaceable with pure C, kinda sucks right now, but does
// its job of isolating libGU and GL

// TODO: Move these to fz graphics header
#define FZ_COLOR_BUFFER 1
#define FZ_DEPTH_BUFFER 2

#define FZ_SRC_COLOR 0
#define FZ_ONE_MINUS_SRC_COLOR 1
#define FZ_SRC_ALPHA 2
#define FZ_ONE_MINUS_SRC_ALPHA 3
#define FZ_DST_COLOR 0
#define FZ_ONE_MINUS_DST_COLOR 1

#define FZ_ADD 0
#define FZ_SUBTRACT 1

#define FZ_FLAT 0
#define FZ_SMOOTH 1

#define FZ_ALPHA_TEST 0
#define FZ_DEPTH_TEST 1
#define FZ_SCISSOR_TEST 2
#define FZ_STENCIL_TEST 3
#define FZ_GL_BLEND 4
#define FZ_CULL_FACE 5
#define FZ_DITHER 6
#define FZ_FOG 7
#define FZ_CLIP_PLANES 8
#define FZ_TEXTURE_2D 9
#define FZ_LIGHTING 10
#define FZ_LIGHT0 11
#define FZ_LIGHT1 12
#define FZ_LIGHT2 13
#define FZ_LIGHT3 14
//#define GU_UNKNOWN_15           (15)
//#define GU_UNKNOWN_16           (16)
#define FZ_COLOR_TEST 17
#define FZ_COLOR_LOGIC_OP 18
#define FZ_FACE_NORMAL_REVERSE 19
#define FZ_PATCH_FACE 20
#define FZ_FRAGMENT_2X 21

#define FZ_POINTS (0)
#define FZ_LINES (1)
#define FZ_LINE_STRIP (2)
#define FZ_TRIANGLES (3)
#define FZ_TRIANGLE_STRIP (4)
#define FZ_TRIANGLE_FAN (5)
#define FZ_SPRITES (6)

#define FZ_TEXTURE_SHIFT(n) ((n) << 0)
#define FZ_TEXTURE_8BIT FZ_TEXTURE_SHIFT(1)
#define FZ_TEXTURE_16BIT FZ_TEXTURE_SHIFT(2)
#define FZ_TEXTURE_32BITF FZ_TEXTURE_SHIFT(3)

#define FZ_COLOR_SHIFT(n) ((n) << 2)
#define FZ_COLOR_RES1 FZ_COLOR_SHIFT(1)
#define FZ_COLOR_RES2 FZ_COLOR_SHIFT(2)
#define FZ_COLOR_RES3 FZ_COLOR_SHIFT(3)
#define FZ_COLOR_5650 FZ_COLOR_SHIFT(4)
#define FZ_COLOR_5551 FZ_COLOR_SHIFT(5)
#define FZ_COLOR_4444 FZ_COLOR_SHIFT(6)
#define FZ_COLOR_8888 FZ_COLOR_SHIFT(7)

#define FZ_NORMAL_SHIFT(n) ((n) << 5)
#define FZ_NORMAL_8BIT FZ_NORMAL_SHIFT(1)
#define FZ_NORMAL_16BIT FZ_NORMAL_SHIFT(2)
#define FZ_NORMAL_32BITF FZ_NORMAL_SHIFT(3)

#define FZ_VERTEX_SHIFT(n) ((n) << 7)
#define FZ_VERTEX_8BIT FZ_VERTEX_SHIFT(1)
#define FZ_VERTEX_16BIT FZ_VERTEX_SHIFT(2)
#define FZ_VERTEX_32BITF FZ_VERTEX_SHIFT(3)

#define FZ_WEIGHT_SHIFT(n) ((n) << 9)
#define FZ_WEIGHT_8BIT FZ_WEIGHT_SHIFT(1)
#define FZ_WEIGHT_16BIT FZ_WEIGHT_SHIFT(2)
#define FZ_WEIGHT_32BITF FZ_WEIGHT_SHIFT(3)

#define FZ_INDEX_SHIFT(n) ((n) << 11)
#define FZ_INDEX_8BIT FZ_INDEX_SHIFT(1)
#define FZ_INDEX_16BIT FZ_INDEX_SHIFT(2)
#define FZ_INDEX_RES3 FZ_INDEX_SHIFT(3)

#define FZ_TRANSFORM_SHIFT(n) ((n) << 23)
#define FZ_TRANSFORM_3D FZ_TRANSFORM_SHIFT(0)
#define FZ_TRANSFORM_2D FZ_TRANSFORM_SHIFT(1)

#define FZ_PSM_5650 (0) /* Display, Texture, Palette */
#define FZ_PSM_5551 (1) /* Display, Texture, Palette */
#define FZ_PSM_4444 (2) /* Display, Texture, Palette */
#define FZ_PSM_8888 (3) /* Display, Texture, Palette */
#define FZ_PSM_T4 (4)   /* Texture */
#define FZ_PSM_T8 (5)   /* Texture */

#ifdef DESKTOP
#define FZ_TEX_MODULATE  0
#define FZ_TEX_DECAL     1
#define FZ_TEX_BLEND     2
#define FZ_TEX_REPLACE   3
#define FZ_TEX_ADD       4

#define FZ_NEAREST                 0
#define FZ_LINEAR                  1
#define FZ_NEAREST_MIPMAP_NEAREST  4
#define FZ_LINEAR_MIPMAP_NEAREST   5
#define FZ_NEAREST_MIPMAP_LINEAR   6
#define FZ_LINEAR_MIPMAP_LINEAR    7
#endif