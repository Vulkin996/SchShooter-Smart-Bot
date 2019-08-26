#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

void startNetwork(int network);

void InitGame();
void on_keyboard_game(unsigned char key, int x, int y);

void keyboard_up_game(unsigned char key, int x, int y);


void on_mouse_pressed_released_game(int button, int state, int x, int y);

void on_mouse_move_active_game(int x, int y);

void on_mouse_move_game(int x, int y);


void on_timer_game();

void DrawHUDBar();
void DrawWepon();
void WriteText();

void DrawMap();

void DrawPlayers();

void DrawBullets();

void on_display_game(void);

void Clean(bool x);
#endif
