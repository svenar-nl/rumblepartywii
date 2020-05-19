#include <grrlib.h>

#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "util.h"
#include "gfx/wii_remote_warning.h"
#include "gfx/background.h"
#include "gfx/logo.h"
#include "gfx/font1.h"
#include "gfx/buttons/button_play.h"
#include "gfx/buttons/button_options.h"
#include "gfx/buttons/button_about.h"
#include "gfx/buttons/button_quit.h"
#include "gfx/buttons/button_resume.h"
#include "gfx/buttons/button_back.h"
#include "gfx/buttons/button_play_selected.h"
#include "gfx/buttons/button_options_selected.h"
#include "gfx/buttons/button_about_selected.h"
#include "gfx/buttons/button_quit_selected.h"
#include "gfx/buttons/button_resume_selected.h"
#include "gfx/buttons/button_back_selected.h"
#include "gfx/irimage1.h"
#include "gfx/irimage2.h"
#include "gfx/irimage3.h"
#include "gfx/irimage4.h"

#include "games/template.h"
#include "games/game_button_smasher.h"
#include "games/game_test2.h"

#define SCRX            rmode->fbWidth
#define SCRY            rmode->efbHeight
#define MAX_WIIMOTES    4

ir_t ir[MAX_WIIMOTES];
int rumble_timer[MAX_WIIMOTES];
int player_score[MAX_WIIMOTES];

GAME_BUTTON_SMASHER game_button_smasher;
GAME_TEST2 game_test2;

GRRLIB_texImg 
    *tex_wii_remote_warning,
    *tex_background,
    *tex_logo,
    *tex_font1,
    *tex_button_play,
    *tex_button_options,
    *tex_button_about,
    *tex_button_quit,
    *tex_button_resume,
    *tex_button_back,
    *tex_button_play_selected,
    *tex_button_options_selected,
    *tex_button_about_selected,
    *tex_button_quit_selected,
    *tex_button_resume_selected,
    *tex_button_back_selected,
    *tex_irimage1,
    *tex_irimage2,
    *tex_irimage3,
    *tex_irimage4
;

void startup_game_screen(GRRLIB_texImg *tex_wii_remote_warning, GRRLIB_texImg *tex_background, GRRLIB_texImg *tex_font1);
void exit_game(void);

