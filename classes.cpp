#include <cmath>
#include <sstream>
#include <random>
#include "init.h"
#include <thread>
#include <chrono>
#include <array>

const double PI = 3.14159265358979323846;
int getRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}

double distanceBetweenPoints(double x1, double y1, double x2, double y2) {
    return sqrt(pow((x1-x2), 2) + pow((y1- y2), 2));
}

bool isNumber(const std::string& s) {
    std::istringstream iss(s);
    double num;

    if (iss >> num) {
        char c;
        if (!(iss >> c)) {
            return true;
        }
    }
    return false;
}

int stringToInt(const std::string& str) {
    std::istringstream iss(str);
    int number;
    iss >> number;
    return number;
}

struct Point {
    double x;
    double y;
    Point (double x, double y): x(x), y(y) {}
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
        Button(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
        Button(int x1, int y1, int x2, int y2, std::string text) : x1(x1), y1(y1), x2(x2), y2(y2), text(text) {}
        ~Button() {}
        bool click(int mouse_x, int mouse_y) {
            if (mouse_x > x1 && mouse_x < x2 && mouse_y > y1 && mouse_y < y2) {
                return true;
            }
            return false;
        }
        int get_x1() const {
            return x1;
        }
        int get_y1() const {
            return y1;
        }
        int get_x2() const {
            return x2;
        }
        int get_y2() const {
            return y2;
        }
        std::string get_text() const {
            return text;
        }
        void draw(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b) {
            DrawRectangle(renderer, x1, y1, x2, y2, text, r, g, b);
        }
};

class Node {
    private:
        double x_coord;
        double y_coord;
        int radius = 5;
    public:
        Node(double x, double y): x_coord(x), y_coord(y) {}

        double get_x() const {
            return x_coord;
        }
        double get_y() const {
            return y_coord;
        }
        int get_radius() const {
            return radius;
        }
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
        int bot = 0;
    public:

        Field(int nodesNumber, int bot): nodesNumber(nodesNumber), bot(bot) {
            angle = 2*PI/nodesNumber;
            nodes = new Node*[nodesNumber];
            matrix = new int*[nodesNumber];
            for (int i = 0; i < nodesNumber; ++i) {
                matrix[i] = new int[nodesNumber];
            }
            for (int i = 0; i < nodesNumber; ++i) {
                Point point = getCoordByNumber(i);
                nodes[i] = new Node(point.x, point.y);
            }
        }
        Field() {}

