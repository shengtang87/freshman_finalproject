// [main.c]
// this template is provided for the 2D shooter game.

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
#include <allegro5/algif5-master/src/algif.h>

// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED

/* Constants. */

// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 800;
// Display (screen) height.
const int SCREEN_H = 600;
// At most 10 audios can be played at a time.
const int RESERVE_SAMPLES = 10;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
enum {
    SCENE_MENU = 1,
    SCENE_START = 2,
    // [HACKATHON 3-7]
    // TODO: Declare a new scene id.
    SCENE_SETTINGS = 3,
	SCENE_BOSS = 4,
	SCENE_WIN = 5,
	SCENE_LOSE = 6
};

/* Input states */

// The active scene id.
int active_scene;
// Keyboard state, whether the key is down or not.
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool *mouse_state;
// Mouse position.
int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...

/* Variables for allegro basic routines. */

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

/* Shared resources*/

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
// TODO: More shared resources or data that needed to be accessed
// across different scenes.

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;
ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;
ALLEGRO_BITMAP* img_mute;
ALLEGRO_BITMAP* img_vol;
// [HACKATHON 3-1]
// TODO: Declare 2 variables for storing settings images.
// Uncomment and fill in the code below.
ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;
ALLEGRO_SAMPLE* meow;
ALLEGRO_SAMPLE_ID meow_id;
ALLEGRO_SAMPLE* lose;
ALLEGRO_SAMPLE_ID lose_id;
ALLEGRO_SAMPLE* win;
ALLEGRO_SAMPLE_ID win_id;
ALLEGRO_SAMPLE* warn;
ALLEGRO_SAMPLE_ID warn_id;
ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;
ALLEGRO_SAMPLE* boss_bgm;
ALLEGRO_SAMPLE_ID boss_bgm_id;
ALLEGRO_SAMPLE* setting_bgm;
ALLEGRO_SAMPLE_ID setting_bgm_id;

/* Start Scene resources*/
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* win_img_background;
ALLEGRO_BITMAP* setting_img_background;
ALLEGRO_BITMAP* lose_img_background;
ALLEGRO_BITMAP* boss_img_background;
ALLEGRO_BITMAP* start_img_plane;
ALLEGRO_BITMAP* img_boss;
ALLEGRO_BITMAP* img_wing;
ALLEGRO_BITMAP* start_img_enemy;
ALLEGRO_BITMAP* img_hp;
ALLEGRO_BITMAP* img_can;
ALLEGRO_BITMAP* img_bullet;
ALLEGRO_BITMAP* img_enebullet;
ALLEGRO_BITMAP* img_bossbullet;
ALLEGRO_BITMAP* bosshp;
ALLEGRO_BITMAP* img_warning;
// [HACKATHON 2-1]
// TODO: Declare a variable to store your bullet's image.
// Uncomment and fill in the code below.

typedef struct {
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
	int hp;
    // The pointer to the object’s image.
    ALLEGRO_BITMAP* img;
} MovableObject;
void draw_movable_object(MovableObject obj);
#define MAX_ENEMY 5
#define MAX_HP 100
#define nextlv 20
// [HACKATHON 2-2]
// TODO: Declare the max bullet count that will show on screen.
// You can try max 4 bullets here and see if you needed more.
// Uncomment and fill in the code below.
#define MAX_BULLET 1000
#define MAX_ENEBULLET 10
#define MAX_BOSSHP 800
MovableObject boss;
MovableObject wing;
MovableObject warning;
MovableObject plane;
MovableObject enemies[MAX_ENEMY];
MovableObject bossbullet;
MovableObject can;
MovableObject enebullet[MAX_ENEBULLET];
// [HACKATHON 2-3]
// TODO: Declare an array to store bullets with size of max bullet count.
// Uncomment and fill in the code below.
MovableObject bullets[MAX_BULLET];
// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
// 1) Declare your shooting cool-down time as constant. (0.2f will be nice)
// 2) Declare your last shoot timestamp.
// Uncomment and fill in the code below.
const float MAX_COOLDOWN =0.2;
double last_shoot_timestamp;
double tik;
double tim;
double tim2;
double touch;
double touch2;
double supatim=-10;
double drop;
double drop2;
double ready;
double throw;
double last_shoot_ene[MAX_ENEMY];
/* Declare function prototypes. */

// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h);
// [HACKATHON 3-2]
// TODO: Declare a function.
// Determines whether the point (px, py) is in rect (x, y, w, h).
// Uncomment the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h);

/* Event callbacks. */
void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);

/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Log using va_list.
void game_vlog(const char* format, va_list arg);
int lasthp=0;
int score=0;
int maxscore;
bool flag = false;
bool flag_2 = false;
bool start = false;
bool move[MAX_ENEMY];
bool no=false;
int main(int argc, char** argv) {
    // Set random seed for better random outcome.
    srand(time(NULL));
    allegro5_init();
    game_log("Allegro5 initialized");
    game_log("Game begin");
    // Initialize game variables.
    game_init();
    game_log("Game initialized");
    // Draw the first frame.
    game_draw();
    game_log("Game start event loop");
    // This call blocks until the game is finished.
    game_start_event_loop();
    game_log("Game end");
    game_destroy();
    return 0;
}

