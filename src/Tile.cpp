#include "Tile.h"

#include "globals.h"
#include "utility/tools.h"

#include "TileTypeLoader.h"

tile::tile (short type) {
  setType (type);
}

tile::tile (short type, int x, int y) {
  setX (x);
  setY (y);
  setType (type);
}

tile::~tile() {

}

//Getters/ setters
int tile::getX() const {
  return x;
}

int tile::getY() const {
  return y;
}

int tile::getWidth() {
  if (t_type)
    return t_type -> GetWidth();
  return 0;
}

int tile::getHeight() {
  if (t_type)
    return t_type -> GetHeight();
  return 0;
}

void tile::setX (int x) {
  this -> x = x;
}

void tile::setY (int y) {
  this -> y = y;
}

short tile::getType() {
  if (t_type)
    return t_type -> GetID();
  return 0;
}

std::string tile::getName() {
  if (t_type)
    return t_type -> GetName();
  return "";
}

// Contains Attribute
bool tile::containsAttribute (int newAttribute) {
  return t_type -> HasAttribute(newAttribute);
}

// Set type
void tile::setType (short type) {
  t_type = TileTypeLoader::GetTile(type);
}

// Draw tile
void tile::draw_tile (BITMAP *buffer, int xOffset, int yOffset, int frame) {
  if (t_type)
    t_type -> Draw(buffer, x - xOffset, y - yOffset, frame);
}
