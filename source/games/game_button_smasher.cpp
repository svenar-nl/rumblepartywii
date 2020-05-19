#include "game_button_smasher.h"

GRRLIB_texImg *tex_background_game_button_smasher, *tex_font_game_button_smasher, *tex_icon_game_button_smasher;
bool button_a_down[4];
bool tmp_button_a_down[4];
int button_a_presses[0];

GRRLIB_texImg* GAME_BUTTON_SMASHER::get_icon(void) {
    return tex_icon_game_button_smasher;
}

void GAME_BUTTON_SMASHER::init(void) {
    tex_icon_game_button_smasher = GRRLIB_LoadTexture(icon_game_button_smasher);
}

void GAME_BUTTON_SMASHER::input(struct Vector2 pointer_positions[4], bool controller_input_arrows_up[4], bool controller_input_arrows_down[4], bool controller_input_arrows_left[4], bool controller_input_arrows_right[4], bool controller_input_a[4], bool controller_input_b[4], bool controller_input_plus[4], bool controller_input_minus[4], bool controller_input_1[4], bool controller_input_2[4]) {
    for (int i = 0; i < 4; i++) {
        button_a_down[i] = controller_input_a[i];
    }
}

bool GAME_BUTTON_SMASHER::load(void) {
    tex_background_game_button_smasher = GRRLIB_LoadTexture(background_game_button_smasher);

    tex_font_game_button_smasher = GRRLIB_LoadTexture(font_game_button_smasher);
    GRRLIB_InitTileSet(tex_font_game_button_smasher, 8, 16, 0);

    for (int i = 0; i < 4; i++) {
        button_a_presses[i] = 0;
    }

    return true;
}

void GAME_BUTTON_SMASHER::update(void) {
    for (int i = 0; i < 4; i++) {
        if (button_a_down[i]) {
            if (!tmp_button_a_down[i]) {
                tmp_button_a_down[i] = true;
                button_a_presses[i] += 1;
            }
        } else tmp_button_a_down[i] = false;
    }
}

void GAME_BUTTON_SMASHER::render(void) {
    GRRLIB_DrawImg(0, 0, tex_background_game_button_smasher, 0, 1, 1, 0xFFFFFFFF);

    for (int i = 0; i < 4; i++) {
        GRRLIB_Printf(190, 200 + (i * 32), tex_font_game_button_smasher, 0x000080FF, 2, "P%d: %d", i + 1, button_a_presses[i]);
    }
}