void allegro5_init(void) {
    if (!al_init())
        game_abort("failed to initialize allegro");

    // Initialize add-ons.
    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");
    if (!al_init_font_addon())
        game_abort("failed to initialize font add-on");
    if (!al_init_ttf_addon())
        game_abort("failed to initialize ttf add-on");
    if (!al_init_image_addon())
        game_abort("failed to initialize image add-on");
    if (!al_install_audio())
        game_abort("failed to initialize audio add-on");
    if (!al_init_acodec_addon())
        game_abort("failed to initialize audio codec add-on");
    if (!al_reserve_samples(RESERVE_SAMPLES))
        game_abort("failed to reserve samples");
    if (!al_install_keyboard())
        game_abort("failed to install keyboard");
    if (!al_install_mouse())
        game_abort("failed to install mouse");
	
    // TODO: Initialize other addons such as video, ...

    // Setup game display.
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");
    al_set_window_title(game_display, "I2P(I)_2020 Final Project <student_id>");

    // Setup update timer.
    game_update_timer = al_create_timer(1.0f / FPS);
    if (!game_update_timer)
        game_abort("failed to create timer");

    // Setup event queue.
    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");

    // Malloc mouse buttons state according to button counts.
    const unsigned m_buttons = al_get_mouse_num_buttons();
    game_log("There are total %u supported mouse buttons", m_buttons);
    // mouse_state[0] will not be used.
    mouse_state = malloc((m_buttons + 1) * sizeof(bool));
    memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));

    // Register display, timer, keyboard, mouse events to the event queue.
    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
    al_register_event_source(game_event_queue, al_get_keyboard_event_source());
    al_register_event_source(game_event_queue, al_get_mouse_event_source());
    // TODO: Register other event sources such as timer, video, ...

    // Start the timer to update and draw the game.
    al_start_timer(game_update_timer);
}

