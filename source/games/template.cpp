#include "template.h"

GRRLIB_texImg *tex_icon_template;

GRRLIB_texImg* GAME_TEMPLATE::get_icon(void) {
    return tex_icon_template;
}


void GAME_TEMPLATE::init(void) {
    tex_icon_template = GRRLIB_LoadTexture(icon_template);
}

bool GAME_TEMPLATE::load(void) {

}

void GAME_TEMPLATE::input(void) {

}

void GAME_TEMPLATE::update(void) {

}

void GAME_TEMPLATE::render(void) {

}