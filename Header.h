#pragma once
// Data Structures Final Project Fall 2023
// Miral Hasan 22i-8753
// Syed Musaab Sami 22i-2682
// Submitted to: Ma'am Maheen Arshad

#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <windows.h>
#include <cstring>
#include <string>
#include <fstream>
#include <iomanip>

#define MaxPlayers 10

using namespace std;

//Basic Class to use linked list in the program
template <typename T>
class LinkedList {
public:
    struct Node
    {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;


    LinkedList() : head(nullptr), tail(nullptr), size(0) {}


    void add(T data)
    {
        Node* newNode = new Node(data);
        if (head == nullptr) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }


    T remove() {
        if (isEmpty()) {
            throw std::runtime_error("Linked list is empty");
        }
        Node* temp = head;
        head = head->next;
        if (head == nullptr) {
            tail = nullptr;
        }
        T data = temp->data;
        delete temp;
        size--;
        return data;
    }


    bool isEmpty() const {
        return head == nullptr;
    }


    int getSize() const {
        return size;
    }

    ~LinkedList() {
        while (!isEmpty()) {
            remove();
        }
    }
};

//Separate Player Class for Leaderboard File Handling
class Player {
public:
    string name;
    int score;

    Player() :name(""), score(0)
    {

    }

    Player(const string& playerName, int playerScore)
        : name(playerName), score(playerScore) {}

};

//Basic Class to manage Queues in the program
template <typename T>
class Queue {
public:
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };

    Node* front;
    Node* rear;
    int size;


    Queue() : front(nullptr), rear(nullptr), size(0) {}


    void enqueue(T data) {
        Node* newNode = new Node(data);
        if (rear == nullptr) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }


    T dequeue() {
        if (isEmpty()) {
            throw runtime_error("Queue is empty");
        }
        Node* temp = front;
        front = front->next;
        if (front == nullptr) {
            rear = nullptr;
        }
        T data = temp->data;
        delete temp;
        size--;
        return data;
    }


    bool isEmpty() const {
        return front == nullptr;
    }


    int getSize() const {
        return size;
    }

    ~Queue() {
        while (!isEmpty()) {
            dequeue();
        }
    }
};

//class to manage obstacles
class Obstacle {
public:
    int x, y;
    char symbol;

    Obstacle(int posX, int posY, char sym) : x(posX), y(posY), symbol(sym) {}
};

//class to manage power ups
class Collectible {
public:
    int x, y;
    char symbol;

    Collectible(int posX, int posY, char sym) : x(posX), y(posY), symbol(sym) {}
};

//Our main class where the graph maze is generated and every functionality is added
class game {
private:
    struct Point {
        int x, y;
    };

    const int mapWidth;
    const int mapHeight;
    char* maze;
    int(*graph)[4];
    int playerX, playerY;
    bool gameOver;
    Point endPoint;
    int score;
    int maxObstacles;
    int totalObstaclesGenerated;
    Player currentPlayer;
    Player leaderboard[MaxPlayers];
    int numPlayers;


    const int maxLevels = 3;
    const char* mazeLayouts[3];

    int currentLevel;

    Queue<Obstacle> obstacleQueue;


    Queue<Collectible> trophyQueue;
    Queue<Collectible> coinQueue;
    LinkedList<Collectible> collectiblesList;

