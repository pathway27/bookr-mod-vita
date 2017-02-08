#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include "bookrconfig.h"

#include "bkdocument.h"

// res 960 × 544

int main(int argc, char *argv[]) {
    BKDocument *documentLayer = 0;
    
    vita2d_pgf *pgf;
    vita2d_pvf *pvf;


    vita2d_init();
    vita2d_set_clear_color(RGBA8(255, 0, 0, 255));

    pgf = vita2d_load_default_pgf();
    pvf = vita2d_load_default_pvf();

    while (1) {
      vita2d_start_drawing();
      vita2d_clear_screen();

      vita2d_pgf_draw_text(pgf, 700, 30, RGBA8(0,255,0,255), 1.0f, "Hello in PGF");
      #ifdef __vita__
      vita2d_pgf_draw_text(pgf, 700, 514, RGBA8(0,255,0,255), 1.0f, GIT_VERSION);
      #endif

      vita2d_end_drawing();
      vita2d_swap_buffers();
    }

    vita2d_fini();
    vita2d_free_pgf(pgf);
    vita2d_free_pvf(pvf);

    sceKernelExitProcess(0);
    return 0;
}
