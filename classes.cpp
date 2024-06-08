#include "classes.h"

const double PI = 3.14159265358979323846;

int getRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);

    return distr(gen);
}

double distanceBetweenPoints(double x1, double y1, double x2, double y2) {
    return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
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

Point::Point(double x, double y): x(x), y(y) {}

Button::Button(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

Button::Button(int x1, int y1, int x2, int y2, std::string text) : x1(x1), y1(y1), x2(x2), y2(y2), text(text) {}

bool Button::click(int mouse_x, int mouse_y) {
    if (mouse_x > x1 && mouse_x < x2 && mouse_y > y1 && mouse_y < y2) {
        return true;
    }
    return false;
}

int Button::get_x1() const {
    return x1;
}

int Button::get_y1() const {
    return y1;
}

int Button::get_x2() const {
    return x2;
}

int Button::get_y2() const {
    return y2;
}

std::string Button::get_text() const {
    return text;
}

void Button::draw(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b) {
    DrawRectangle(renderer, x1, y1, x2, y2, text, r, g, b);
}

Node::Node(double x, double y): x_coord(x), y_coord(y) {}

double Node::get_x() const {
    return x_coord;
}
double Node::get_y() const {
    return y_coord;
}
int Node::get_radius() const {
    return radius;
}

Field::Field(int nodesNumber, int bot): nodesNumber(nodesNumber), bot(bot) {
    angle = 2 * PI / nodesNumber;
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

Field::Field() {}

Field::~Field() {
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

Point Field::getCoordByNumber(int n) {
    double delta_x = radius * std::sin(angle * n);
    double delta_y = radius * std::cos(angle * n);
    Point point(x_center + delta_x, y_center + delta_y);
    return point;
}

int Field::get_bot() const {
    return bot;
}

void Field::finishGame() {
    if (game_finished) {
        std::cout << "Game is finished, player " << player + 1 << " lost" << std::endl;
        score[player] += 1;
        clear_matrix();
        if ((score[0] + score[1]) % 2 == 0) {
            player = 0;
        } else {
            player = 1;
        }
        firstMove = true;
        game_finished = false;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void Field::connectNodes() {
    matrix[node_from][node_to] = player + 1;
    matrix[node_to][node_from] = player + 1;
    if (end_game(node_from, node_to)) {
        game_finished = true;
    }
    clicks = 0;
    chose = false;
    node_from = -1;
    node_to = -1;
    player = (player + 1) % 2;
}

int Field::check_connection() {
    if (node_from == -1) {
        return true;
    }
    if (matrix[node_from][flag] == 0 && matrix[flag][node_from] == 0) {
        end_game(node_from, flag);
        return true;
    }
    return false;
}

bool Field::end_game(int from, int to) {
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

void Field::clear_matrix() {
    for (int i = 0; i < nodesNumber; ++i) {
        for (int j = 0; j < nodesNumber; ++j) {
            matrix[i][j] = 0;
        }
    }
}

void Field::draw(SDL_Renderer* renderer, int mouseX, int mouseY) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    Back->draw(renderer, 255, 255, 255);
    DrawRectangle(renderer, 680, 20, 730, 70, std::to_string(score[0]), 36, 3, 255);
    DrawRectangle(renderer, 730, 20, 780, 70, std::to_string(score[1]), 3, 255, 87);
    if (firstMove) {
        if (player == 0) {
            DrawRectangle(renderer, Width / 2 - 100, Height / 2 + 50, Width / 2 + 100, Height / 2 + 100, "Blue Moves First", 0, 0, 0);
        } else {
            DrawRectangle(renderer, Width / 2 - 100, Height / 2 + 50, Width / 2 + 100, Height / 2 + 100, "Green Moves First", 0, 0, 0);
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


void Field::handleEvent(int mouseX, int mouseY) {
    firstMove = false;
    if (Back->click(mouseX, mouseY)) {
        currentState = MENU;
    }
    if (flag != -1 && flag != node_from && check_connection()) {
        clicks = (clicks + 1) % 3;
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

int Field::bot_move() {
    if (player == 0) {
        return 0;
    }

    int** moves = new int*[nodesNumber - 1];
    for (int i = 0; i < nodesNumber - 1; ++i) {
        moves[i] = new int[nodesNumber - i - 1];
        for (int j = 0; j < nodesNumber - i - 1; ++j) {
            moves[i][j] = 0;
        }
    }
    int counter = 0;
    for (int i = 0; i < nodesNumber - 1; ++i) {
        int k = 0;
        for (int j = i + 1; j < nodesNumber; ++j) {
            if (matrix[i][j] == 0 && matrix[j][i] == 0) {
                moves[i][k] = 1;
                ++counter;
            }
            ++k;
        }
    }
    int move = getRandomNumber(1, counter);
    counter = 0;
    for (int i = 0; i < nodesNumber - 1; ++i) {
        for (int j = 0; j < nodesNumber - i - 1; ++j) {
            if (moves[i][j] == 1) {
                ++counter;
                if (counter == move) {
                    node_from = i;
                    node_to = j + i + 1;
                    connectNodes();
                    player = 0;
                    break;
                }
            }
        }
    }
    for (int i = 0; i < nodesNumber - 1; ++i) {
        delete[] moves[i];
    }
    delete[] moves;
    return 1;
}

Menu::Menu() {
    buttons[0] = new Button(Width / 2 - 200, 160, Width / 2 + 200, 160 + 120, "1 Player");
    buttons[1] = new Button(Width / 2 - 200, 320, Width / 2 + 200, 320 + 120, "2 Players");
    buttons[2] = new Button(Width / 2 - 200, 480, Width / 2 + 200, 480 + 120, "Exit the Game");
}

Menu::Menu(int Width) : Width(Width) {
    buttons[0] = new Button(Width / 2 - 200, 160, Width / 2 + 200, 160 + 120, "1 Player");
    buttons[1] = new Button(Width / 2 - 200, 320, Width / 2 + 200, 320 + 120, "2 Players");
    buttons[2] = new Button(Width / 2 - 200, 480, Width / 2 + 200, 480 + 120, "Exit the Game");
}

Menu::~Menu() {
    for (int i = 0; i < 3; ++i) {
        delete buttons[i];
    }

}

std::string Menu::get_nodesNumber() const {
    return NodesNumber;
}

void Menu::addSymbol(std::string symbol) {
    NodesNumber += symbol;
}

void Menu::deleteSymbol() {
    NodesNumber.pop_back();
}

void Menu::draw(SDL_Renderer* renderer, int mouseX, int mouseY) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    DrawRectangle(renderer, 10, 10, Width - 10, Height - 10, 255, 255, 255);
    DrawRectangle(renderer, Width / 2 - 50, 50, Width / 2 + 50, 100, NodesNumber, 255, 255, 255);

    for (int i = 0; i < 3; ++i) {
        if (buttons[i]->click(mouseX, mouseY)) {
            buttons[i]->draw(renderer, 255, 3, 217);
        } else {
            buttons[i]->draw(renderer, 255, 255, 255);
        }
    }
}


std::array<int, 2> Menu::handleEvent(int mouseX, int mouseY) {
    for (int i = 0; i < 3; ++i) {
        if (buttons[i]->click(mouseX, mouseY)) {
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