        ~Field() {
            if (nodesNumber != 0) {
                for (int i = 0; i < nodesNumber; ++i) {
                    delete nodes[i];
                    delete[] matrix[i];
                }
                delete[] nodes;
                delete[] matrix;
                delete Back;
            }
        }
        Point getCoordByNumber(int n) {
            double delta_x = radius * std::sin(angle * n);
            double delta_y = radius * std::cos(angle * n);
            Point point(x_center + delta_x, y_center + delta_y);
            return point;
        }
        int get_radius() const {
            return radius;
        }
        Node** get_nodes() const {
            return nodes;
        }
        int** get_matrix() const {
            return matrix;
        }
        void finishGame() {
            if (game_finished) {
                std::cout << "Game is finished, player " << player+1 << " lost" << std::endl;
                score[player] += 1;
                clear_matrix();
                if ((score[0] + score[1]) % 2 == 0) {
                    player = 1;
                } else {
                    player = 0;
                }
                firstMove = true;
                game_finished = false;
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        }
        void connectNodes() {
            matrix[node_from][node_to] = player+1;
            matrix[node_to][node_from] = player+1;
            if (end_game(node_from, node_to)) {
                game_finished = true;
            }
            clicks = 0;
            chose = false;
            node_from = -1;
            node_to = -1;
            player = (player+1)%2;
        }
        int check_connection() {
            if (node_from == -1) {
                return true;
            }
            if (matrix[node_from][flag] == 0 && matrix[flag][node_from] == 0) {
                end_game(node_from, flag);
                return true;
            }
            return false;
        }
        bool end_game(int from, int to) {
            for (int i = 0; i < nodesNumber; ++i) {
                if (matrix[i][to] == matrix[from][to] && i != from) {
                    for (int j = 0; j < nodesNumber; ++j) {
                        if (matrix[i][j] == matrix[from][to] && j == from) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }
        void clear_matrix() {
            for (int i = 0; i < nodesNumber; ++i) {
                for (int j = 0; j < nodesNumber; ++j) {
                    matrix[i][j] = 0;
                }
            }
        }
        void draw(SDL_Renderer* renderer, int mouseX, int mouseY) {
            if (bot == 0) {
                bot_move();
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            Back->draw(renderer, 255, 255, 255);
            DrawRectangle(renderer, 680, 20, 730, 70, std::to_string(score[0]), 36, 3, 255);
            DrawRectangle(renderer, 730, 20, 780, 70, std::to_string(score[1]), 3, 255, 87);
            if (firstMove) {
                if (player == 0) {
                    DrawRectangle(renderer, Width/2-100, Height/2+50, Width/2 + 100, Height/2 + 100, "Blue Moves First", 0, 0, 0);
                }
                else {
                    DrawRectangle(renderer, Width/2-100, Height/2+50, Width/2 + 100, Height/2 + 100, "Green Moves First", 0, 0, 0);
                }
            }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            DrawCircle(renderer, Width/2, Height/2, radius);
            flag = -1;
            for (int i = 0; i < nodesNumber; ++i) {
                if (distanceBetweenPoints(nodes[i]->get_x(), nodes[i]->get_y(), mouseX, mouseY) <= detection_radius) {
                    DrawFilledCircle(renderer, nodes[i]->get_x(), nodes[i]->get_y(), nodes[i]->get_radius(), 255, 20, 0);
                    flag = i;
                } else {
                    DrawFilledCircle(renderer, nodes[i]->get_x(), nodes[i]->get_y(), nodes[i]->get_radius(), 255, 255, 255);
                }

                for (int j = i+1; j < nodesNumber; ++j) {
                    if (matrix[i][j] == 1) {
                        DrawLine(renderer, nodes[i]->get_x(), nodes[i]->get_y(), nodes[j]->get_x(), nodes[j]->get_y(), 36, 3, 255);
                    }
                    if (matrix[i][j] == 2) {
                        DrawLine(renderer, nodes[i]->get_x(), nodes[i]->get_y(), nodes[j]->get_x(), nodes[j]->get_y(), 3, 255, 87);
                    }
                }
            }
            if (node_from != -1) {
                DrawFilledCircle(renderer, nodes[node_from]->get_x(), nodes[node_from]->get_y(), nodes[node_from]->get_radius(), 250, 233, 2);
            }
        }
        void handleEvent(int mouseX, int mouseY) {
            firstMove = false;
            if (Back->click(mouseX, mouseY)) {
                currentState = MENU;
            }
            if (flag != -1 && flag != node_from && check_connection()) {
                clicks = (clicks+1) % 3;
                if (clicks == 1 && chose != true) {
                    chose = true;
                    node_from = flag;
                }
                if (clicks == 2 && chose == true) {
                    node_to = flag;
                    connectNodes();
                }
            } else if (flag == -1) {
                clicks = 0;
                node_from = -1;
                node_to = -1;
                chose = false;
            }
        }
        int bot_move() {
            if (player == 0) {
                return 0;
            }

            int**  moves = new int*[nodesNumber-1];
            for (int i = 0; i < nodesNumber-1; ++i) {
                moves[i] = new int[nodesNumber-i-1];
                for (int j = 0; j < nodesNumber-i-1; ++j) {
                    moves[i][j] = 0;
                }
            }
            int counter = 0;
            for (int i = 0; i < nodesNumber-1; ++i) {
                int k = 0;
                for (int j = i+1; j < nodesNumber; ++j) {
                    if (matrix[i][j] == 0 && matrix[j][i] == 0) {
                        moves[i][k] = 1;
                        ++counter;
                    }
                    ++k;
                }
            }
            int move = getRandomNumber(1, counter);
            counter = 0;
            for (int i = 0; i < nodesNumber-1; ++i) {
                for (int j = 0; j < nodesNumber-i-1; ++j) {
                    if (moves[i][j] == 1) {
                        ++counter;
                        if (counter == move) {
                            node_from = i;
                            node_to = j+i+1;    
                            connectNodes();
                            break;
                        }
                    }
                }
            }
            for (int i = 0; i < nodesNumber-1; ++i) {
                delete[] moves[i];
            }
            delete[] moves;
            return 1;
            
        }

};

class Menu {
    private:
        int Width = 800;
        int Height = 640;
        Button* buttons[3];
        std::string NodesNumber = "";
    public:
    Menu() {
        buttons[0] = new Button(Width/2-200, 160, Width/2+200, 160+120, "1 Player");
        buttons[1] = new Button(Width/2-200, 320, Width/2+200, 320+120, "2 Players");
        buttons[2] = new Button(Width/2-200, 480, Width/2+200, 480+120, "Exit the Game");
    }
    Menu(int Width) : Width(Width) {
        buttons[0] = new Button(Width/2-200, 160, Width/2+200, 160+120, "1 Player");
        buttons[1] = new Button(Width/2-200, 320, Width/2+200, 320+120, "2 Players");
        buttons[2] = new Button(Width/2-200, 480, Width/2+200, 480+120, "Exit the Game");
    }
    ~Menu() {
        for (int i = 0; i < 3; ++i) {
            delete buttons[i];
        }
    }
    std::string get_nodesNumber() const {
        return NodesNumber;
    }
    void addSymbol(std::string symbol) {
        NodesNumber += symbol;
    }
    void deleteSymbol() {
        NodesNumber.pop_back();
    }
    void draw(SDL_Renderer* renderer, int mouseX, int mouseY) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        DrawRectangle(renderer, 10, 10, Width-10, Height-10, 255, 255, 255);
        DrawRectangle(renderer, Width/2-50, 50, Width/2+50, 100, NodesNumber, 255, 255, 255);
        
        for (int i = 0; i < 3; ++i) {
            if (buttons[i]->click(mouseX, mouseY)) {
                buttons[i]->draw(renderer, 255, 3, 217);
            } else {
                buttons[i]->draw(renderer, 255, 255, 255);
            }
        }

    }
    std::array<int, 2> handleEvent(int mouseX, int moouseY) {
        for (int i = 0; i < 3; ++i) {
            if (buttons[i]->click(mouseX, moouseY)) {
                if (i == 0 || i == 1) {
                    if (isNumber(NodesNumber) && stringToInt(NodesNumber) >= 5 && stringToInt(NodesNumber) <= 20) {
                        currentState = GAME;
                        return {stringToInt(NodesNumber), i};
                    } else {
                        std::cerr << "Invalid input number" << std::endl;
                    }
                }
                if (i == 2) {
                    return {-1, -1};
                }
            }
        }
        return {10, 10};
    }
};