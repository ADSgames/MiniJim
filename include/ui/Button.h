#ifndef BUTTON_H
#define BUTTON_H

#include <allegro.h>
#include <functional>

class Button {
  public:
    Button ();
    Button (int x, int y);
    ~Button();

    void Update();

    void SetImages (const char *image1, const char *image2);

    int GetX();
    int GetY();

    void SetOnClick(std::function<void()> func);

    void Draw (BITMAP *buffer);

    bool Hover();

  private:
    std::function<void(void)> OnClick;

    int height;
    int width;

    int x;
    int y;

    BITMAP *images[2];
};

#endif

