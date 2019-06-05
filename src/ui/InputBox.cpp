#include "ui/InputBox.h"

#include <iostream>

#include "utility/MouseListener.h"

InputBox::InputBox() {
  this -> x = 0;
  this -> y = 0;
  this -> width = 0;
  this -> height = 0;
  this -> text = "";
  this -> focused = false;
  text_iter = 0;
}

InputBox::InputBox(int x, int y, int width, int height, std::string value, std::string type) :
  InputBox() {
  this -> x = x;
  this -> y = y;
  this -> width = width;
  this -> height = height;
  this -> text = value;
  this -> type = type;
}

InputBox::~InputBox() {

}

void InputBox::Focus() {
  focused = true;
}

std::string InputBox::GetValue() {
  return text;
}

bool InputBox::Hover() {
  return mouse_x > x && mouse_x < x + width && mouse_y > y && mouse_y < y + height;
}

void InputBox::Update() {
  // Focus
  if (MouseListener::mouse_pressed & 1) {
    focused = Hover();
    if (focused) {
      int closest = width;
      for (unsigned int i = 0; i <= text.length(); i++) {
        int distance = abs(text_length(font, text.substr(0, i).c_str()) + x + 3 - mouse_x);
        if (distance < closest) {
          text_iter = i;
          closest = distance;
        }
      }
    }
    clear_keybuf();
  }

  if (!focused || !keypressed())
    return;

  int  newkey   = readkey();
  char ASCII    = newkey & 0xff;
  char scancode = newkey >> 8;

  // a character key was pressed; add it to the string
  if (type == "number") {
    if (ASCII >= 48 && ASCII <= 57 && text_length(font, (text + ASCII).c_str()) < width) {
      text.insert (text.begin() + text_iter, ASCII);
      text_iter++;
      return;
    }
  }

  else if (type == "text") {
    if (ASCII >= 32 && ASCII <= 126 && text_length(font, (text + ASCII).c_str()) < width) {
      text.insert (text.begin() + text_iter, ASCII);
      text_iter++;
      return;
    }
  }

  // some other, "special" key was pressed; handle it here
  if (scancode == KEY_BACKSPACE) {
    if (text_iter != 0) {
      text_iter--;
      text.erase (text.begin() + text_iter);
    }
  }

  else if (scancode == KEY_RIGHT) {
    if (text_iter != text.size()) {
      text_iter++;
    }
  }

  else if (scancode == KEY_LEFT) {
    if (text_iter != 0) {
      text_iter--;
    }
  }
}

// Draw box
void InputBox::Draw(BITMAP *buffer) {
  rectfill (buffer, x, y, x + width, y + height, makecol (12, 12, 12));

  int col = (Hover() || focused) ? makecol (230, 230, 230) : makecol (245, 245, 245);

  if (focused)
    rectfill (buffer, x + 2, y + 2, x + width - 2, y + height - 2, col);
  else
    rectfill (buffer, x + 1, y + 1, x + width - 1, y + height - 1, col);

  // Output the string to the screen
  textout_ex (buffer, font, text.c_str(), x + 6, y, makecol (12, 12, 12), -1);

  // Draw the caret
  if (focused)
    vline (buffer, text_length(font, text.substr(0, text_iter).c_str()) + x + 3, y + 8, y + height - 8, makecol (0, 0, 0));
}