void game_init(void) {
    /* Shared resources*/
    font_pirulen_32 = al_load_font("pirulen.ttf", 32, 0);
    if (!font_pirulen_32)
        game_abort("failed to load font: pirulen.ttf with size 32");

    font_pirulen_24 = al_load_font("pirulen.ttf", 24, 0);
    if (!font_pirulen_24)
        game_abort("failed to load font: pirulen.ttf with size 24");

    /* Menu Scene resources*/
    main_img_background = load_bitmap_resized("main-bg.jpg", SCREEN_W, SCREEN_H);
	lose = al_load_sample("WHAT ARE YOU FUCKING GAY.mp3");
	win= al_load_sample("Oh yes daddy sound effect.mp3");
	img_warning= al_load_bitmap("warning.png");
    main_bgm = al_load_sample("S31-Night Prowler.mp3");
	boss_bgm = al_load_sample("Undertale - Megalovania.mp3");
	setting_bgm = al_load_sample("Wii Music - Gaming Background Music (HD).mp3");
    if (!main_bgm)
        game_abort("failed to load audio: S31-Night Prowler.mp3");
	meow = al_load_sample("Angry Cat Meow - Sound Effect.mp3");
	warn = al_load_sample("warn.mp3");
    // [HACKATHON 3-4]
    // TODO: Load settings images.
    // Don't forget to check their return values.
    // Uncomment and fill in the code below.
    img_settings = al_load_bitmap("settings.png");
    if (!img_settings)
        game_abort("failed to load image: settings.png");
    img_settings2 = al_load_bitmap("settings2.png");
    if (!img_settings2)
        game_abort("failed to load image: settings2.png");
	img_mute = al_load_bitmap("mute.png");
	img_vol = al_load_bitmap("vol.png");

    /* Start Scene resources*/
	boss_img_background = load_bitmap_resized("boss-bg.jpg", SCREEN_W, SCREEN_H);
    start_img_background = load_bitmap_resized("start-bg.jpg", SCREEN_W, SCREEN_H);
	lose_img_background = load_bitmap_resized("lose-bg.jpg", SCREEN_W, SCREEN_H);
	win_img_background = load_bitmap_resized("win-bg.jpg", SCREEN_W, SCREEN_H);
	setting_img_background = load_bitmap_resized("setting-bg.jpg", SCREEN_W, SCREEN_H);
	img_hp = al_load_bitmap("hp.png");
	bosshp = al_load_bitmap("bosshp.png");
	img_wing = al_load_bitmap("wing.png");

	if (!img_hp)
		game_abort("failed to load image: hp.png");
    start_img_plane = al_load_bitmap("plane.png");
    if (!start_img_plane)
        game_abort("failed to load image: plane.png");
	img_boss = al_load_bitmap("boss.png");
    start_img_enemy = al_load_bitmap("smallfighter0006.png");
    if (!start_img_enemy)
        game_abort("failed to load image: smallfighter0006.png");

    start_bgm = al_load_sample("mythica.mp3");
    if (!start_bgm)
        game_abort("failed to load audio: mythica.ogg");
    img_bullet = al_load_bitmap("image12.png");
    if (!img_bullet)
        game_abort("failed to load image: image12.png");
	img_enebullet = al_load_bitmap("enebullet.png");
	if (!img_enebullet)
		game_abort("failed to load image: enebullet.png");
	img_bossbullet = al_load_bitmap("bossbullet.png");
	img_can = al_load_bitmap("can.png");
    // Change to first scene.
    game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void) {
    bool done = false;
    ALLEGRO_EVENT event;
    int redraws = 0;
    while (!done) {
        al_wait_for_event(game_event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Event for clicking the window close button.
            game_log("Window close button clicked");
            done = true;
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            // Event for redrawing the display.
            if (event.timer.source == game_update_timer)
                // The redraw timer has ticked.
                redraws++;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Event for keyboard key down.
            game_log("Key with keycode %d down", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = true;
            on_key_down(event.keyboard.keycode);
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            // Event for keyboard key up.
            game_log("Key with keycode %d up", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            // Event for mouse key down.
            game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = true;
            on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            // Event for mouse key up.
            game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            if (event.mouse.dx != 0 || event.mouse.dy != 0) {
                // Event for mouse move.
                // game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
            } else if (event.mouse.dz != 0) {
                // Event for mouse scroll.
                game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
            }
        }
        // TODO: Process more events and call callbacks by adding more
        // entries inside Scene.

        // Redraw
        if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
            // if (redraws > 1)
            //     game_log("%d frame(s) dropped", redraws - 1);
            // Update and draw the next frame.
            game_update();
            game_draw();
            redraws = 0;
        }
    }
}
void game_update(void) {
	if (active_scene == SCENE_START) {
		double nowst = al_get_time();
		if (!flag) {
			touch2 = nowst;
			tik = nowst;
			tim = nowst;
			drop = nowst;
			flag = true;
		}
		plane.vx = plane.vy = 0;
		if ((key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])&&score<nextlv)
			plane.vy -= 2;
		if ((key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])&& score < nextlv)
			plane.vy += 2;
		if ((key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])&& score < nextlv)
			plane.vx -= 2;
		if ((key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])&& score < nextlv)
			plane.vx += 2;
		// 0.71 is (1/sqrt(2)).
		plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
		plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);
		// [HACKATHON 1-1]
		// TODO: Limit the plane's collision box inside the frame.
		//       (x, y axes can be separated.)
		// Uncomment and fill in the code below.
		if (plane.x < plane.w / 2)
			plane.x = plane.w / 2;
		else if (plane.x + plane.w / 2 > SCREEN_W)
			plane.x = SCREEN_W - plane.w / 2;
		if (plane.y < plane.h / 2)
			plane.y = plane.h / 2;
		else if (plane.y + plane.h / 2 > SCREEN_H)
			plane.y = SCREEN_H - plane.h / 2;
		// [HACKATHON 2-7]
		// TODO: Update bullet coordinates.
		// 1) For each bullets, if it's not hidden, update x, y
		// according to vx, vy.
		// 2) If the bullet is out of the screen, hide it.
		// Uncomment and fill in the code below.
		int i;
		for (i = 0; i < MAX_BULLET; i++) {
			if (bullets[i].hidden)
				continue;
			bullets[i].x += bullets[i].vx;
			bullets[i].y += bullets[i].vy;
			if (bullets[i].y < 0)
				bullets[i].hidden = true;
		}

		for (i = 0; i < MAX_ENEBULLET; i++) {
			if (enebullet[i].hidden)
				continue;
			enebullet[i].x += enebullet[i].vx;
			enebullet[i].y += enebullet[i].vy;
			if (enebullet[i].y > SCREEN_H+500)
				enebullet[i].hidden = true;
		}

		for (i = 0; i < MAX_ENEMY; i++) {			
			enemies[i].y += enemies[i].vy;
			enemies[i].x += enemies[i].vx;
			if (enemies[i].x > SCREEN_W)
				enemies[i].x = 0;
			else if (enemies[i].x < 0)
				enemies[i].x = SCREEN_W;
			else if (enemies[i].y > 0)
				enemies[i].hidden = false;
			if (enemies[i].y >= 80 || move[i]) {
				move[i] = 1;
				enemies[i].vx = (plane.x > enemies[i].x) ? 1 + (nowst - tim) / 20 : -1 - (nowst - tim) / 20;
				enemies[i].vy = (plane.y > enemies[i].y) ? 1 + (nowst - tim) / 20 : -1 - (nowst - tim) / 20;
			}
		}

		for (i = 0; i < MAX_ENEMY; i++) {
			for (int j = 0; j < MAX_BULLET; j++) {
				if (bullets[j].x + bullets[j].w / 2 >= enemies[i].x - enemies[i].w / 2 &&
					bullets[j].x - bullets[j].w / 2 <= enemies[i].x + enemies[i].w / 2 &&
					bullets[j].y + bullets[j].h / 2 >= enemies[i].y - enemies[i].h / 2 &&
					bullets[j].y - bullets[j].h / 2 <= enemies[i].y + enemies[i].h / 2 &&
					!bullets[j].hidden && !enemies[j].hidden&&enemies[i].hp) {
					enemies[i].hp--;
					bullets[j].hidden = true;
					if (!enemies[i].hp) {
						enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
						enemies[i].y = -500;
						enemies[i].vx = 0;
						enemies[i].vy = 2;
						enemies[i].hp = 2;
						move[i] = 0;
						score++;
					}
				}
			}
		}/*
		for (i = 0; i < MAX_ENEBULLET; i++) {
			for (int j = 0; j < MAX_BULLET; j++) {
				if (bullets[j].x + bullets[j].w / 2 >= enebullet[i].x - enebullet[i].w / 2 &&
					bullets[j].x - bullets[j].w / 2 <= enebullet[i].x + enebullet[i].w / 2 &&
					bullets[j].y + bullets[j].h / 2 >= enebullet[i].y - enebullet[i].h / 2 &&
					bullets[j].y - bullets[j].h / 2 <= enebullet[i].y + enebullet[i].h / 2 &&
					!bullets[j].hidden && !enebullet[i].hidden) {
					bullets[j].hidden = true;
					enebullet[i].hidden = true;
				}
			}
		}*/

			for (i = 0; i < MAX_ENEMY; i++) {
				if (plane.x + plane.w / 2 >= (enemies[i].x - enemies[i].w / 2) &&
					plane.x - plane.w / 2 <= (enemies[i].x + enemies[i].w / 2) &&
					plane.y + plane.h / 2 >= (enemies[i].y - enemies[i].h / 2) &&
					plane.y - plane.h / 2 <= (enemies[i].y + enemies[i].h / 2) &&
					!enemies[i].hidden) {
					if (nowst - touch2 > 1) {
						touch2 = nowst;
						plane.hp--;
						score -= 5;
					}
					move[i] = 0;
					enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
					enemies[i].y = -500;
					enemies[i].vx = 0;
					enemies[i].vy = 2;
					enemies[i].hp = 2;
					score++;
				}
			}

			for (i = 0; i < MAX_ENEBULLET; i++) {
				if (plane.x + plane.w / 2 >= (enebullet[i].x - enebullet[i].w / 2) &&
					plane.x - plane.w / 2 <= (enebullet[i].x + enebullet[i].w / 2) &&
					plane.y + plane.h / 2 >= (enebullet[i].y - enebullet[i].h / 2) &&
					plane.y - plane.h / 2 <= (enebullet[i].y + enebullet[i].h / 2) &&
					!enebullet[i].hidden) {
					if (nowst - touch2 > 1) {
						touch2 = nowst;
						plane.hp--;
						score -= 5;
					}
					enebullet[i].hidden = true;
				}
			}
		
		if ((score >= nextlv && score < nextlv+100)||nowst-tik>=130) {
			tik = nowst;
			score += 100;
			plane.x = 400;
			plane.y = 500;
			for (i = 0; i < MAX_ENEMY; i++) {
				enemies[i].hidden = true;
				enemies[i].vx = 0;
				enemies[i].vy = 0;
				enemies[i].x = 0;
				enemies[i].y = -1000;
			}
			for (i = 0; i < MAX_ENEBULLET; i++) {
				enebullet[i].hidden = true;
				enebullet[i].vx = 0;
				enebullet[i].vy = 0;
				enebullet[i].x = 0;
				enebullet[i].y = 0;
			}
			warning.hidden = false;
			if(!no) al_play_sample(warn, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &warn_id);
		}
		if (nowst -tik > 3 && score >= nextlv+100) {
			lasthp=plane.hp;
			warning.hidden=true;
			game_change_scene(SCENE_BOSS);
		}
		

		if (nowst - supatim < 8) {
			wing.hidden = false;
			wing.x = plane.x;
			wing.y = plane.y-40;
			touch2 = nowst;
		}
		else wing.hidden = true;

		if (plane.hp == 0) {
			plane.hp = MAX_HP;
			game_change_scene(SCENE_LOSE);
		}
		if (score > maxscore) maxscore = score;

		if ((nowst - drop)*((float)rand() / RAND_MAX) > 25) {
			if (can.hidden) {
				drop = nowst;
				can.hidden = false;
				can.x = (float)rand() / RAND_MAX * (SCREEN_W / 4) + SCREEN_W / 4;
				can.y = -100;
			}
		}
		can.x += sin(nowst);
		can.y += can.vy;
		if (can.y > 700) can.hidden = true;
		if (plane.x >= (can.x - can.w / 2) &&
			plane.x <= (can.x + can.w / 2) &&
			plane.y >= (can.y - can.h / 2) &&
			plane.y <= (can.y + can.h / 2) &&
			!can.hidden) {
			can.hidden = true;
			supatim = nowst;
		}

		if (key_state[ALLEGRO_KEY_SPACE] && (nowst - last_shoot_timestamp >= MAX_COOLDOWN||nowst-supatim<8) && score < nextlv) {
			for (i = 0; i < MAX_BULLET; i++) {
				if (bullets[i].hidden) {
					last_shoot_timestamp = nowst;
					bullets[i].hidden = false;
					bullets[i].x = plane.x;
					bullets[i].y = plane.y - plane.h / 2;
					break;
				}
			}
		}

		for (i = 0; i < MAX_ENEMY; i++) {
			if (enemies[i].y >= 80 && nowst - last_shoot_ene[i] >= 0.1 && enemies[i].hidden == false) {
				for (int j = 0; j < MAX_ENEBULLET; j++) {
					if (enebullet[j].hidden) {
						if (!no) al_play_sample(meow, 1, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, &meow_id);
						last_shoot_ene[i] = nowst;
						enebullet[j].hidden = false;
						enebullet[j].x = enemies[i].x;
						enebullet[j].y = enemies[i].y + enemies[i].h / 3;
						break;
					}
				}
			}
		}
	}


	else if (active_scene == SCENE_BOSS) {
		double nowbo = al_get_time();
		if (!flag_2) {
			touch = nowbo;
			ready = nowbo;
			drop2 = nowbo;
			throw = nowbo;
			tim2 = nowbo;
			flag_2 = true;
		}
		plane.vx = plane.vy = 0;
		if ((key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])&&!boss.hidden)
			plane.vy -= 2;
		if ((key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S]) && !boss.hidden)
			plane.vy += 2;
		if ((key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A ])&& !boss.hidden)
			plane.vx -= 2;
		if ((key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D]) && !boss.hidden)
			plane.vx += 2;
		plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
		plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);
		if (plane.x < plane.w / 2)
			plane.x = plane.w / 2;
		else if (plane.x + plane.w / 2 > SCREEN_W)
			plane.x = SCREEN_W - plane.w / 2;
		if (plane.y < plane.h / 2)
			plane.y = plane.h / 2;
		else if (plane.y + plane.h / 2 > SCREEN_H)
			plane.y = SCREEN_H - plane.h / 2;
		int i;
		for (i = 0; i < MAX_BULLET; i++) {
			if (bullets[i].hidden)
				continue;
			bullets[i].x += bullets[i].vx;
			bullets[i].y += bullets[i].vy;
			if (bullets[i].y < 0)
				bullets[i].hidden = true;
		}		

		if (plane.hp == 0) {
			plane.hp = MAX_HP;
			game_change_scene(SCENE_LOSE);
		}
		if (score > maxscore) maxscore = score;
        if (key_state[ALLEGRO_KEY_SPACE] &&( nowbo - last_shoot_timestamp >= MAX_COOLDOWN ||nowbo-supatim<8)&& !boss.hidden) {
			for (i = 0; i < MAX_BULLET;i++) {
                if (bullets[i].hidden) {
					last_shoot_timestamp = nowbo;
                    bullets[i].hidden = false;
                    bullets[i].x = plane.x;
                    bullets[i].y = plane.y-plane.h/2;
                    break;
                }
            }
        }

		if (nowbo - ready > 3&&!start) {
			boss.hidden = false;
			warning.hidden = true;
			boss.vx = 1;
			boss.vy = 0;
			boss.x = 400;
			start = true;
		}
		if (boss.x < 250) {
			boss.x = 250;
			boss.vx = (2 + ((nowbo - tim2) / 10));
		}
		else if (boss.x > 550) {
			boss.x = 550;
			boss.vx = -(2 + ((nowbo - tim2) / 10));
		}
		if (boss.y < boss.h / 2)
			boss.y = boss.h / 2;
		else if (boss.y + boss.h / 2 > SCREEN_H)
			boss.y = SCREEN_H - boss.h / 2;
		
			if (plane.x + plane.w / 2 >= boss.x - boss.w / 2 &&
				plane.x - plane.w / 2 <= boss.x + boss.w / 2 &&
				plane.y + plane.h / 2 >= boss.y - boss.h / 2 &&
				plane.y - plane.h / 2 <= boss.y + boss.h / 2) {
				if (nowbo - touch > 1) {
					touch = nowbo;
					plane.hp--;
					score -= 100;
				}
			}

		boss.x += boss.vx;

		if ((nowbo - throw)*((float)rand()/RAND_MAX) > 3) {
			if (bossbullet.hidden) {
				if (!no) al_play_sample(meow, 1, 0.0, 3.0, ALLEGRO_PLAYMODE_ONCE, &meow_id);
				throw = nowbo;
				bossbullet.hidden = false;
				bossbullet.x = boss.x;
				bossbullet.y = boss.y + boss.h / 3;
			}
		}


		if ((nowbo - drop2)*((float)rand() / RAND_MAX) > 20) {
			if (can.hidden) {
				drop2 = nowbo;
				can.hidden = false;
				can.x = (float)rand() / RAND_MAX * (SCREEN_W / 4) + SCREEN_W / 4;
				can.y = -100;
			}
		}
		can.x += sin(nowbo);
		can.y += can.vy;
		if (can.y > 700) can.hidden = true;

		if (plane.x >= (can.x - can.w / 2) &&
			plane.x <= (can.x + can.w / 2) &&
			plane.y >= (can.y - can.h / 2) &&
			plane.y <= (can.y + can.h / 2) &&
			!can.hidden) {
			can.hidden = true;
			supatim = nowbo;
		}

		bossbullet.vy = 4+ (nowbo - tim2) / 100;
		bossbullet.y += bossbullet.vy;

		if (bossbullet.y > 1200) bossbullet.hidden = true;
		
		if (plane.x - plane.w / 2 >= (bossbullet.x - bossbullet.w / 2) &&
			plane.x + plane.w / 2 <= (bossbullet.x + bossbullet.w / 2) &&
			plane.y - plane.h / 2 >= (bossbullet.y - bossbullet.h / 2) &&
			plane.y + plane.h / 2 <= (bossbullet.y + bossbullet.h / 2)) {
			if (nowbo - touch > 1) {
				touch = nowbo;
				plane.hp--;
				score -= 100;
			}
		}

		for (int j = 0; j < MAX_BULLET; j++) {
			if (bullets[j].x - bullets[j].w / 2 >= boss.x - boss.w / 2 &&
				bullets[j].x + bullets[j].w / 2 <= boss.x + boss.w / 2 &&
				bullets[j].y - bullets[j].h / 2 >= boss.y - boss.h / 2 &&
				bullets[j].y + bullets[j].h / 2 <= boss.y + boss.h / 2 &&
				!bullets[j].hidden) {
				boss.hp--;
				bullets[j].hidden = true;
				score++;
			}
		}

		if (boss.hp<=0) {
			boss.hp = MAX_BOSSHP;
			score += 1000;
			game_change_scene(SCENE_WIN);
		}
		if (nowbo - supatim < 8) {
			wing.hidden = false;
			wing.x = plane.x;
			wing.y = plane.y-40;
			touch=nowbo;
		}
		else wing.hidden = true;

		if (score > maxscore) maxscore = score;
    }
}

