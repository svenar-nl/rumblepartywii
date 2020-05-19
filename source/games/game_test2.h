#ifndef _GAME_TEST2_H_
#define _GAME_TEST2_H_

    #include <stddef.h>
    #include <grrlib.h>
    #include <wiiuse/wpad.h>
    #include "template.h"
    #include "../gfx/game/icons/icon_test2.h"

class GAME_TEST2: public GAME_TEMPLATE {
    public:
        GRRLIB_texImg* get_icon(void);
        void init(void);
        bool load(void);
        void input(void);
        void update(void);
        void render(void);
};

#endif