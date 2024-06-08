#ifndef CLASSES_H
#define CLASSES_H


#include "init.h"
#include <array>
#include <cmath>
#include <random>
#include <thread>
#include <chrono>
#include <sstream>

struct Point {
    double x;
    double y;
    Point(double x, double y);
    ~Point() {}
};

class Button {
private:
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    std::string text = "";

public:
    Button(int x1, int y1, int x2, int y2);
    Button(int x1, int y1, int x2, int y2, std::string text);
    ~Button() {}
    bool click(int mouse_x, int mouse_y);
    int get_x1() const;
    int get_y1() const;
    int get_x2() const;
    int get_y2() const;
    std::string get_text() const;
    void draw(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b);
};

class Node {
private:
    double x_coord;
    double y_coord;
    int radius = 5;

public:
    Node(double x, double y);
    double get_x() const;
    double get_y() const;
    int get_radius() const;
    ~Node() {}
};

class Field {
private:
    int nodesNumber = 0;
    Node** nodes;
    int radius = 300;
    int x_center = 400;
    int y_center = 320;
    double angle = 0;
    int** matrix;
    int Width = 800;
    int Height = 640;
    int flag = -1;
    int detection_radius = 45;
    int node_from = -1;
    int node_to = -1;
    int clicks = 0;
    bool chose = false;
    int player = 0;
    Button* Back = new Button(20, 20, 120, 70, "Back to Menu");
    int score[2] = {0, 0};
    bool game_finished = false;
    bool firstMove = true;
    int bot = -1;

public:
    Field(int nodesNumber, int bot);
    Field();
    ~Field();
    Point getCoordByNumber(int n);
    int get_bot() const;
    void finishGame();
    void connectNodes();
    int check_connection();
    bool end_game(int from, int to);
    void clear_matrix();
    void draw(SDL_Renderer* renderer, int mouseX, int mouseY);
    void handleEvent(int mouseX, int mouseY);
    int bot_move();
};

class Menu {
private:
    int Width = 800;
    int Height = 640;
    Button* buttons[3];
    std::string NodesNumber = "";

public:
    Menu();
    Menu(int Width);
    ~Menu();
    std::string get_nodesNumber() const;
    void addSymbol(std::string symbol);
    void deleteSymbol();
    void draw(SDL_Renderer* renderer, int mouseX, int mouseY);
    std::array<int, 2> handleEvent(int mouseX, int moouseY);
};

#endif