    void setConsoleColor(int colorCode) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
    }

    void gotoXY(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

public:
    game() : mapWidth(27), mapHeight(25),
        maze(nullptr),
        graph(new int[mapWidth * mapHeight][4]),
        playerX(0), playerY(0),
        gameOver(false),
        score(0), endPoint({ 0, 0 }), maxObstacles(5), totalObstaclesGenerated(0), numPlayers(0)

    {
        currentLevel = 0;
        maze = new char[mapWidth * mapHeight + 1];
        mazeLayouts[0] =
            "###########################"
            "#S                        #"
            "#                         #"
            "#  ###  #    ######  ###  #"
            "#                         #"
            "#  #  #  #        #  #    #"
            "#                         #"
            "#  #  ###  ####  #  #  #  #"
            "#                         #"
            "#      #  #      #  #  #  #"
            "#                         #"
            "#####  #  #  ####   #  #  #"
            "#                         #"
            "# #   #  #  #     #  #    #"
            "#                         #"
            "#  #  #  #  ######    #####"
            "#                         #"
            "#   #   #            #    #"
            "#                         #"
            "#    ############   #  #  #"
            "#                         #"
            "#                  #  #  ##"
            "###########################";


        mazeLayouts[1] =

            "###########################"
            "S   ##     ######        ##"
            "#                         #"
            "# ###  ##   ###### ## #  ##"
            "#       ##         ####   #"
            "#   ##     # ## ####   # ##"
            "# ##   ##          ##     #"
            "#  ##     ### #####   ## ##"
            "#        #     ####    #  #"
            "#     ##   ##    ##     ###"
            "#        ######  ##   #   #"
            "#   ##   #### ##  ##      #"
            "#   ##     ##        ##   #"
            "#   ### #   ##  ######    #"
            "#  #####          ##  #####"
            "# ##   ##     ##      #   #"
            "#  ## ## # ##    ## #     #"
            "#  ###    ## #####     ####"
            "#  #  ### ##    ##  #######"
            "#  ## ##   ########### ## #"
            "#                         #"
            "###########################";

        mazeLayouts[2] =

            "###########################"
            "S   #     ##      ##      #"
            "#   ##  ##     ##    ##   #"
            "# # ## ## ##      ##  ## ##"
            "#   ## ##   ## ##  ## ##  #"
            "#       ##     ##  ##     #"
            "#  ## ##   ##   ##     ## #"
            "#      ## ###    # # ##  ##"
            "# ### ##      ## ## ##    #"
            "# ##    # ##       ## ##  #"
            "#   ##     ##  #   ## ## ##"
            "# ##    ##     ## ##      #"
            "#                         #"
            "#  #  ##    ## ## ## ##   #"
            "# ##   ## # ##    ## ##   #"
            "#     ### ##  ###  ###   ##"
            "# ##   ##      # ##  ##   #"
            "#  ## ##   ## ##    ##    #"
            "# ## #   ##    ##   # ## ##"
            "#  #   # ## #     ##      #"
            "#  ## ##  ###  # # # # #  #"
            "#                  #  #  ##"
            "###########################";



        for (int i = 0; i < 3; ++i) {
            int posX = rand() % (mapWidth - 2) + 1;
            int posY = rand() % (mapHeight - 2) + 1;
            trophyQueue.enqueue(Collectible(posX, posY, 'T'));

            posX = rand() % (mapWidth - 2) + 1;
            posY = rand() % (mapHeight - 2) + 1;
            coinQueue.enqueue(Collectible(posX, posY, 'C'));
        }

    }
    int index(int x, int y) {
        return y * mapWidth + x;
    }

    void resetMaze() {
        const char* originalLayout = mazeLayouts[currentLevel];
        copy(originalLayout, originalLayout + mapWidth * mapHeight + 1, maze);
    }

    void buildGraph(char maze[]) {
        for (int y = 0; y < mapHeight; ++y) {
            for (int x = 0; x < mapWidth; ++x) {
                int currentNode = index(x, y);
                for (int i = 0; i < 4; ++i) {
                    graph[currentNode][i] = -1;
                }

                if (maze[currentNode] != '#') {
                    if (x > 0 && maze[index(x - 1, y)] != '#') {
                        graph[currentNode][0] = index(x - 1, y);
                    }
                    if (x < mapWidth - 1 && maze[index(x + 1, y)] != '#') {
                        graph[currentNode][1] = index(x + 1, y);
                    }
                    if (y > 0 && maze[index(x, y - 1)] != '#') {
                        graph[currentNode][2] = index(x, y - 1);
                    }
                    if (y < mapHeight - 1 && maze[index(x, y + 1)] != '#') {
                        graph[currentNode][3] = index(x, y + 1);
                    }
                }
            }
        }
    }




    void generateObstacle() {
        if (totalObstaclesGenerated < maxObstacles) {
            int posX, posY;
            do {
                posX = rand() % (mapWidth - 2) + 30;
                posY = rand() % (mapHeight - 2) + 4;



            } while (maze[index(posX, posY)] != ' ');

            obstacleQueue.enqueue(Obstacle(posX, posY, 'O'));
            totalObstaclesGenerated++;
        }
    }

    bool obstaclesCollision(int playerX, int playerY, const Obstacle& ob) {
        return (playerX == ob.x && playerY == ob.y);
    }



    void InitializeGame() {
        gameOver = false;
        score = 1;


        playerX = 1;
        playerY = 1;


        resetMaze();


        if (currentLevel < maxLevels) {

            do {
                endPoint.x = rand() % (mapWidth - 2) + 1;
                endPoint.y = rand() % (mapHeight - 2) + 1;
            } while (maze[index(endPoint.x, endPoint.y)] != ' ');


            maze[index(endPoint.x, endPoint.y)] = 'E';



            buildGraph(maze);


            while (!trophyQueue.isEmpty()) {
                trophyQueue.dequeue();
            }
            while (!coinQueue.isEmpty()) {
                coinQueue.dequeue();
            }


            for (int i = 0; i < 3; ++i) {
                int posX, posY;
                do {
                    posX = rand() % (mapWidth - 2) + 1;
                    posY = rand() % (mapHeight - 2) + 1;
                } while (maze[index(posX, posY)] != ' ');

                trophyQueue.enqueue(Collectible(posX, posY, 'T'));
            }

            for (int i = 0; i < 3; ++i) {
                int posX, posY;
                do {
                    posX = rand() % (mapWidth - 2) + 1;
                    posY = rand() % (mapHeight - 2) + 1;
                } while (maze[index(posX, posY)] != ' ');

                coinQueue.enqueue(Collectible(posX, posY, 'C'));
            }
        }
    }


    void Input() {
        int newX = playerX, newY = playerY;

        if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            newX--;
        }
        else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        {
           newX++;
        }
        else if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            newY--;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            newY++;
        }


        if ((newX >= 0 && newX < mapWidth && newY >= 0 && newY < mapHeight) &&
            (maze[newY * mapWidth + newX] == ' ' || maze[newY * mapWidth + newX] == 'E')) {
            playerX = newX;
            playerY = newY;
        }
    }

    void UpdateGame() {

        if (playerX == endPoint.x && playerY == endPoint.y) {
            gameOver = true;
            cout << "Congratulations! You reached the end!" << endl;
            return;
        }


        int obstacleQueueSize = obstacleQueue.getSize();
        for (int i = 0; i < obstacleQueueSize; ++i) {
            Obstacle ob = obstacleQueue.dequeue();
            if (obstaclesCollision(playerX, playerY, ob)) {

                score = max(1, score - 5);
                cout << "You hit an obstacle! Score: " << score << endl;
            }
            obstacleQueue.enqueue(ob);
        }


        if (playerX < 0) playerX = 0;
        if (playerX >= mapWidth) playerX = mapWidth - 1;
        if (playerY < 0) playerY = 0;
        if (playerY >= mapHeight) playerY = mapHeight - 1;
    }



    void RenderGame() {
        static bool firstRender = true;
        static int prevPlayerX = playerX;
        static int prevPlayerY = playerY;


        int startX = max(0, (80 - mapWidth) / 2);
        int startY = max(0, (25 - mapHeight) / 2 + 4);

        for (int y = 0; y < mapHeight; ++y) {
            gotoXY(startX, startY + y);
            for (int x = 0; x < mapWidth; ++x) {
                setConsoleColor(8);
                cout << maze[y * mapWidth + x];
            }
        }

        Queue<Obstacle> tempObstacleQueue;
        while (!obstacleQueue.isEmpty()) {
            Obstacle ob = obstacleQueue.dequeue();

            gotoXY(ob.x, ob.y);
            setConsoleColor(14);
            cout << ob.symbol;

            tempObstacleQueue.enqueue(ob);
        }


        while (!tempObstacleQueue.isEmpty()) {
            obstacleQueue.enqueue(tempObstacleQueue.dequeue());
        }


        Queue<Collectible> tempCoinQueue;
        int coinQueueSize = coinQueue.getSize();
        for (int i = 0; i < coinQueueSize; ++i) {
            Collectible coin = coinQueue.dequeue();
            gotoXY(coin.x + startX, coin.y + startY);
            setConsoleColor(10);
            cout << coin.symbol;


            tempCoinQueue.enqueue(coin);
        }


        while (!tempCoinQueue.isEmpty()) {
            coinQueue.enqueue(tempCoinQueue.dequeue());
        }


        Queue<Collectible> tempTrophyQueue;
        int trophyQueueSize = trophyQueue.getSize();
        for (int i = 0; i < trophyQueueSize; ++i) {
            Collectible trophy = trophyQueue.dequeue();
            gotoXY(trophy.x + startX, trophy.y + startY);
            setConsoleColor(21);
            cout << trophy.symbol;


            tempTrophyQueue.enqueue(trophy);
        }


        while (!tempTrophyQueue.isEmpty()) {
            trophyQueue.enqueue(tempTrophyQueue.dequeue());
        }


        gotoXY(playerX + startX, playerY + startY);
        setConsoleColor(13);
        cout << 'P';


        prevPlayerX = playerX;
        prevPlayerY = playerY;


        setConsoleColor(7);


        gotoXY(0, startY + mapHeight);
        cout << "Score: " << score << "         ";
    }


    void processCollectibles() {

        int trophyQueueSize = trophyQueue.getSize();
        for (int i = 0; i < trophyQueueSize; ++i) {
            Collectible trophy = trophyQueue.dequeue();
            if (trophiesCollision(playerX, playerY, trophy)) {
                collectiblesList.add(trophy);
                score *= 2;
            }
            else {
                trophyQueue.enqueue(trophy);
            }
        }


        int coinQueueSize = coinQueue.getSize();
        for (int i = 0; i < coinQueueSize; ++i) {
            Collectible coin = coinQueue.dequeue();
            if (coinsCollision(playerX, playerY, coin)) {
                collectiblesList.add(coin);
                score *= 2;
            }
            else {
                coinQueue.enqueue(coin);
            }
        }


        int obstacleQueueSize = obstacleQueue.getSize();
        for (int i = 0; i < obstacleQueueSize; ++i) {
            Obstacle ob = obstacleQueue.dequeue();
            if (obstaclesCollision(playerX, playerY, ob)) {
                score = max(1, score - 5);
            }
            else {
                obstacleQueue.enqueue(ob);
            }
        }
    }




    bool trophiesCollision(int playerX, int playerY, const Collectible& trophy) {
        return (playerX == trophy.x && playerY == trophy.y);
    }

    bool coinsCollision(int playerX, int playerY, const Collectible& coin) {
        return (playerX == coin.x && playerY == coin.y);
    }

    void displayLevelInfo() {
        system("cls");
        cout << "Level " << currentLevel + 1 << endl;
        cout << "Avoid obstacles ('O') and reach the end ('E')." << endl;
        cout << "Controls: Use arrow keys to move (Up, Down, Left, Right)." << endl;
        cout << endl;
    }

    void displayIntroduction() {
        cout << endl << endl << endl << endl << endl;
        cout << "\t\t\t\t\t      ----------------------------" << endl;
        cout << "\t\t\t\t\t\tWelcome to the Maze Game!" << endl;
        cout << endl;
        cout << "\t\t\t\tInstructions: " << endl;
        cout << "\t\t\t\tNavigate through the maze, avoid obstacles ('O'), and reach the end ('E')." << endl;
        cout << endl;
        cout << "\t\t\t\tControls:" << endl;
        cout << "\t\t\t\tUse arrow keys to move:" << endl;
        cout << "\t\t\t\tUp, Down, Left, Right" << endl;
        cout << endl;
        cout << "\t\t\t\tTry to qualify for the next level by reaching the end." << endl;
        cout << "\t\t\t\t\t\t\tGood luck!" << endl << endl;
        cout << "\t\t\t\t\t      ----------------------------" << endl;
        system("pause");
    }

    void loadPreviousScores() {
        ifstream leaderboardFile("leaderboard.txt", std::ios::in);
        if (leaderboardFile.is_open()) {
            numPlayers = 0;
            while (numPlayers < MaxPlayers && leaderboardFile >> leaderboard[numPlayers].name >> leaderboard[numPlayers].score) {
                numPlayers++;
            }
            leaderboardFile.close();
        }
    }
    void displayLeaderboard() {
        system("cls");
        cout << "Leaderboard:\n";
        for (int i = 0; i < numPlayers; ++i) {
            cout << "Name: " << leaderboard[i].name << "\tScore: " << leaderboard[i].score << endl;
        }
        system("pause");
    }

    void saveLeaderboardToFile() {
        ofstream leaderboardFile("leaderboard.txt", std::ios::out);
        if (leaderboardFile.is_open()) {
            for (int i = 0; i < numPlayers; ++i) {
                leaderboardFile << leaderboard[i].name << " " << leaderboard[i].score << "\n";
            }
            leaderboardFile.close();
        }
    }

    void Run() {

        int startX = max(0, (80 - mapWidth) / 2);
        int startY = max(0, (25 - mapHeight) / 2);

        int obstacleGenerationCounter = 0;
        srand(static_cast<unsigned int>(time(nullptr)));

        displayIntroduction();
        do {
            InitializeGame();
            gameOver = false;

            if (currentLevel < maxLevels) {
                displayLevelInfo();
            }

            while (!gameOver) {
                generateObstacle();

                RenderGame();
                Input();
                UpdateGame();
                this_thread::sleep_for(chrono::milliseconds(200));
                processCollectibles();
            }

            if (currentLevel < maxLevels && maze[index(playerX, playerY)] == 'E') {
                cout << "Level " << currentLevel + 1 << " Complete! Your final score: " << score << endl;


                if (numPlayers < MaxPlayers) {
                    cout << "Enter your name: ";
                    cin >> currentPlayer.name;
                    currentPlayer.score = score;
                    leaderboard[numPlayers++] = currentPlayer;
                }
                else {
                    cout << "Leaderboard is full. Your score will not be added." << endl;
                }

                cout << "Congratulations! You reached the end." << endl;


                if (currentLevel < maxLevels - 1) {
                    cout << "Do you want to play the next level? (y/n): ";
                    char response;
                    cin >> response;

                    if (response == 'n' || response == 'N') {
                        break;
                    }
                }

                system("pause");
                system("cls");

                currentLevel++;
            }
            else if (gameOver && maze[index(playerX, playerY)] == 'O') {
                cout << "Game over! You did not qualify for the next level." << endl;
                break;
            }

        } while (currentLevel < maxLevels);


        system("cls");
        cout << "Congratulations! You have completed all three levels." << endl;
        displayLeaderboard();
        cout << "Press anywhere to quit..." << endl;
        system("pause");


        saveLeaderboardToFile();
    }
    ~game()
    {
        delete[] maze;
        delete[] graph;
    }
};