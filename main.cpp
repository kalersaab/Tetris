#include <SFML/Graphics.hpp>
#include <ctime>

using namespace sf;

// Board size
const int M = 20;
const int N = 10;
const int SIZE = 30;

// Game field
int field[M][N] = {0};

// Tetromino shapes
int figures[7][4] = {
    {1,3,5,7}, // I
    {2,4,5,7}, // Z
    {3,5,4,6}, // S
    {3,5,4,7}, // T
    {2,3,5,7}, // L
    {3,5,7,6}, // J
    {2,3,4,5}  // O
};

// Colors
Color colors[8] = {
    Color::Black,
    Color(0,240,240),   // I
    Color(240,0,0),     // Z
    Color(0,240,0),     // S
    Color(160,0,240),   // T
    Color(240,160,0),   // L
    Color(0,0,240),     // J
    Color(240,240,0)    // O
};

struct Point { int x, y; };
Point a[4], b[4];

int colorNum;
int nextFigure;

// Collision check
bool check() {
    for (int i = 0; i < 4; i++) {
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M)
            return false;
        if (field[a[i].y][a[i].x])
            return false;
    }
    return true;
}

// Spawn piece
void spawnPiece(int fig) {
    colorNum = fig + 1;
    for (int i = 0; i < 4; i++) {
        a[i].x = figures[fig][i] % 2 + N / 2 - 1;
        a[i].y = figures[fig][i] / 2;
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    RenderWindow window(VideoMode(500, 600), "SFML Tetris");
    window.setFramerateLimit(60);

    Clock clock;
    float timer = 0, delay = 0.5f;
    int dx = 0;
    bool rotate = false;

    nextFigure = rand() % 7;
    int current = nextFigure;
    nextFigure = rand() % 7;
    spawnPiece(current);

    RectangleShape block(Vector2f(SIZE - 1, SIZE - 1));
    block.setOutlineThickness(1);
    block.setOutlineColor(Color(30,30,30));

    // Preview box
    int previewX = N * SIZE + 20;
    int previewY = 50;

    RectangleShape previewBg(Vector2f(4 * SIZE, 4 * SIZE));
    previewBg.setPosition(previewX, previewY);
    previewBg.setFillColor(Color(20,20,20));
    previewBg.setOutlineThickness(2);
    previewBg.setOutlineColor(Color(100,100,100));

    while (window.isOpen()) {
        float time = clock.restart().asSeconds();
        timer += time;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Up) rotate = true;
                else if (e.key.code == Keyboard::Left) dx = -1;
                else if (e.key.code == Keyboard::Right) dx = 1;
                else if (e.key.code == Keyboard::Down) delay = 0.05f;
            }
        }

        // Move
        for (int i = 0; i < 4; i++) {
            b[i] = a[i];
            a[i].x += dx;
        }
        if (!check())
            for (int i = 0; i < 4; i++)
                a[i] = b[i];

        // Rotate
        if (rotate) {
            Point p = a[1];
            for (int i = 0; i < 4; i++) {
                int x = a[i].y - p.y;
                int y = a[i].x - p.x;
                a[i].x = p.x - x;
                a[i].y = p.y + y;
            }
            if (!check())
                for (int i = 0; i < 4; i++)
                    a[i] = b[i];
        }

        // Gravity
        if (timer > delay) {
            for (int i = 0; i < 4; i++) {
                b[i] = a[i];
                a[i].y++;
            }

            if (!check()) {
                for (int i = 0; i < 4; i++)
                    field[b[i].y][b[i].x] = colorNum;

                current = nextFigure;
                nextFigure = rand() % 7;
                spawnPiece(current);
            }
            timer = 0;
        }

        // Line clear
        int k = M - 1;
        for (int i = M - 1; i >= 0; i--) {
            int count = 0;
            for (int j = 0; j < N; j++) {
                if (field[i][j]) count++;
                field[k][j] = field[i][j];
            }
            if (count < N) k--;
        }

        dx = 0;
        rotate = false;
        delay = 0.5f;

        // ---- DRAW ----
        window.clear(Color::Black);

        // Field
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                if (!field[i][j]) continue;
                block.setPosition(j * SIZE, i * SIZE);
                block.setFillColor(colors[field[i][j]]);
                window.draw(block);
            }

        // Current piece
        for (int i = 0; i < 4; i++) {
            block.setPosition(a[i].x * SIZE, a[i].y * SIZE);
            block.setFillColor(colors[colorNum]);
            window.draw(block);
        }

        // Preview
        window.draw(previewBg);
        for (int i = 0; i < 4; i++) {
            int x = figures[nextFigure][i] % 2;
            int y = figures[nextFigure][i] / 2;
            block.setPosition(previewX + x * SIZE, previewY + y * SIZE);
            block.setFillColor(colors[nextFigure + 1]);
            window.draw(block);
        }

        window.display();
    }
    return 0;
}