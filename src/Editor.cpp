#include "Editor.h"

#include <string>
#include "TileTypeLoader.h"

#include "globals.h"
#include "utility/tools.h"
#include "utility/MouseListener.h"
#include "utility/KeyListener.h"

Editor::Editor() {
  // Create map
  tile_map = new TileMap ();

  // Create example tile
  pallette_tile = new tile (0, 0, 0);

  font = load_font_ex ("fonts/arial_black.pcx");
  cursor = load_png_ex ("images/gui/cursor1.png");

  ib_save = InputBox(400, 408, 500, 50, "untitled");
  ib_open = InputBox(400, 408, 500, 50, "templates/blank64x48");

  btn_save = Button(540, 500);
  btn_open = Button(540, 500);
  btn_save.SetOnClick(std::bind(&Editor::Save, this));
  btn_open.SetOnClick(std::bind(&Editor::Open, this));
  btn_save.SetImages("images/gui/button_save.png", "images/gui/button_save_hover.png");
  btn_open.SetImages("images/gui/button_load.png", "images/gui/button_load_hover.png");

  set_alpha_blender();

  layer = 1;
  opening = true;
  saving = false;
}

Editor::~Editor() {
  destroy_bitmap (cursor);

  delete tile_map;
  delete pallette_tile;
}

void Editor::Save() {
  tile_map -> save ("data/" + ib_save.GetValue());
  saving = false;
}

void Editor::Open() {
  tile_map -> load ("data/" + ib_open.GetValue().substr (0, ib_open.GetValue().size() - 4));
  opening = false;
  cam = Camera(NATIVE_SCREEN_W, NATIVE_SCREEN_H, tile_map -> getWidth(), tile_map -> getHeight());
  cam.SetSpeed(1);
  cam.SetBounds(20, 20);
}

void Editor::Edit() {
  cam.Follow(MouseListener::x + cam.GetX(), MouseListener::y + cam.GetY());

  // Change selected
  if (KeyListener::keyPressed[KEY_UP]) {
    int i = pallette_tile -> getType () + 1;
    while (!TileTypeLoader::GetTile(i))
      i = (i + 1) > 400 ? 0 : i + 1;
    pallette_tile -> setType (i);
  }

  if (KeyListener::keyPressed[KEY_DOWN]) {
    int i = pallette_tile -> getType () - 1;
    while (!TileTypeLoader::GetTile(i))
      i = (i - 1) < 0 ? 400 : i - 1;
    pallette_tile -> setType (i);
  }

  // Change Layer
  if (KeyListener::keyPressed[KEY_TAB])
    layer = !layer;

  // Operations
  tile *temp_tile = tile_map -> get_tile_at(MouseListener::x + cam.GetX(), MouseListener::y + cam.GetY(), layer);
  if (temp_tile) {
    // Place tile
    if (mouse_b & 1)
      temp_tile -> setType (pallette_tile -> getType());

    // Erase tile
    if (mouse_b & 2)
      temp_tile -> setType (0);

    // Get tile type tile
    if (KeyListener::keyPressed[KEY_K])
      pallette_tile -> setType(temp_tile -> getType());
  }

  // Save map
  if (KeyListener::keyPressed[KEY_S]) {
    clear_keybuf();
    saving = true;
  }

  // Open map
  if (KeyListener::keyPressed[KEY_O]) {
    clear_keybuf();
    opening = true;
  }

  //Fill map
  if (KeyListener::keyPressed[KEY_F]) {
    for (auto &t: tile_map -> mapTilesBack) {
      t.setType (pallette_tile -> getType());
    }
  }
}

void Editor::update(StateEngine *engine) {
  // Back to menu
  if (KeyListener::keyPressed[KEY_M] && !saving && !opening) {
    setNextState (engine, StateEngine::STATE_MENU);
  }

  // Close menu
  if (KeyListener::keyPressed[KEY_TILDE]) {
    opening = false;
    saving = false;
  }

  // Run states
  if (saving) {
    ib_save.Focus();
    ib_save.Update();
    btn_save.Update();
  }
  else if (opening) {
    ib_open.Focus();
    ib_open.Update();
    btn_open.Update();
  }
  else {
    Edit();
  }
}

void Editor::draw(BITMAP *buffer) {
  // Background
  rectfill (buffer, 0, 0, NATIVE_SCREEN_W, NATIVE_SCREEN_H, makecol (255, 255, 255));

  // Draw tiles
  tile_map -> draw (buffer, cam.GetX(), cam.GetY());
  pallette_tile -> draw_tile (buffer, 0, 0, 0);
  textprintf_ex (buffer, font, 70, 20, makecol (255, 255, 255), makecol (0, 0, 0), "%s", pallette_tile -> getName().c_str());

  // Map info
  textprintf_ex (buffer, font, 0, 80, makecol (255, 255, 255), makecol (0, 0, 0), "height-%i width-%i", tile_map -> getHeight(), tile_map -> getWidth());
  if (layer == 1)
    textprintf_ex (buffer, font, 0, 130, makecol (255, 255, 255), makecol (0, 0, 0), "Editing Mode: Foreground");
  else
    textprintf_ex (buffer, font, 0, 130, makecol (255, 255, 255), makecol (0, 0, 0), "Editing Mode: Background");

  if (saving) {
    rectfill (buffer, 330, 300, NATIVE_SCREEN_W - 330, NATIVE_SCREEN_H - 400, makecol (255, 255, 255));
    rect (buffer, 330, 300, NATIVE_SCREEN_W - 330, NATIVE_SCREEN_H - 400, makecol (0, 0, 0));
    textprintf_centre_ex (buffer, font, 640, 310, makecol (0, 0, 0), -1, "Save Map Name");
    ib_save.Draw(buffer);
    btn_save.Draw(buffer);
  }
  else if (opening) {
    rectfill (buffer, 330, 300, NATIVE_SCREEN_W - 330, NATIVE_SCREEN_H - 400, makecol (255, 255, 255));
    rect (buffer, 330, 300, NATIVE_SCREEN_W - 330, NATIVE_SCREEN_H - 400, makecol (0, 0, 0));
    textprintf_centre_ex (buffer, font, 640, 310, makecol (0, 0, 0), -1, "Open Map Name");
    ib_open.Draw(buffer);
    btn_open.Draw(buffer);
  }

  // Cursor
  draw_sprite (buffer, cursor, MouseListener::x, MouseListener::y);
}