void game_draw(void) {
    if (active_scene == SCENE_MENU) {
        al_draw_bitmap(main_img_background, 0, 0, 0);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "Space Shooter");
        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, "Press enter key to start");

        // [HACKATHON 3-5]
        // TODO: Draw settings images.
        // The settings icon should be located at (x, y, w, h) =
        // (SCREEN_W - 48, 10, 38, 38).
        // Change its image according to your mouse position.
        // Uncomment and fill in the code below.
        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W-48, 10, 38, 38))
            al_draw_bitmap(img_settings2, SCREEN_W-48, 10, 0);
        else
            al_draw_bitmap(img_settings, SCREEN_W-48, 10, 0);
    } else if (active_scene == SCENE_START) {
        int i;
        al_draw_bitmap(start_img_background, 0, 0, 0);
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 48, 10, 38, 38))
			al_draw_bitmap(img_settings2, SCREEN_W - 48, 10, 0);
		else
			al_draw_bitmap(img_settings, SCREEN_W - 48, 10, 0);
		al_draw_textf(font_pirulen_32, al_map_rgb(225, 225, 225), 0, 20, 0, "SCORE:%d",score);
		for (i = 0; i < MAX_ENEBULLET; i++)
			draw_movable_object(enebullet[i]);
        for (i=0;i<MAX_BULLET;i++)
			draw_movable_object(bullets[i]);
        for (i = 0; i < MAX_ENEMY; i++)
            draw_movable_object(enemies[i]);
		draw_movable_object(warning);
		draw_movable_object(plane);
		draw_movable_object(can);
		draw_movable_object(wing);
		img_hp = load_bitmap_resized("hp.png", (400 / MAX_HP)*plane.hp, 15);
		al_draw_bitmap(img_hp, 200, SCREEN_H-15, 0);
    }

	else if (active_scene == SCENE_BOSS) {
		al_draw_bitmap(boss_img_background, 0, 0, 0);
		
		al_draw_textf(font_pirulen_32, al_map_rgb(225, 225, 225), 0, 20, 0, "SCORE:%d", score);
		draw_movable_object(warning);
		draw_movable_object(boss);
		draw_movable_object(bossbullet);
		draw_movable_object(plane);
		draw_movable_object(can);
		draw_movable_object(wing);
		int i;
		for (i = 0; i < MAX_BULLET; i++)
			draw_movable_object(bullets[i]);

		bosshp = load_bitmap_resized("bosshp.png", (SCREEN_W / MAX_BOSSHP)*boss.hp, 26);
		al_draw_bitmap(bosshp, 0, 0, 0);
		img_hp = load_bitmap_resized("hp.png", (400 / MAX_HP)*plane.hp, 15);
		al_draw_bitmap(img_hp, 200, SCREEN_H - 15, 0);
	}
    // [HACKATHON 3-9]
    // TODO: If active_scene is SCENE_SETTINGS.
    // Draw anything you want, or simply clear the display.
    else if (active_scene == SCENE_SETTINGS) {
		al_draw_bitmap(setting_img_background, 0, 0, 0);
		if (no)
			al_draw_bitmap(img_mute, SCREEN_W - 48, 10, 0);
		else
			al_draw_bitmap(img_vol, SCREEN_W - 48, 10, 0);
		al_draw_textf(font_pirulen_24, al_map_rgb(225, 0, 0), 350, 550, 0, "back");
    }
	else if (active_scene == SCENE_LOSE) {
		al_draw_bitmap(lose_img_background, 0, 0, 0);
		al_draw_textf(font_pirulen_32, al_map_rgb(0, 0, 0), SCREEN_W / 2.5+30, SCREEN_H / 4 , 0, "YOURSCORE:%d", score);
		al_draw_textf(font_pirulen_32, al_map_rgb(0, 0, 0), SCREEN_W / 2.5+30, SCREEN_H / 4 - 100, 0, "HIGHESTSCORE:%d", maxscore);
		al_draw_text(font_pirulen_24, al_map_rgb(255, 0, 100), SCREEN_W / 2 - 200, SCREEN_H -200, 0, "Press ESC to leave");
		al_draw_text(font_pirulen_24, al_map_rgb(255, 0, 100), SCREEN_W / 2 - 250, SCREEN_H -250, 0, "Press Enter to restart");
	}
	else if (active_scene == SCENE_WIN) {
		al_draw_bitmap(win_img_background, 0, 0, 0);
		al_draw_textf(font_pirulen_32, al_map_rgb(0, 0, 0), SCREEN_W / 2.5+30, SCREEN_H / 4, 0, "YOURSCORE:%d", score);
		al_draw_textf(font_pirulen_32, al_map_rgb(0, 0, 0), SCREEN_W / 2.5+30, SCREEN_H / 4 - 100, 0, "HIGHESTSCORE:%d", maxscore);
		al_draw_text(font_pirulen_24, al_map_rgb(255, 0, 100), SCREEN_W / 2 - 200, SCREEN_H - 200, 0, "Press ESC to leave");
		al_draw_text(font_pirulen_24, al_map_rgb(255, 0, 100), SCREEN_W / 2 - 250, SCREEN_H - 250, 0, "Press Enter to restart");
	}
    al_flip_display();
}

