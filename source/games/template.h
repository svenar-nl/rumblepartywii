#ifndef _GAME_TEMPLATE_H_
#define _GAME_TEMPLATE_H_

    #include <stddef.h>
    #include <grrlib.h>
    #include <wiiuse/wpad.h>
    #include "../gfx/game/icons/icon_template.h"

class GAME_TEMPLATE {
    public:
        GRRLIB_texImg* get_icon(void);
        void init(void);
        bool load(void);
        void input(void);
        void update(void);
        void render(void);
};

#endif