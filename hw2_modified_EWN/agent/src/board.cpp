#include "board.hpp"

ostream& operator<<(ostream& os, Cube cube) {
    switch (cube.color) {
        case Color::Red:
            return (os << "R" << cube.num);
        case Color::Blue:
            return (os << "B" << cube.num);
        case Color::Empty:
            return (os << "_"
                       << "_");
    }
}

void Cube::flipColor() {
    if (this->color == Color::Red)
        this->color = Color::Blue;
    else if (this->color == Color::Blue)
        this->color = Color::Red;
}

map<pair<char, Direction>, Ply> Ply::initPlyInstances() {
    map<pair<char, Direction>, Ply> instances;
    Direction directions[] = {Direction::Horizontal, Direction::Vertical,
                              Direction::Diagonal};
    for (int i = 0; i < 6; i++) {
        for (Direction& dir : directions) {
            instances.insert(make_pair(make_pair('0' + i, dir), Ply('0' + i, dir)));
        }
    }
    return instances;
}

const map<pair<char, Direction>, Ply> Ply::plyInstances = Ply::initPlyInstances();

const Ply& Ply::getPly(char num, Direction dir) {
    return plyInstances.at(make_pair(num, dir));
}

ostream& operator<<(ostream& os, Ply ply) {
    switch (ply.dir) {
        case Direction::Horizontal:
            return (os << ply.num << " horizontal");
        case Direction::Vertical:
            return (os << ply.num << " vertical");
        case Direction::Diagonal:
            return (os << ply.num << " diagonal");
    }
}

pair<int, int> Board::getCubePos(const Cube& cube) const {
    int r = -1, c = -1;
    if (cube.color == Color::Red) {
        r = this->redCubeRow[cube.num - '0'];
        c = this->redCubeCol[cube.num - '0'];
    } else if (cube.color == Color::Blue) {
        r = this->blueCubeRow[cube.num - '0'];
        c = this->blueCubeCol[cube.num - '0'];
    }
    return make_pair(r, c);
}

void Board::setCube(int r, int c, Cube cube) {
    this->cubes[r][c] = cube;
    if (cube.color == Color::Red) {
        this->redCubeRow[cube.num - '0'] = r;
        this->redCubeCol[cube.num - '0'] = c;
    } else if (cube.color == Color::Blue) {
        this->blueCubeRow[cube.num - '0'] = r;
        this->blueCubeCol[cube.num - '0'] = c;
    }
}

void Board::swap(int r1, int c1, int r2, int c2) {
    Cube temp = cubes[r1][c1];
    this->setCube(r1, c1, this->cubes[r2][c2]);
    this->setCube(r2, c2, temp);
}

void Board::flip() {
    // horizontal flip
    for (int r = 0; r < N_ROW; r++) {
        for (int c = 0; c < N_COL / 2; c++) {
            this->swap(r, c, r, N_COL - c - 1);
        }
    }
    // vertical flip
    for (int c = 0; c < N_COL; c++) {
        for (int r = 0; r < N_ROW / 2; r++) {
            this->swap(r, c, N_ROW - 1 - r, c);
        }
    }
    // exchange colors
    for (int r = 0; r < N_ROW; r++) {
        for (int c = 0; c < N_COL; c++) {
            Cube& cube = this->cubes[r][c];
            if (cube.color == Color::Blue) {
                this->redCubeRow[cube.num - '0'] = r;
                this->redCubeCol[cube.num - '0'] = c;
            } else if (cube.color == Color::Red) {
                this->blueCubeRow[cube.num - '0'] = r;
                this->blueCubeCol[cube.num - '0'] = c;
            }
            cube.flipColor();
        }
    }
}

Color Board::getWinner() const {
    // both colors have reached the corner
    Cube upperLeft = this->cubes.front().front();
    Cube lowerRight = this->cubes.back().back();
    if (upperLeft.color == Color::Blue && lowerRight.color == Color::Red) {
        if (upperLeft.num < lowerRight.num)
            return Color::Blue;
        else if (upperLeft.num > lowerRight.num)
            return Color::Red;
        return Color::Empty;
    }

    // all red/blue cubes are captured
    unsigned redCnt = 0, blueCnt = 0;
    for (const array<Cube, N_COL>& row : this->cubes) {
        for (const Cube& c : row) {
            Color color = c.color;
            if (color == Color::Red)
                redCnt += 1;
            else if (color == Color::Blue)
                blueCnt += 1;

            if (redCnt > 0 && blueCnt > 0) return Color::Empty;
        }
    }
    return (redCnt == 0 ? Color::Blue : Color::Red);
}

vector<Ply>* Board::getNextPly() const {
    vector<Ply>* validPlys = new vector<Ply>();
    validPlys->reserve(N_NEXT);
    for (int r = 0; r < N_ROW; r++) {
        for (int c = 0; c < N_COL; c++) {
            Cube cube = this->cubes[r][c];
            if (cube.color != Color::Red) continue;
            // horizontal: (r, c) -> (r, c + 1)
            if ((c + 1 < N_COL) && (this->cubes[r][c + 1].color != Color::Red)) {
                validPlys->push_back(Ply::getPly(cube.num, Direction::Horizontal));
            }
            // vertical: (r, c) -> (r + 1, c)
            if ((r + 1 < N_ROW) && (this->cubes[r + 1][c].color != Color::Red)) {
                validPlys->push_back(Ply::getPly(cube.num, Direction::Vertical));
            }
            // diagonal: (r, c) -> (r + 1, c + 1)
            if ((r + 1 < N_ROW && c + 1 < N_COL) &&
                this->cubes[r + 1][c + 1].color != Color::Red) {
                validPlys->push_back(Ply::getPly(cube.num, Direction::Diagonal));
            }
        }
    }
    return validPlys;
}

void Board::playRand() {}

ostream& operator<<(ostream& os, Board board) {
    // board
    for (int r = 0; r < N_ROW; r++) {
        for (int c = 0; c < N_COL; c++) {
            os << setw(4) << board.get(r, c);
        }
        os << endl;
    }

    // cube positions
    Color colors[2] = {Color::Red, Color::Blue};
    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < 6; i++) {
            pair<int, int> cubePos = board.getCubePos(Cube(colors[c], '0' + i));
            os << "(" << cubePos.first << ", " << cubePos.second << ") ";
        }
        os << endl;
    }
    return os;
}