void game_destroy(void) {
    // Destroy everything you have created.
    // Free the memories allocated by malloc or allegro functions.
    // Destroy shared resources.
    al_destroy_font(font_pirulen_32);
    al_destroy_font(font_pirulen_24);

    /* Menu Scene resources*/
	al_destroy_bitmap(boss_img_background);
    al_destroy_bitmap(main_img_background);
	al_destroy_bitmap(img_warning);
	al_destroy_bitmap(img_enebullet);
	al_destroy_bitmap(img_bossbullet);
	al_destroy_bitmap(img_can);
	al_destroy_bitmap(img_wing);
	al_destroy_sample(boss_bgm);
	al_destroy_sample(main_bgm);
	al_destroy_sample(setting_bgm);
	al_destroy_sample(warn);
    al_destroy_sample(start_bgm);
	al_destroy_sample(lose);
	al_destroy_sample(win);
	al_destroy_sample(meow);
    // [HACKATHON 3-6]
    // TODO: Destroy the 2 settings images.
    // Uncomment and fill in the code below.
	al_destroy_bitmap(img_settings);
	al_destroy_bitmap(img_settings2);
	al_destroy_bitmap(img_vol);
	al_destroy_bitmap(img_mute);

    /* Start Scene resources*/
    al_destroy_bitmap(start_img_background);
	al_destroy_bitmap(win_img_background);
	al_destroy_bitmap(lose_img_background);
	al_destroy_bitmap(setting_img_background);
    al_destroy_bitmap(start_img_plane);
    al_destroy_bitmap(start_img_enemy);
	al_destroy_bitmap(img_boss);
	al_destroy_bitmap(img_hp);
	al_destroy_bitmap(bosshp);
    // [HACKATHON 2-10]
    // TODO: Destroy your bullet image.
    // Uncomment and fill in the code below.
	al_destroy_bitmap(img_bullet);
    al_destroy_timer(game_update_timer);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);
    free(mouse_state);
}

