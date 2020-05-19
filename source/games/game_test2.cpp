#include "game_test2.h"

GRRLIB_texImg *tex_icon_test2;

GRRLIB_texImg* GAME_TEST2::get_icon(void) {
    return tex_icon_test2;
}

void GAME_TEST2::init(void) {
    tex_icon_test2 = GRRLIB_LoadTexture(icon_test2);
}

bool GAME_TEST2::load(void) {
    return true;
}

void GAME_TEST2::input(void) {

}

void GAME_TEST2::update(void) {

}

void GAME_TEST2::render(void) {

}