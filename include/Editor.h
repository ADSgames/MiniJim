#ifndef EDITOR_H
#define EDITOR_H

#include "State.h"

#include <allegro.h>

#include "TileMap.h"
#include "Tile.h"
#include "ui/InputBox.h"
#include "ui/Button.h"
#include "Camera.h"

class Editor : public State {
  public:
    Editor();
    virtual ~Editor();
    virtual void update(StateEngine *engine) override;
    virtual void draw(BITMAP *buffer) override;

  private:
    void SaveClicked();
    void OpenClicked();

    int layer;
    int draw_layer;

    bool saving, opening, creating;

    TileMap *tile_map;
    tile *pallette_tile;
    BITMAP *cursor;

    //Text input
    Button btn_save, btn_open, btn_new, btn_close;
    InputBox ib_save, ib_open, ib_width, ib_height;

    void Close();
    void Save();
    void Open();
    void New();
    void Edit();

    Camera cam;
};

#endif // EDITOR_H