void game_change_scene(int next_scene) {
    game_log("Change scene from %d to %d", active_scene, next_scene);
    // TODO: Destroy resources initialized when creating scene.
    if (active_scene == SCENE_MENU) {
        al_stop_sample(&main_bgm_id);
        game_log("stop audio (bgm)");
    }
	else if (active_scene == SCENE_START) {
		al_stop_sample(&start_bgm_id);
		al_stop_sample(&warn_id);
		game_log("stop audio (bgm)");
	}
	else if (active_scene == SCENE_BOSS) {
		al_stop_sample(&boss_bgm_id);
		game_log("stop audio (bgm)");
	}
	else if (active_scene == SCENE_SETTINGS) {
		al_stop_sample(&setting_bgm_id);
		game_log("stop audio (bgm)");
	}
    active_scene = next_scene;
    // TODO: Allocate resources before entering scene.
    if (active_scene == SCENE_MENU) {
		if (!no) if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
            game_abort("failed to play audio (bgm)");
    } 
	else if (active_scene == SCENE_SETTINGS) {
		if (!no) if (!al_play_sample(setting_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &setting_bgm_id))
			game_abort("failed to play audio (bgm)");
	}
	else if (active_scene == SCENE_LOSE) {
		if (!no) if (!al_play_sample(lose, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &lose_id))
			game_abort("failed to play audio (bgm)");
	}
	else if (active_scene == SCENE_WIN) {
		if (!no) if (!al_play_sample(win, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &win_id))
			game_abort("failed to play audio (bgm)");
	}
	else if (active_scene == SCENE_START) {
        int i;
        plane.img = start_img_plane;
        plane.x = 400;
        plane.y = 500;
        plane.w = al_get_bitmap_width(plane.img);
        plane.h = al_get_bitmap_height(plane.img);
		plane.hp = MAX_HP;
        for (i = 0; i < MAX_ENEMY; i++) {
            enemies[i].img = start_img_enemy;
            enemies[i].w = al_get_bitmap_width(start_img_enemy);
            enemies[i].h = al_get_bitmap_height(start_img_enemy);
            enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
            enemies[i].y = (float)rand() / RAND_MAX * (SCREEN_H - enemies[i].h) - 800;;
			enemies[i].vx = 0;
			enemies[i].vy = 2;
			enemies[i].hidden = false;
			enemies[i].hp = 2;
        }

		for (i = 0; i < MAX_BULLET;i++) {
            bullets[i].w = al_get_bitmap_width(img_bullet);
            bullets[i].h = al_get_bitmap_height(img_bullet);
            bullets[i].img = img_bullet;
            bullets[i].vx = 0;
            bullets[i].vy = -5;
            bullets[i].hidden = true;
        }
		for (i = 0; i < MAX_ENEBULLET; i++) {
			enebullet[i].img = img_enebullet;
			enebullet[i].w = al_get_bitmap_width(enebullet[i].img);
			enebullet[i].h = al_get_bitmap_height(enebullet[i].img);
			enebullet[i].vx = 0;
			enebullet[i].vy = 8;
			enebullet[i].hidden = true;
		}

		wing.img = img_wing;
		wing.w = al_get_bitmap_width(wing.img);
		wing.h = al_get_bitmap_height(wing.img);
		wing.vx = 0;
		wing.vy = 0;
		wing.hidden = true;


		can.img = img_can;
		can.w = al_get_bitmap_width(can.img);
		can.h = al_get_bitmap_height(can.img);
		can.vx = 0;
		can.vy = 2;
		can.hidden = true;

		warning.img = img_warning;
		warning.w = al_get_bitmap_width(img_warning);
		warning.h = al_get_bitmap_height(img_warning);
		warning.x = SCREEN_W / 2;
		warning.y = SCREEN_H / 2-30;
		warning.hidden = true;
		if (!no) if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
            game_abort("failed to play audio (bgm)");
    }
	else if (active_scene == SCENE_BOSS) {
		int i;
		plane.img = start_img_plane;
		plane.x = 400;
		plane.y = 500;
		plane.w = al_get_bitmap_width(plane.img);
		plane.h = al_get_bitmap_height(plane.img);
		plane.hp = lasthp;

		
		for (i = 0; i < MAX_BULLET; i++) {
			bullets[i].img = img_bullet;
			bullets[i].w = al_get_bitmap_width(bullets[i].img);
			bullets[i].h = al_get_bitmap_height(bullets[i].img);
			bullets[i].vx = 0;
			bullets[i].vy = -5;
			bullets[i].hidden = true;
		}

		bossbullet.img = img_bossbullet;
		bossbullet.w = al_get_bitmap_width(bossbullet.img);
		bossbullet.h = al_get_bitmap_height(bossbullet.img);
		bossbullet.vx = 0;
		bossbullet.hidden = true;

		wing.img = img_wing;
		wing.w = al_get_bitmap_width(wing.img);
		wing.h = al_get_bitmap_height(wing.img);
		wing.vx = 0;
		wing.vy = 0;
		wing.hidden = true;

		can.img = img_can;
		can.w = al_get_bitmap_width(can.img);
		can.h = al_get_bitmap_height(can.img);
		can.vx = 0;
		can.vy = 2;
		can.hidden = true;

		boss.img = img_boss;
		boss.x = 400;
		boss.y = 50;
		boss.vx = 0;
		boss.vy = 0;
		boss.w = al_get_bitmap_width(img_boss);
		boss.h = al_get_bitmap_height(img_boss);
		boss.hp = MAX_BOSSHP;
		boss.hidden = true;

		warning.img = img_warning;
		warning.w = al_get_bitmap_width(img_warning);
		warning.h = al_get_bitmap_height(img_warning);
		warning.x = SCREEN_W / 2;
		warning.y = SCREEN_H / 2-30;
		warning.hidden = false;

		if(!no) if (!al_play_sample(boss_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &boss_bgm_id))
			game_abort("failed to play audio (bgm)");
	}
}

