#ifndef _GAME_BUTTON_SMASHER_H_
#define _GAME_BUTTON_SMASHER_H_

    #include <stddef.h>
    #include <grrlib.h>
    #include <wiiuse/wpad.h>
    #include "template.h"
    #include "../gfx/game/icons/icon_game_button_smasher.h"
    #include "../gfx/game/background_game_button_smasher.h"
    #include "../gfx/game/font_game_button_smasher.h"
    #include "../util.h"

class GAME_BUTTON_SMASHER: public GAME_TEMPLATE {
    public:
        GRRLIB_texImg* get_icon(void);
        void init(void);
        bool load(void);
        void input(struct Vector2 pointer_positions[4], bool controller_input_arrows_up[4], bool controller_input_arrows_down[4], bool controller_input_arrows_left[4], bool controller_input_arrows_right[4], bool controller_input_a[4], bool controller_input_b[4], bool controller_input_plus[4], bool controller_input_minus[4], bool controller_input_1[4], bool controller_input_2[4]);
        void update(void);
        void render(void);
};

#endif