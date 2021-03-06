/**
 * State for machine and State Engine
 * Allan Legemaate
 * 30/12/2016
 * Compartmentalize program into states
 *   which can handle only their own logic,
 *   drawing and transitions
 */

#ifndef STATE_H
#define STATE_H

#include <allegro.h>

// Class
class State;

/*****************
 * STATE ENGINE
 *****************/
class StateEngine {
 public:
  // Init
  StateEngine();

  // Update
  void update();

  // Draw
  void draw(BITMAP* buffer);

  // Set next state
  void setNextState(const int newState);

  // Get state id
  int getStateId() const;

  // Game states
  enum programStates {
    STATE_NULL,
    STATE_INIT,
    STATE_INTRO,
    STATE_MENU,
    STATE_EDIT,
    STATE_GAME,
    STATE_EXIT,
  };

 private:
  // Change state
  void changeState();

  // Next state
  int nextState = STATE_NULL;

  // State id
  int currentState = STATE_NULL;

  // Stores states
  State* state;
};

/*********
 * STATE
 *********/
class State {
 public:
  // Virtual destructor
  State(){};
  virtual ~State(){};

  // Draw to screen
  virtual void draw(BITMAP* buffer) = 0;

  // Update logic
  virtual void update(StateEngine& engine) = 0;

  // Change state
  static void setNextState(StateEngine& engine, int state);

 private:
  // Disallow copy
  State(const State&);
  State& operator=(const State&);
};

#endif  // STATE_H