void on_key_down(int keycode) {
    if (active_scene == SCENE_MENU) {
		if (keycode == ALLEGRO_KEY_ENTER) {
			score = 0;
			flag = false;
			flag_2 = false;
			start = false;
			game_change_scene(SCENE_START);
		}
		if (keycode == ALLEGRO_KEY_ESCAPE)
			game_abort("Loser");
		if (keycode == ALLEGRO_KEY_BACKSPACE)
			game_change_scene(SCENE_SETTINGS);
    }
    // [HACKATHON 3-10]
    // TODO: If active_scene is SCENE_SETTINGS and Backspace is pressed,
    // return to SCENE_MENU.
    else if (active_scene == SCENE_SETTINGS) {
        if (keycode == ALLEGRO_KEY_BACKSPACE)
            game_change_scene(SCENE_MENU);
		if (keycode == ALLEGRO_KEY_ESCAPE)
			game_abort("Loser");
    }
	else if (active_scene == SCENE_START) {
		if (keycode == ALLEGRO_KEY_ESCAPE)
			game_abort("Loser");
		if (keycode == ALLEGRO_KEY_BACKSPACE)
			game_change_scene(SCENE_SETTINGS);
	}
	else if (active_scene == SCENE_BOSS) {
		if (keycode == ALLEGRO_KEY_ESCAPE)
			game_abort("Loser");
		if (keycode == ALLEGRO_KEY_BACKSPACE)
			game_change_scene(SCENE_SETTINGS);
	}
	else if (active_scene == SCENE_LOSE) {
		if (keycode == ALLEGRO_KEY_ESCAPE)
			game_abort("Loser");
		if (keycode == ALLEGRO_KEY_ENTER) {
			game_change_scene(SCENE_MENU);
		}
	}
	else if (active_scene == SCENE_WIN) {
		if (keycode == ALLEGRO_KEY_ESCAPE)
			game_abort("Loser");
		if (keycode == ALLEGRO_KEY_ENTER) {
			game_change_scene(SCENE_MENU);
		}
	}	
}

