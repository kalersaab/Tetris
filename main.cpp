#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstring>
#include <string>

using namespace sf;

const int M = 20;
const int N = 10;
const int SIZE = 30;

int field[M][N] = {0};

int figures[7][4] = {
    {1,3,5,7}, {2,4,5,7}, {3,5,4,6},
    {3,5,4,7}, {2,3,5,7}, {3,5,7,6},
    {2,3,4,5}
};

Color colors[8] = {
    Color::Black,
    Color(0,240,240), Color(240,0,0), Color(0,240,0),
    Color(160,0,240), Color(240,160,0),
    Color(0,0,240), Color(240,240,0)
};

struct Point { int x, y; };
Point a[4], b[4];

int colorNum;
bool gameOver = false;
int score = 0, level = 1, totalLines = 0;

bool check() {
    for (int i = 0; i < 4; i++) {
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return false;
        if (field[a[i].y][a[i].x]) return false;
    }
    return true;
}

void spawn(int fig) {
    colorNum = fig + 1;
    for (int i = 0; i < 4; i++) {
        a[i].x = figures[fig][i] % 2 + N / 2 - 1;
        a[i].y = figures[fig][i] / 2;
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    RenderWindow window(VideoMode(520, 600), "SFML Tetris");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("arial.ttf"))
        return -1;

    Text info("", font, 18);
    info.setPosition(330, 220);

    Text over("GAME OVER\nPress R to Restart", font, 28);
    over.setFillColor(Color::White);
    over.setStyle(Text::Bold);
    FloatRect ob = over.getLocalBounds();
    over.setOrigin(ob.width / 2, ob.height / 2);
    over.setPosition(N * SIZE / 2, M * SIZE / 2);

    RectangleShape block(Vector2f(SIZE - 1, SIZE - 1));
    block.setOutlineThickness(1);
    block.setOutlineColor(Color(30,30,30));

    RectangleShape previewBg(Vector2f(4 * SIZE, 4 * SIZE));
    previewBg.setPosition(N * SIZE + 20, 50);
    previewBg.setFillColor(Color(20,20,20));
    previewBg.setOutlineThickness(2);
    previewBg.setOutlineColor(Color(120,120,120));

    Clock clock;
    float timer = 0, delay = 0.5f;

    int dx = 0;
    bool rotate = false;

    int next = rand() % 7;
    int current = rand() % 7;
    spawn(current);

    while (window.isOpen()) {
        float time = clock.restart().asSeconds();
        timer += time;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                if (!gameOver) {
                    if (e.key.code == Keyboard::Left) dx = -1;
                    if (e.key.code == Keyboard::Right) dx = 1;
                    if (e.key.code == Keyboard::Up) rotate = true;
                }

                if (e.key.code == Keyboard::R) {
                    memset(field, 0, sizeof(field));
                    score = level = totalLines = 0;
                    delay = 0.5f;
                    gameOver = false;
                    current = rand() % 7;
                    next = rand() % 7;
                    spawn(current);
                }
            }
        }

        if (!gameOver) {
            // Move
            for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].x += dx; }
            if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];

            // Rotate
            if (rotate) {
                for (int i = 0; i < 4; i++) b[i] = a[i];
                Point p = a[1];
                for (int i = 0; i < 4; i++) {
                    int x = a[i].y - p.y;
                    int y = a[i].x - p.x;
                    a[i].x = p.x - x;
                    a[i].y = p.y + y;
                }
                if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];
            }

            // Soft drop
            if (Keyboard::isKeyPressed(Keyboard::Down))
                timer = delay;

            // Gravity
            if (timer > delay) {
                for (int i = 0; i < 4; i++) {
                    b[i] = a[i];
                    a[i].y++;
                }

                if (!check()) {
                    for (int i = 0; i < 4; i++)
                        field[b[i].y][b[i].x] = colorNum;

                    current = next;
                    next = rand() % 7;
                    spawn(current);

                    if (!check()) gameOver = true;
                }
                timer = 0;
            }

            // Line clear
            int k = M - 1, lines = 0;
            for (int i = M - 1; i >= 0; i--) {
                int count = 0;
                for (int j = 0; j < N; j++) {
                    if (field[i][j]) count++;
                    field[k][j] = field[i][j];
                }
                if (count < N) k--;
                else lines++;
            }

            if (lines) {
                totalLines += lines;
                level = totalLines / 10 + 1;
                static int scores[] = {0,100,300,500,800};
                score += scores[lines] * level;
                delay = std::max(0.1f, 0.5f - level * 0.03f);
            }
        }

        dx = 0;
        rotate = false;

        info.setString(
            "Score: " + std::to_string(score) +
            "\nLevel: " + std::to_string(level)
        );

        window.clear(Color::Black);

        // Board
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++) {
                if (!field[i][j]) continue;
                block.setPosition(j * SIZE, i * SIZE);
                block.setFillColor(colors[field[i][j]]);
                window.draw(block);
            }

        // Current piece
        if (!gameOver) {
            for (int i = 0; i < 4; i++) {
                block.setPosition(a[i].x * SIZE, a[i].y * SIZE);
                block.setFillColor(colors[colorNum]);
                window.draw(block);
            }
        }

        // Next preview
        window.draw(previewBg);
        for (int i = 0; i < 4; i++) {
            int x = figures[next][i] % 2;
            int y = figures[next][i] / 2;
            block.setPosition(N * SIZE + 20 + x * SIZE, 50 + y * SIZE);
            block.setFillColor(colors[next + 1]);
            window.draw(block);
        }

        window.draw(info);
        if (gameOver) window.draw(over);

        window.display();
    }
    return 0;
}