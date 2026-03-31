#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>

using namespace std;

const int width = 15;
const int height = 15;
int cannonX = width / 2;
bool gameOver = false;
int score = 0;
int frameCount = 0;

struct Bullet {
    int x, y;
};

struct Star {
    int x, y;
};

vector<Bullet> bullets;
vector<Star> stars;

// Move cursor to top-left (no flicker, no stray lines)
void SetCursorPosition(int x, int y) {
    COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Hide blinking console cursor
void HideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void Setup() {
    cannonX = width / 2;
    gameOver = false;
    score = 0;
    frameCount = 0;
    bullets.clear();
    stars.clear();
    srand(static_cast<unsigned>(time(0)));
    system("cls");
    HideCursor();
}

void Draw() {
    SetCursorPosition(0, 0);

    // Top wall
    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << "\n";

    // Game grid
    for (int y = 0; y < height; y++) {
        cout << "#";
        for (int x = 0; x < width; x++) {
            char ch = ' ';

            // Cannon
            if (y == height - 1 && x == cannonX)
                ch = '^';

            // Bullets
            for (const auto& b : bullets)
                if (b.x == x && b.y == y)
                    ch = '|';

            // Stars
            for (const auto& s : stars)
                if (s.x == x && s.y == y)
                    ch = '*';

            cout << ch;
        }
        cout << "#\n";
    }

    // Bottom wall
    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << "\nScore: " << score;
    cout << "\nControls: A = Left, D = Right, SPACE = Shoot, Q = Quit\n";
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a': case 'A':
            if (cannonX > 0) cannonX--;
            break;
        case 'd': case 'D':
            if (cannonX < width - 1) cannonX++;
            break;
        case ' ':
            bullets.push_back({ cannonX, height - 2 });
            break;
        case 'q': case 'Q':
            gameOver = true;
            break;
        }
    }
}

void Logic() {
    frameCount++;

    // Move bullets
    for (int i = 0; i < bullets.size(); i++) {
        bullets[i].y--;
        if (bullets[i].y < 0) {
            bullets.erase(bullets.begin() + i);
            i--;
        }
    }

    // Move stars every 3 frames
    if (frameCount % 3 == 0) {
        for (int i = 0; i < stars.size(); i++) {
            stars[i].y++;
            if (stars[i].y >= height - 1) {
                gameOver = true;
                return;
            }
        }
    }

    // Bullet-star collision
    for (int i = 0; i < bullets.size(); i++) {
        for (int j = 0; j < stars.size(); j++) {
            if (bullets[i].x == stars[j].x && bullets[i].y == stars[j].y) {
                bullets.erase(bullets.begin() + i);
                stars.erase(stars.begin() + j);
                score += 10;
                i--;
                break;
            }
        }
    }

    // Add new star
    if (frameCount % 10 == 0 && rand() % 100 < 60) {
        stars.push_back({ rand() % width, 0 });
    }
}

void GameOverScreen() {
    SetCursorPosition(0, height + 5);
    cout << "\n========= GAME OVER =========\n";
    cout << "Final Score: " << score << endl;
    cout << "Press any key to exit...\n";
    _getch();
}

int main() {
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(120);
    }
    GameOverScreen();
    return 0;
}