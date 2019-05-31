#include "Game.h"

#include <string>

#include "globals.h"
#include "utility/tools.h"

Game::Game() {
  // Other Sprites
  buffer = create_bitmap (SCREEN_W, SCREEN_H);

  // Create screens
  if (single_player) {
    screen1 = create_bitmap (SCREEN_W, SCREEN_H);
    screen2 = create_bitmap (0, 0);
  }
  else {
    screen1 = create_bitmap (SCREEN_W, SCREEN_H / 2);
    screen2 = create_bitmap (SCREEN_W, SCREEN_H / 2);
  }

  lightingEnabled = (levelOn == 3);

  // Player
  player1.load_images (1);
  player1.load_sounds();
  player1.set_keys (KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER_PAD, 0);

  player2.load_images (2);
  player2.load_sounds();
  player2.set_keys (KEY_W, KEY_S, KEY_A, KEY_D, KEY_SPACE, 1);

  // Build a color lookup table for lighting effects
  get_palette (pal);
  create_light_table (&light_table, pal, 0, 0, 0, NULL);

  //Sets Font
  cooper = load_font_ex ("fonts/cooper_black.pcx");

  // Load images
  countdownImage = load_png_ex ("images/321go.png");
  darkness = load_png_ex ("images/darkness.png");
  darkness_old = load_png_ex ("images/darkness.png");

  spotlight = load_png_ex ("images/spotlight.png");
  spaceImage[0] = load_png_ex ("images/space0.png");
  spaceImage[1] = load_png_ex ("images/space1.png");
  spaceImage[2] = load_png_ex ("images/abutton0.png");
  spaceImage[3] = load_png_ex ("images/abutton1.png");

  results = load_png_ex ("images/gui/winscreen.png");
  results_singleplayer = load_png_ex ("images/gui/winscreen_singleplayer.png");

  // Samples
  countdown = load_sample_ex ("sounds/countdown.wav");
  timeout = load_sample_ex ("sounds/timeout.wav");

  // Load music
  waitingMusic = load_ogg_ex("sounds/music/BasicJim.ogg");
  mainMusic = load_ogg_ex("sounds/music/BasicJimFull.ogg");

  // Init
  init();
}

void Game::init() {
  // Create map
  delete tile_map;

  tile_map = new TileMap ();

  std::string file_name = "data/level_" + std::to_string(levelOn + 1);
  if (!tile_map -> load (file_name.c_str()))
    abort_on_error(("Could not open level" + file_name).c_str());


  cam_1 = Camera(screen1 -> w, screen1 -> h, tile_map -> getWidth(), tile_map -> getHeight());
  cam_2 = Camera(screen2 -> w, screen2 -> h, tile_map -> getWidth(), tile_map -> getHeight());

  tm_p1.Stop();
  tm_p1.Reset();
  tm_p2.Stop();
  tm_p2.Reset();

  // Find spawn
  tile *spawnTile = tile_map -> find_tile_type(199, 1);
  if (spawnTile != nullptr) {
    player1.set_spawn (spawnTile -> getX(), spawnTile -> getY());
    player2.set_spawn (spawnTile -> getX(), spawnTile -> getY());
  }

  // Play music
  play_sample (countdown, 255, 125, 1000, 0);
  play_sample (mainMusic, 255, 125, 1000, 1);

  // Start game
  tm_begin.Start();
}

void Game::update() {
  // Camera follow
  cam_1.Follow(player1.getX(), player1.getY());
  cam_2.Follow(player2.getX(), player2.getY());

  // Starting countdown
  if (!tm_begin.IsRunning()) {
    poll_joystick();

    // Stop from moving once done
    if (!player1.getFinished()) {
      player1.update (tile_map);
    }
    if (!player2.getFinished() && !single_player) {
      player2.update (tile_map);
    }
  }

  // Stop timers
  if (player1.getFinished() && tm_p1.IsRunning())
    tm_p1.Stop();
  if (player2.getFinished() && tm_p2.IsRunning())
    tm_p2.Stop();

  // Change level when both are done
  if (key[KEY_ENTER] && player1.getFinished() && (player2.getFinished() || single_player)) {
    set_next_state (STATE_MENU);
  }

  // Back to menu
  if (key[KEY_M])
    set_next_state (STATE_MENU);
}