void on_mouse_down(int btn, int x, int y) {    
    if (active_scene == SCENE_MENU) {
        if (btn == 1) {
            if (pnt_in_rect(x, y, SCREEN_W-48, 10, 38, 38))
                game_change_scene(SCENE_SETTINGS);
			if (pnt_in_rect(x, y, 20, SCREEN_H - 100, 600,100))
				game_change_scene(SCENE_START);
        }
    }
	else if (active_scene == SCENE_SETTINGS) {
		if (btn == 1) {
			if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38)) {
				if (!no) no = true;
				else no = false;
				game_change_scene(SCENE_SETTINGS);
			}
			if (pnt_in_rect(x, y, 350, 550, 100, 50)) {
				game_change_scene(SCENE_MENU);
			}
		}
	}
	else if (active_scene == SCENE_START) {
		if (btn == 1) {
			if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38))
				game_change_scene(SCENE_SETTINGS);
		}
	}
	else if (active_scene == SCENE_BOSS) {
		if (btn == 1) {
			if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38))
				game_change_scene(SCENE_SETTINGS);
		}
	}
}

void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
}

ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h) {
    ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
    if (!loaded_bmp)
        game_abort("failed to load image: %s", filename);
    ALLEGRO_BITMAP *resized_bmp = al_create_bitmap(w, h);
    ALLEGRO_BITMAP *prev_target = al_get_target_bitmap();

    if (!resized_bmp)
        game_abort("failed to create bitmap when creating resized image: %s", filename);
    al_set_target_bitmap(resized_bmp);
    al_draw_scaled_bitmap(loaded_bmp, 0, 0,
        al_get_bitmap_width(loaded_bmp),
        al_get_bitmap_height(loaded_bmp),
        0, 0, w, h, 0);
    al_set_target_bitmap(prev_target);
    al_destroy_bitmap(loaded_bmp);

    game_log("resized image: %s", filename);

    return resized_bmp;
}

// [HACKATHON 3-3]
// TODO: Define bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
// Uncomment and fill in the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h) {
	if (px<=x+w&&px>=x&&py<=y+h&&py>=y)	return true;
	else return false;
}


// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occured, exiting after 2 secs");
    // Wait 2 secs before exiting.
    al_rest(0.5);
    // Force exit program.
    exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
    static bool clear_file = true;
    vprintf(format, arg);
    printf("\n");
    // Write log to file for later debugging.
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile) {
        vfprintf(pFile, format, arg);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    clear_file = false;
#endif
}