int main() {
    float logo_effect = 0.0;
    int current_game_id = -1;

    bool
        button_play_is_selected[MAX_WIIMOTES], tmp_button_play_is_selected[MAX_WIIMOTES],
        button_options_is_selected[MAX_WIIMOTES], tmp_button_options_is_selected[MAX_WIIMOTES],
        button_about_is_selected[MAX_WIIMOTES], tmp_button_about_is_selected[MAX_WIIMOTES],
        button_quit_is_selected[MAX_WIIMOTES], tmp_button_quit_is_selected[MAX_WIIMOTES],
        button_resume_is_selected[MAX_WIIMOTES], tmp_button_resume_is_selected[MAX_WIIMOTES],
        button_back_is_selected[MAX_WIIMOTES], tmp_button_back_is_selected[MAX_WIIMOTES],

        button_game0_is_selected[MAX_WIIMOTES],
        button_game1_is_selected[MAX_WIIMOTES],
        button_game2_is_selected[MAX_WIIMOTES],
        button_game3_is_selected[MAX_WIIMOTES],
        button_game4_is_selected[MAX_WIIMOTES],
        button_game5_is_selected[MAX_WIIMOTES],
        button_game6_is_selected[MAX_WIIMOTES],
        button_game7_is_selected[MAX_WIIMOTES]
    ;

    typedef enum {
		MAINMENU,
		GAMESELECTOR,
		OPTIONS,
		ABOUT,
		LOADGAME,
		GAME
	} states;

	states state = MAINMENU;

    GRRLIB_Init();
    WPAD_Init();

    for (int i = 0; i < MAX_WIIMOTES; i++) {
        WPAD_SetDataFormat(i, WPAD_FMT_BTNS_ACC_IR);
        WPAD_SetVRes(i, SCRX, SCRY);
    }

    GRRLIB_Settings.antialias = false;

    tex_wii_remote_warning = GRRLIB_LoadTexture(wii_remote_warning);
    tex_background = GRRLIB_LoadTexture(background);
    tex_logo = GRRLIB_LoadTexture(logo);
    tex_font1 = GRRLIB_LoadTexture(font1);
    GRRLIB_InitTileSet(tex_font1, 8, 16, 0);

    tex_button_play = GRRLIB_LoadTexture(button_play);
    tex_button_options = GRRLIB_LoadTexture(button_options);
    tex_button_about = GRRLIB_LoadTexture(button_about);
    tex_button_quit = GRRLIB_LoadTexture(button_quit);
    tex_button_resume = GRRLIB_LoadTexture(button_resume);
    tex_button_back = GRRLIB_LoadTexture(button_back);
    tex_button_play_selected = GRRLIB_LoadTexture(button_play_selected);
    tex_button_options_selected = GRRLIB_LoadTexture(button_options_selected);
    tex_button_about_selected = GRRLIB_LoadTexture(button_about_selected);
    tex_button_quit_selected = GRRLIB_LoadTexture(button_quit_selected);
    tex_button_resume_selected = GRRLIB_LoadTexture(button_resume_selected);
    tex_button_back_selected = GRRLIB_LoadTexture(button_back_selected);

    tex_irimage1 = GRRLIB_LoadTexture(irimage1);
    tex_irimage2 = GRRLIB_LoadTexture(irimage2);
    tex_irimage3 = GRRLIB_LoadTexture(irimage3);
    tex_irimage4 = GRRLIB_LoadTexture(irimage4);

    game_button_smasher.init();
    game_test2.init();

    // startup_game_screen(tex_wii_remote_warning, tex_background, tex_font1);

    while(1) {
        WPAD_ScanPads();

        u32 WPADKeyDown[MAX_WIIMOTES];

        for (int i = 0; i < MAX_WIIMOTES; i++) {
            WPAD_IR(i, &ir[i]);
            WPADKeyDown[i] = WPAD_ButtonsDown(i);

            if (rumble_timer[i] > 0) {
                rumble_timer[i]--;
                WPAD_Rumble(i, 1);
            } else {
                WPAD_Rumble(i, 0);
            }
        }

        if (WPADKeyDown[0] & WPAD_BUTTON_HOME) { // Only player 1 can exit using the home button.
			if (state == GAME) {
				//game_paused = !game_paused;
			} else break;
		}

        GRRLIB_FillScreen(0x00000000);

        logo_effect += 0.02;
        switch (state) {
			case MAINMENU: {
                GRRLIB_texImg *tmp_tex_button_play = tex_button_play;
                GRRLIB_texImg *tmp_tex_button_options = tex_button_options;
                GRRLIB_texImg *tmp_tex_button_about = tex_button_about;
                GRRLIB_texImg *tmp_tex_button_quit = tex_button_quit;

                for (int i = 0; i < MAX_WIIMOTES; i++) {
                    button_play_is_selected[i] = ir[i].x > SCRX / 2 - tex_button_play->w / 2 && ir[i].x < SCRX / 2 + tex_button_play->w / 2 && ir[i].y > 100 && ir[i].y < 100 + tex_button_play->h;
                    button_options_is_selected[i] = ir[i].x > SCRX / 2 - tex_button_options->w / 2 && ir[i].x < SCRX / 2 + tex_button_options->w / 2 && ir[i].y > 200 && ir[i].y < 200 + tex_button_options->h;
                    button_about_is_selected[i] = ir[i].x > SCRX / 2 - tex_button_about->w / 2 && ir[i].x < SCRX / 2 + tex_button_about->w / 2 && ir[i].y > 300 && ir[i].y < 300 + tex_button_about->h;
                    button_quit_is_selected[i] = ir[i].x > SCRX / 2 - tex_button_quit->w / 2 && ir[i].x < SCRX / 2 + tex_button_quit->w / 2 && ir[i].y > 400 && ir[i].y < 400 + tex_button_quit->h;

                    if (button_play_is_selected[i]) {
                        if (button_play_is_selected[i] != tmp_button_play_is_selected[i]) {

                            rumble_timer[i] = 4;
                            tmp_button_play_is_selected[i] = button_play_is_selected[i];
                        }
                        tmp_tex_button_play = tex_button_play_selected;
                    }

                    if (button_options_is_selected[i]) {
                        if (button_options_is_selected[i] != tmp_button_options_is_selected[i]) {

                            rumble_timer[i] = 4;
                            tmp_button_options_is_selected[i] = button_options_is_selected[i];
                        }
                        tmp_tex_button_options = tex_button_options_selected;
                    }

                    if (button_about_is_selected[i]) {
                        if (button_about_is_selected[i] != tmp_button_about_is_selected[i]) {

                            rumble_timer[i] = 4;
                            tmp_button_about_is_selected[i] = button_about_is_selected[i];
                        }
                        tmp_tex_button_about = tex_button_about_selected;
                    }

                    if (button_quit_is_selected[i]) {
                        if (button_quit_is_selected[i] != tmp_button_quit_is_selected[i]) {
                                
                            rumble_timer[i] = 4;
                            tmp_button_quit_is_selected[i] = button_quit_is_selected[i];
                        }
                        tmp_tex_button_quit = tex_button_quit_selected;
                    }

                    if (WPADKeyDown[i] && WPAD_BUTTON_A) {
                        rumble_timer[i] = 8;
                        if (button_play_is_selected[i]) {
                            state = GAMESELECTOR;
                        }

                        if (button_options_is_selected[i]) {
                            state = OPTIONS;
                        }

                        if (button_about_is_selected[i]) {
                            state = ABOUT;
                        }

                        if (button_quit_is_selected[i]) {
                            exit_game();
                        }
                    }   
                }

                GRRLIB_DrawImg(0, 0, tex_background, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tex_logo->w / 2 + cos(logo_effect) * 15, sin(logo_effect) * 5 + 5, tex_logo, 0, 1, 1, 0xFFFFFFFF);

                GRRLIB_DrawImg(SCRX / 2 - tmp_tex_button_play->w / 2, 100, tmp_tex_button_play, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tmp_tex_button_options->w / 2, 200, tmp_tex_button_options, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tmp_tex_button_about->w / 2, 300, tmp_tex_button_about, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tmp_tex_button_quit->w / 2, 400, tmp_tex_button_quit, 0, 1, 1, 0xFFFFFFFF);
                break;
            }
            
            case GAMESELECTOR: {
                GRRLIB_texImg *tmp_tex_button_back = tex_button_back;
                bool has_game0_selected = false;
                bool has_game1_selected = false;
                bool has_game2_selected = false;
                bool has_game3_selected = false;
                bool has_game4_selected = false;
                bool has_game5_selected = false;
                bool has_game6_selected = false;
                bool has_game7_selected = false;

                for (int i = 0; i < MAX_WIIMOTES; i++) {
                    button_back_is_selected[i] = ir[i].x > SCRX / 2 - tex_button_back->w / 2 && ir[i].x < SCRX / 2 + tex_button_back->w / 2 && ir[i].y > 400 && ir[i].y < 400 + tex_button_back->h;
                    button_game0_is_selected[i] = ir[i].x > 20 && ir[i].x < 20 + tex_button_play->w * .43 && ir[i].y > 182 && ir[i].y < 182 + tex_button_play->h * .5;
                    button_game1_is_selected[i] = ir[i].x > 180 && ir[i].x < 180 + tex_button_play->w * .43 && ir[i].y > 182 && ir[i].y < 182 + tex_button_play->h * .5;
                    button_game2_is_selected[i] = ir[i].x > 340 && ir[i].x < 340 + tex_button_play->w * .43 && ir[i].y > 182 && ir[i].y < 182 + tex_button_play->h * .5;
                    button_game3_is_selected[i] = ir[i].x > 500 && ir[i].x < 500 + tex_button_play->w * .43 && ir[i].y > 182 && ir[i].y < 182 + tex_button_play->h * .5;
                    button_game4_is_selected[i] = ir[i].x > 20 && ir[i].x < 20 + tex_button_play->w * .43 && ir[i].y > 330 && ir[i].y < 330 + tex_button_play->h * .5;
                    button_game5_is_selected[i] = ir[i].x > 180 && ir[i].x < 180 + tex_button_play->w * .43 && ir[i].y > 330 && ir[i].y < 330 + tex_button_play->h * .5;
                    button_game6_is_selected[i] = ir[i].x > 340 && ir[i].x < 340 + tex_button_play->w * .43 && ir[i].y > 330 && ir[i].y < 330 + tex_button_play->h * .5;
                    button_game7_is_selected[i] = ir[i].x > 500 && ir[i].x < 500 + tex_button_play->w * .43 && ir[i].y > 330 && ir[i].y < 330 + tex_button_play->h * .5;

                    if (button_game0_is_selected[i]) has_game0_selected = true;
                    if (button_game1_is_selected[i]) has_game1_selected = true;
                    if (button_game2_is_selected[i]) has_game2_selected = true;
                    if (button_game3_is_selected[i]) has_game3_selected = true;
                    if (button_game4_is_selected[i]) has_game4_selected = true;
                    if (button_game5_is_selected[i]) has_game5_selected = true;
                    if (button_game6_is_selected[i]) has_game6_selected = true;
                    if (button_game7_is_selected[i]) has_game7_selected = true;

                    if (button_back_is_selected[i]) {
                        if (button_back_is_selected[i] != tmp_button_back_is_selected[i]) {

                            rumble_timer[i] = 4;
                            tmp_button_back_is_selected[i] = button_back_is_selected[i];
                        }
                        tmp_tex_button_back = tex_button_back_selected;
                    }

                    if (WPADKeyDown[i] && WPAD_BUTTON_A) {
                        rumble_timer[i] = 8;
                        if (button_back_is_selected[i]) {
                            state = MAINMENU;
                        }

                        if (has_game0_selected) current_game_id = 0;
                        if (has_game1_selected) current_game_id = 1;
                        if (has_game2_selected) current_game_id = 2;
                        if (has_game3_selected) current_game_id = 3;
                        if (has_game4_selected) current_game_id = 4;
                        if (has_game5_selected) current_game_id = 5;
                        if (has_game6_selected) current_game_id = 6;
                        if (has_game7_selected) current_game_id = 7;

                        if (has_game0_selected || has_game1_selected || has_game2_selected || has_game3_selected || has_game4_selected || has_game5_selected || has_game6_selected || has_game7_selected) {
                            state = LOADGAME;
                        }
                    }
                }

                GRRLIB_DrawImg(0, 0, tex_background, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tex_logo->w / 2 + cos(logo_effect) * 15, sin(logo_effect) * 5 + 5, tex_logo, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tex_logo->w / 2 + cos(logo_effect) * 15, sin(logo_effect) * 5 + 5, tex_logo, 0, 1, 1, 0xFFFFFFFF);

                GRRLIB_DrawImg(SCRX / 2 - tmp_tex_button_back->w / 2, 400, tmp_tex_button_back, 0, 1, 1, 0xFFFFFFFF);

                GRRLIB_DrawImg(20, 80, game_button_smasher.get_icon(), 0, .4, .4, 0xFFFFFFFF);
                GRRLIB_DrawImg(180, 80, game_test2.get_icon(), 0, .4, .4, 0xFFFFFFFF);
                GRRLIB_DrawImg(340, 80, game_test2.get_icon(), 0, .4, .4, 0xFFFFFFFF);
                GRRLIB_DrawImg(500, 80, game_test2.get_icon(), 0, .4, .4, 0xFFFFFFFF);

                GRRLIB_DrawImg(20, 228, game_test2.get_icon(), 0, .4, .4, 0xFFFFFFFF);
                GRRLIB_DrawImg(180, 228, game_test2.get_icon(), 0, .4, .4, 0xFFFFFFFF);
                GRRLIB_DrawImg(340, 228, game_test2.get_icon(), 0, .4, .4, 0xFFFFFFFF);
                GRRLIB_DrawImg(500, 228, game_test2.get_icon(), 0, .4, .4, 0xFFFFFFFF);

                GRRLIB_DrawImg(20, 182, has_game0_selected ? tex_button_play_selected : tex_button_play, 0, .43, .5, 0xFFFFFFFF);
                GRRLIB_DrawImg(180, 182, has_game1_selected ? tex_button_play_selected : tex_button_play, 0, .43, .5, 0xFFFFFFFF);
                GRRLIB_DrawImg(340, 182, has_game2_selected ? tex_button_play_selected : tex_button_play, 0, .43, .5, 0xFFFFFFFF);
                GRRLIB_DrawImg(500, 182, has_game3_selected ? tex_button_play_selected : tex_button_play, 0, .43, .5, 0xFFFFFFFF);

                GRRLIB_DrawImg(20, 330, has_game4_selected ? tex_button_play_selected : tex_button_play, 0, .43, .5, 0xFFFFFFFF);
                GRRLIB_DrawImg(180, 330, has_game5_selected ? tex_button_play_selected : tex_button_play, 0, .43, .5, 0xFFFFFFFF);
                GRRLIB_DrawImg(340, 330, has_game6_selected ? tex_button_play_selected : tex_button_play, 0, .43, .5, 0xFFFFFFFF);
                GRRLIB_DrawImg(500, 330, has_game7_selected ? tex_button_play_selected : tex_button_play, 0, .43, .5, 0xFFFFFFFF);

				break;
            }

			case OPTIONS: {
                GRRLIB_texImg *tmp_tex_button_back = tex_button_back;

                for (int i = 0; i < MAX_WIIMOTES; i++) {
                    button_back_is_selected[i] = ir[i].x > SCRX / 2 - tex_button_back->w / 2 && ir[i].x < SCRX / 2 + tex_button_back->w / 2 && ir[i].y > 400 && ir[i].y < 400 + tex_button_back->h;

                    if (button_back_is_selected[i]) {
                        if (button_back_is_selected[i] != tmp_button_back_is_selected[i]) {

                            rumble_timer[i] = 4;
                            tmp_button_back_is_selected[i] = button_back_is_selected[i];
                        }
                        tmp_tex_button_back = tex_button_back_selected;
                    }

                    if (WPADKeyDown[i] && WPAD_BUTTON_A) {
                        rumble_timer[i] = 8;
                        if (button_back_is_selected[i]) {
                            state = MAINMENU;
                        }
                    }
                }

                GRRLIB_DrawImg(0, 0, tex_background, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tex_logo->w / 2 + cos(logo_effect) * 15, sin(logo_effect) * 5 + 5, tex_logo, 0, 1, 1, 0xFFFFFFFF);

                GRRLIB_DrawImg(SCRX / 2 - tmp_tex_button_back->w / 2, 400, tmp_tex_button_back, 0, 1, 1, 0xFFFFFFFF);
				break;
            }
			
			case ABOUT: {
                GRRLIB_texImg *tmp_tex_button_back = tex_button_back;

                for (int i = 0; i < MAX_WIIMOTES; i++) {
                    button_back_is_selected[i] = ir[i].x > SCRX / 2 - tex_button_back->w / 2 && ir[i].x < SCRX / 2 + tex_button_back->w / 2 && ir[i].y > 400 && ir[i].y < 400 + tex_button_back->h;

                    if (button_back_is_selected[i]) {
                        if (button_back_is_selected[i] != tmp_button_back_is_selected[i]) {

                            rumble_timer[i] = 4;
                            tmp_button_back_is_selected[i] = button_back_is_selected[i];
                        }
                        tmp_tex_button_back = tex_button_back_selected;
                    }

                    if (WPADKeyDown[i] && WPAD_BUTTON_A) {
                        rumble_timer[i] = 8;
                        if (button_back_is_selected[i]) {
                            state = MAINMENU;
                        }
                    }
                }

                GRRLIB_DrawImg(0, 0, tex_background, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tex_logo->w / 2 + cos(logo_effect) * 15, sin(logo_effect) * 5 + 5, tex_logo, 0, 1, 1, 0xFFFFFFFF);

                char text[10][80];
                strcpy(text[0], "Thank you for playing!");
                strcpy(text[1], "");
                strcpy(text[2], "Programming and design by:");
                strcpy(text[3], "Sven Arends");
                strcpy(text[4], "https://svenar.nl");
                strcpy(text[5], "");
                strcpy(text[6], "Thanks to the following people");
                strcpy(text[7], "for helping with minigame ideas!");
                strcpy(text[8], "Marnix, Mitchell");
                strcpy(text[9], "<3");

                for (int i = 0; i < 10; i++) {
                    GRRLIB_Printf(0, 80 + (i * 32), tex_font1, 0x00008000 | 255 - i * 20, 2, text[i]);
                }

                GRRLIB_DrawImg(SCRX / 2 - tmp_tex_button_back->w / 2, 400, tmp_tex_button_back, 0, 1, 1, 0xFFFFFFFF);
				break;
            }
			
			case LOADGAME: {
                bool loading_done = false;

                GRRLIB_DrawImg(0, 0, tex_background, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_DrawImg(SCRX / 2 - tex_logo->w / 2 + cos(logo_effect) * 15, sin(logo_effect) * 5 + 5, tex_logo, 0, 1, 1, 0xFFFFFFFF);
                GRRLIB_Printf(SCRX / 2 - (10 * 16) / 2, SCRY / 2 - 8 / 2, tex_font1, 0x000080FF, 2, "Loading...");

                if (current_game_id == 0) loading_done = game_button_smasher.load();
                if (current_game_id == 1) loading_done = game_test2.load();

                if (current_game_id == 2) loading_done = game_test2.load();
                if (current_game_id == 3) loading_done = game_test2.load();
                if (current_game_id == 4) loading_done = game_test2.load();
                if (current_game_id == 5) loading_done = game_test2.load();
                if (current_game_id == 6) loading_done = game_test2.load();
                if (current_game_id == 7) loading_done = game_test2.load();

                if (loading_done) state = GAME;
				break;
            }
			
			case GAME: {
                struct Vector2 pointer_positions[MAX_WIIMOTES];
                bool controller_input_arrows_up[MAX_WIIMOTES];
				bool controller_input_arrows_down[MAX_WIIMOTES];
				bool controller_input_arrows_left[MAX_WIIMOTES];
				bool controller_input_arrows_right[MAX_WIIMOTES];
				bool controller_input_a[MAX_WIIMOTES];
				bool controller_input_b[MAX_WIIMOTES];
				bool controller_input_plus[MAX_WIIMOTES];
				bool controller_input_minus[MAX_WIIMOTES];
				bool controller_input_1[MAX_WIIMOTES];
				bool controller_input_2[MAX_WIIMOTES];

                for (int i = 0; i < MAX_WIIMOTES; i++) {
                    pointer_positions[i] = {ir[i].x, ir[i].y};
                    controller_input_arrows_up[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_UP;
					controller_input_arrows_down[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_DOWN;
					controller_input_arrows_left[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_LEFT;
					controller_input_arrows_right[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_RIGHT;
					controller_input_a[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_A;
					controller_input_b[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_B;
					controller_input_plus[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_PLUS;
					controller_input_minus[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_MINUS;
					controller_input_1[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_1;
					controller_input_2[i] = WPAD_ButtonsHeld(i) & WPAD_BUTTON_2;
                }
                if (current_game_id == 0) {
                    game_button_smasher.input(pointer_positions, controller_input_arrows_up, controller_input_arrows_down, controller_input_arrows_left, controller_input_arrows_right, controller_input_a, controller_input_b, controller_input_plus, controller_input_minus, controller_input_1, controller_input_2);
                    game_button_smasher.update();
                    game_button_smasher.render();
                }
				break;
            }
			
			default: {
				state = MAINMENU;
				break;
            }
        }

        if (ir[0].raw_valid) GRRLIB_DrawImg(ir[0].x, ir[0].y, tex_irimage1, ir[0].angle, 1, 1, 0xFFFFFFFF);
        if (ir[1].raw_valid) GRRLIB_DrawImg(ir[1].x, ir[1].y, tex_irimage2, ir[1].angle, 1, 1, 0xFFFFFFFF);
        if (ir[2].raw_valid) GRRLIB_DrawImg(ir[2].x, ir[2].y, tex_irimage3, ir[2].angle, 1, 1, 0xFFFFFFFF);
        if (ir[3].raw_valid) GRRLIB_DrawImg(ir[3].x, ir[3].y, tex_irimage4, ir[3].angle, 1, 1, 0xFFFFFFFF);

        GRRLIB_Render();

    }
    
    exit_game();
}

void startup_game_screen(GRRLIB_texImg *tex_wii_remote_warning, GRRLIB_texImg *tex_background, GRRLIB_texImg *tex_font1) {
    for(int i = 0; i < 255; i += 4) {
        GRRLIB_DrawImg(0, 0, tex_wii_remote_warning, 0, SCRX / (float)tex_wii_remote_warning->w, SCRY / (float)tex_wii_remote_warning->h, 0xFFFFFF00|i);
        GRRLIB_Render();
    }
    for(int i = 0; i < 255; i += 1) {
        GRRLIB_DrawImg(0, 0, tex_wii_remote_warning, 0, SCRX / (float)tex_wii_remote_warning->w, SCRY / (float)tex_wii_remote_warning->h, 0xFFFFFFFF);
        GRRLIB_Render();
    }
    for(int i = 255; i >= 0; i -= 4) {
        GRRLIB_DrawImg(0, 0, tex_wii_remote_warning, 0, SCRX / (float)tex_wii_remote_warning->w, SCRY / (float)tex_wii_remote_warning->h, 0xFFFFFF00|i);
        GRRLIB_Render();
    }

    for(int i = 0; i < 255; i += 1) {
        GRRLIB_Printf(SCRX / 2 - 240 / 2, SCRY / 2 - 48 * 2, tex_font1, 0xFFFFFF00|i, 3, "A game by:");
        GRRLIB_Printf(SCRX / 2 - 264 / 2, SCRY / 2 - 48, tex_font1, 0xFFFFFF00|i, 3, "SVEN ARENDS");
        GRRLIB_Printf(SCRX / 2 - 408 / 2, SCRY / 2 + 48, tex_font1, 0xFFFFFF00|i, 3, "https://svenar.nl");
        
        GRRLIB_Render();
    }
    for(int i = 255; i > 0; i -= 2) {
        GRRLIB_Printf(SCRX / 2 - 240 / 2, SCRY / 2 - 48 * 2, tex_font1, 0xFFFFFF00|i, 3, "A game by:");
        GRRLIB_Printf(SCRX / 2 - 264 / 2, SCRY / 2 - 48, tex_font1, 0xFFFFFF00|i, 3, "SVEN ARENDS");
        GRRLIB_Printf(SCRX / 2 - 408 / 2, SCRY / 2 + 48, tex_font1, 0xFFFFFF00|i, 3, "https://svenar.nl");

        GRRLIB_Render();
    }

    for (int i = 0; i <= 255; i += 8) {
        GRRLIB_DrawImg(0, 0, tex_background, 0, 1, 1, 0xFFFFFF00 | i);

        GRRLIB_Render();
    }
}

void exit_game(void) {
    GRRLIB_FreeTexture(tex_wii_remote_warning);
    GRRLIB_FreeTexture(tex_background);
    GRRLIB_FreeTexture(tex_logo);
    GRRLIB_FreeTexture(tex_font1);
    GRRLIB_FreeTexture(tex_button_play);
    GRRLIB_FreeTexture(tex_button_options);
    GRRLIB_FreeTexture(tex_button_about);
    GRRLIB_FreeTexture(tex_button_quit);
    GRRLIB_FreeTexture(tex_button_resume);
    GRRLIB_FreeTexture(tex_button_back);
    GRRLIB_FreeTexture(tex_button_play_selected);
    GRRLIB_FreeTexture(tex_button_options_selected);
    GRRLIB_FreeTexture(tex_button_about_selected);
    GRRLIB_FreeTexture(tex_button_quit_selected);
    GRRLIB_FreeTexture(tex_button_resume_selected);
    GRRLIB_FreeTexture(tex_button_back_selected);
    GRRLIB_FreeTexture(tex_irimage1);
    GRRLIB_FreeTexture(tex_irimage2);
    GRRLIB_FreeTexture(tex_irimage3);
    GRRLIB_FreeTexture(tex_irimage4);

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
    exit(0);
}