void Game::draw() {
  // Black background (just in case)
  rectfill (buffer, 0, 0, SCREEN_W, SCREEN_H, 0x000000);

  // Draw tiles and characters
  tile_map -> draw (screen1, cam_1.GetX(), cam_1.GetY());
  player1.draw (screen1, cam_1.GetX(), cam_1.GetY());

  if (!single_player) {
    tile_map -> draw (screen2, cam_2.GetX(), cam_2.GetY());
    player2.draw (screen1, cam_1.GetX(), cam_1.GetY());
    player1.draw (screen2, cam_1.GetX(), cam_1.GetY());
    player2.draw (screen2, cam_2.GetX(), cam_2.GetY());
  }

  // Lighting
  if (lightingEnabled) {
    /*set_alpha_blender();
    draw_sprite (darkness, darkness_old, 0, 0);

    // Player 1
    for (auto &t: tile_map -> mapTiles) {
      if ((t.getX() >= tile_map -> x - t.getWidth()) && (t.getX() < tile_map -> x + 1280) &&
          (t.getY() >= tile_map -> y - t.getHeight()) && (t.getY() < tile_map -> y + 960)) {
        if (t.containsAttribute (light)) {
          stretch_sprite (darkness, spotlight, t.getX() - tile_map -> x + 32 - t.getWidth() * 3,
                          t.getY() - tile_map -> y + 32 - (t.getHeight() * 3), t.getWidth() * 6,
                          t.getHeight() * 6);
        }
      }
    }
    draw_sprite (darkness, spotlight, player1.getX() - cam_1.GetX() + 32 - (spotlight->w / 2), player1.getY() - cam_2.GetY() + 32 - (spotlight->h / 2));
    draw_trans_sprite (screen1, darkness, 0, 0);
    draw_sprite (darkness, spotlight, player2.getX() - cam_2.GetX() + 32 - (spotlight->w / 2), player2.getY() - cam_2.GetY() + 32 - (spotlight->h / 2));
    draw_trans_sprite (screen2, darkness, 0, 0);*/
  }

  // Draw split screens
  // Screens
  if (single_player) {
    stretch_sprite (buffer, screen1, 0, 0, SCREEN_W, SCREEN_H);
  }
  else {
    stretch_sprite (buffer, screen1, 0, 0, SCREEN_W, SCREEN_H / 2);
    stretch_sprite (buffer, screen2, 0, SCREEN_H / 2, SCREEN_W, SCREEN_H / 2);
    rectfill (buffer, 0, (SCREEN_H / 2) - 8,  SCREEN_W, (SCREEN_H / 2) + 8, makecol (0, 0, 0));
  }

  // Frame
  rectfill (buffer, 0, 0, SCREEN_W, 16, makecol (0, 0, 0));
  rectfill (buffer, 0, 0, 16, SCREEN_H, makecol (0, 0, 0));
  rectfill (buffer, SCREEN_W - 16, 0, SCREEN_W, SCREEN_H, makecol (0, 0, 0));
  rectfill (buffer, 0, SCREEN_H - 16, SCREEN_W, SCREEN_H, makecol (0, 0, 0));

  // Only draw timer once game has begun
  if (!tm_begin.IsRunning()) {
    // Timers
    rectfill (buffer, 20, 20, 320, 90, makecol (0, 0, 0));

    if (!single_player) {
      rectfill (buffer, 20, (SCREEN_H / 2) + 20, 320, (SCREEN_H / 2) + 90, makecol (0, 0, 0));
    }

    // Draw timer to screen
    textprintf_ex (buffer, cooper, 40, 55, makecol (255, 255, 255), -1, "Time: %.1f", tm_p1.GetElapsedTime<milliseconds>() / 1000);
    textprintf_ex (buffer, cooper, 40, 20, makecol (255, 255, 255), -1, "Deaths:%i", player1.getDeathcount());

    if (!single_player) {
      textprintf_ex (buffer, cooper, 40, (SCREEN_H / 2) + 20 + 35, makecol (255, 255, 255), -1, "Time: %.1f", tm_p2.GetElapsedTime<milliseconds>() / 1000);
      textprintf_ex (buffer, cooper, 40, (SCREEN_H / 2) + 20, makecol (255, 255, 255), -1, "Deaths:%i", player2.getDeathcount());
    }
  }
  // Starting countdown
  else {
    // Timer 3..2..1..GO!
    if (tm_begin.GetElapsedTime<milliseconds>() < 330) {
      masked_stretch_blit (countdownImage, buffer, 0, 0, 14, 18, SCREEN_W / 2 - 100, SCREEN_H / 2 - 100, 140, 180);
    }
    else if (tm_begin.GetElapsedTime<milliseconds>() < 660) {
      masked_stretch_blit (countdownImage, buffer, 19, 0, 14, 18, SCREEN_W / 2 - 100, SCREEN_H / 2 - 100, 140, 180);
    }
    else if (tm_begin.GetElapsedTime<milliseconds>() < 990) {
      masked_stretch_blit (countdownImage, buffer, 39, 0, 14, 18, SCREEN_W / 2 - 100, SCREEN_H / 2 - 100, 140, 180);
    }
    else if (tm_begin.GetElapsedTime<milliseconds>() < 1200) {
      masked_stretch_blit (countdownImage, buffer, 57, 0, 40, 18, SCREEN_W / 2 - 200, SCREEN_H / 2 - 100, 400, 180);
    }
    else {
      tm_begin.Stop();
      tm_p1.Start();
      tm_p2.Start();
    }
  }

  // Change level when both are done
  if ((player1.getFinished() && player2.getFinished() && !single_player) || (player1.getFinished() && single_player)) {
    const float p1_time = tm_p1.GetElapsedTime<milliseconds>() / 1000;
    const float p2_time = tm_p1.GetElapsedTime<milliseconds>() / 1000;

    set_alpha_blender();

    if (single_player) {
      draw_trans_sprite (buffer, results_singleplayer, (SCREEN_W / 2) - 364, (SCREEN_H / 2) - 200);
    }
    else {
      draw_trans_sprite (buffer, results, (SCREEN_W / 2) - 364, (SCREEN_H / 2) - 200);
    }

    textprintf_ex (buffer, cooper, (SCREEN_W / 2) - 60, (SCREEN_H / 2) - 110, makecol (255, 255, 255), -1, "%.1f", p1_time);

    if (!single_player) {
      textprintf_ex (buffer, cooper, (SCREEN_W / 2) - 60, (SCREEN_H / 2) - 55, makecol (255, 255, 255), -1, "%.1f", p2_time);

      if (p1_time < p2_time) {
        textprintf_ex (buffer, cooper, (SCREEN_W / 2) - 175, (SCREEN_H / 2) + 2, makecol (255, 255, 255), -1, "1");
        textprintf_ex (buffer, cooper, (SCREEN_W / 2) - 5, (SCREEN_H / 2) + 2, makecol (255, 255, 255), -1, "%.1f", p2_time - p1_time);
      }
      else if (p1_time > p2_time) {
        textprintf_ex (buffer, cooper, (SCREEN_W / 2) - 175, (SCREEN_H / 2) + 2, makecol (255, 255, 255), -1, "2");
        textprintf_ex (buffer, cooper, (SCREEN_W / 2) - 5, (SCREEN_H / 2) + 2, makecol (255, 255, 255), -1, "%.1f", p1_time - p2_time);
      }
    }
  }

  // Draw buffer
  stretch_sprite (screen, buffer, 0, 0, SCREEN_W, SCREEN_H);
}

Game::~Game() {
  // Destroy images
  destroy_bitmap (buffer);
  destroy_bitmap (screen1);
  destroy_bitmap (screen2);
  destroy_bitmap (countdownImage);
  destroy_bitmap (results);
  destroy_bitmap (results_singleplayer);

  for (int i = 0; i < 4; i++)
    destroy_bitmap (spaceImage[0]);

  // Destroy fonts
  destroy_font (cooper);

  // Destroy sounds
  destroy_sample (countdown);
  destroy_sample (timeout);
  destroy_sample (waitingMusic);
  destroy_sample (mainMusic);

  // Fade out
  highcolor_fade_out (16);
}
