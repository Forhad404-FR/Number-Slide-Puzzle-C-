#include <iostream>
#include <iomanip>
#include <algorithm>
#include <array>
#include <chrono>
#include <random>
#include <limits>
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>
#include <climits>

using namespace std;

class PuzzleGame {
private:
    static const int SIZE = 4;
    int board[SIZE][SIZE]{};
    int moves = 0;
    int emptyRow = SIZE - 1;
    int emptyCol = SIZE - 1;
    mt19937 rng;

    void createSolvedBoard() {
        int value = 1;

        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (row == SIZE - 1 && col == SIZE - 1) {
                    board[row][col] = 0;
                } else {
                    board[row][col] = value++;
                }
            }
        }

        emptyRow = SIZE - 1;
        emptyCol = SIZE - 1;
    }

    bool isSolvable(const array<int, 16>& tiles) const {
        int inversions = 0;
        int blankRowFromBottom = 0;

        for (int i = 0; i < 16; i++) {
            if (tiles[i] == 0) {
                blankRowFromBottom = SIZE - (i / SIZE);
                continue;
            }

            for (int j = i + 1; j < 16; j++) {
                if (tiles[j] != 0 && tiles[i] > tiles[j]) {
                    inversions++;
                }
            }
        }

        return (inversions + blankRowFromBottom) % 2 == 1;
    }
    
    bool isSolved() const {
        int expected = 1;

        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                if (row == SIZE - 1 && col == SIZE - 1) {
                    return board[row][col] == 0;
                }

                if (board[row][col] != expected++) {
                    return false;
                }
            }
        }

        return true;
    }

    bool findTile(int tile, int& row, int& col) const {
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (board[r][c] == tile) {
                    row = r;
                    col = c;
                    return true;
                }
            }
        }

        return false;
    }

    bool moveTile(int tile) {
        if (tile < 1 || tile > 15) {
            cout << "Please enter a tile number from 1 to 15.\n";
            return false;
        }

        int tileRow, tileCol;

        if (!findTile(tile, tileRow, tileCol)) {
            cout << "That tile was not found.\n";
            return false;
        }

        int distance = abs(tileRow - emptyRow) + abs(tileCol - emptyCol);

        if (distance != 1) {
            cout << "Invalid move! Choose a tile next to the empty space.\n";
            return false;
        }

        board[emptyRow][emptyCol] = board[tileRow][tileCol];
        board[tileRow][tileCol] = 0;

        emptyRow = tileRow;
        emptyCol = tileCol;
        moves++;

        return true;
    }

    void displayBoard() const {
        cout << "\n";
        cout << "        +------+------+------+------+\n";

        for (int row = 0; row < SIZE; row++) {
            cout << "        ";

            for (int col = 0; col < SIZE; col++) {
                if (board[row][col] == 0) {
                    cout << "|      ";
                } else {
                    cout << "| " << setw(4) << board[row][col] << " ";
                }
            }

            cout << "|\n";
            cout << "        +------+------+------+------+\n";
        }

        cout << "\n";
    }

    void shuffleBoard(int shuffleMoves) {
        createSolvedBoard();

        int previousEmptyRow = -1;
        int previousEmptyCol = -1;

        for (int step = 0; step < shuffleMoves; step++) {
            vector<pair<int, int>> possibleMoves;

            const int rowChange[4] = {-1, 1, 0, 0};
            const int colChange[4] = {0, 0, -1, 1};

            for (int direction = 0; direction < 4; direction++) {
                int newRow = emptyRow + rowChange[direction];
                int newCol = emptyCol + colChange[direction];

                if (newRow < 0 || newRow >= SIZE || newCol < 0 || newCol >= SIZE) {
                    continue;
                }

                if (newRow == previousEmptyRow && newCol == previousEmptyCol) {
                    continue;
                }

                possibleMoves.push_back({newRow, newCol});
            }

            if (possibleMoves.empty()) {
                for (int direction = 0; direction < 4; direction++) {
                    int newRow = emptyRow + rowChange[direction];
                    int newCol = emptyCol + colChange[direction];

                    if (newRow >= 0 && newRow < SIZE && newCol >= 0 && newCol < SIZE) {
                        possibleMoves.push_back({newRow, newCol});
                    }
                }
            }

            uniform_int_distribution<int> choose(0, static_cast<int>(possibleMoves.size()) - 1);
            pair<int, int> selected = possibleMoves[choose(rng)];

            previousEmptyRow = emptyRow;
            previousEmptyCol = emptyCol;

            board[emptyRow][emptyCol] = board[selected.first][selected.second];
            board[selected.first][selected.second] = 0;

            emptyRow = selected.first;
            emptyCol = selected.second;
        }

        moves = 0;
    }
    
    static int readNumber(const string& prompt) {
        int number;

        while (true) {
            cout << prompt;

            if (cin >> number) {
                return number;
            }

            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    void showResult(chrono::steady_clock::time_point startTime, const string& levelName) const {
        auto finishTime = chrono::steady_clock::now();
        auto totalSeconds =
            chrono::duration_cast<chrono::seconds>(finishTime - startTime).count();

        long long minutes = totalSeconds / 60;
        long long seconds = totalSeconds % 60;

        cout << "\n====================================\n";
        cout << "          CONGRATULATIONS!\n";
        cout << "       You solved the puzzle!\n";
        cout << "====================================\n";
        cout << "Difficulty  : " << levelName << "\n";
        cout << "Total moves : " << moves << "\n";
        cout << "Time taken  : " << minutes << " minute(s) "
             << seconds << " second(s)\n";
        cout << "====================================\n\n";
    }

public:
    PuzzleGame() : rng(random_device{}()) {
        createSolvedBoard();
    }

    void showInstructions() const {
        cout << "\n========== HOW TO PLAY ==========\n";
        cout << "1. The board contains tiles numbered 1 to 15.\n";
        cout << "2. One position is empty (shown as a blank space).\n";
        cout << "3. Enter the number of a tile beside the empty space.\n";
        cout << "4. The selected tile moves into the empty position.\n";
        cout << "5. Arrange the tiles from 1 to 15, left to right,\n";
        cout << "   top to bottom, with the empty space at the end.\n";
        cout << "6. Enter 0 during a game to return to the main menu.\n";
        cout << "=================================\n\n";
    }

    void play() {
        cout << "\n========== SELECT LEVEL ==========\n";
        cout << "1. Easy \n";
        cout << "2. Medium\n";
        cout << "3. Hard\n";
        cout << "0. Return to Main Menu\n";
        cout << "==================================\n";

        int levelChoice = readNumber("Choose difficulty: ");

        if (levelChoice == 0) {
            return;
        }

        int shuffleMoves;
        string levelName;

        switch (levelChoice) {
            case 1:
                shuffleMoves = 15;
                levelName = "Easy";
                break;
            case 2:
                shuffleMoves = 30;
                levelName = "Medium";
                break;
            case 3:
                shuffleMoves = 60;
                levelName = "Hard";
                break;
            default:
                cout << "Invalid level selection.\n";
                return;
        }

        shuffleBoard(shuffleMoves);

        auto startTime = chrono::steady_clock::now();

        cout << "\n" << levelName << " level started!\n";
        cout << "Arrange the numbers from 1 to 15.\n";
        cout << "Enter 0 to return to the main menu.\n";

        while (!isSolved()) {
            displayBoard();

            cout << "Level: " << levelName << " | Moves: " << moves << "\n";
            int tile = readNumber("Enter tile number to move (1-15, 0 = menu): ");

            if (tile == 0) {
                cout << "\nReturning to the main menu...\n";
                return;
            }

            if (moveTile(tile) && isSolved()) {
                displayBoard();
                showResult(startTime, levelName);
                return;
            }
        }
    }

    void run() {
        while (true) {
            cout << "\n========== MAIN MENU ==========\n";
            cout << "1. Start New Game\n";
            cout << "2. How to Play\n";
            cout << "3. Exit\n";
            cout << "===============================\n";

            int choice = readNumber("Enter your choice: ");

            switch (choice) {
                case 1:
                    play();
                    break;

                case 2:
                    showInstructions();
                    break;

                case 3:
                    cout << "\nThank you for playing Number Slide Puzzle!\n";
                    return;

                default:
                    cout << "Invalid choice. Please select 1, 2, or 3.\n";
            }
        }
    }
};

int main() {
    cout << "Name: Forhad Hossen | ID: 04326205101114 | Section: 4B-2\n";

    PuzzleGame game;
    game.run();

    return 0;
}

