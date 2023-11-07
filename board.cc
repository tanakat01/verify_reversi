#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <set>
#include <map>
#include <deque>
class Board {
public:
    uint64_t black;
    uint64_t white;  
    explicit Board(uint64_t black_, uint64_t white_) :black(black_), white(white_) {}
    explicit Board(std::string const& obf) :black(0), white(0) {
        for (size_t y = 0; y < 8; y++) {
            for (size_t x = 0; x < 8; x++) {
                uint64_t i = y * 8 + x;
                if (obf[i] == 'O') {
                    white |= (1LL << i);
                } else if (obf[i] == 'X') {
                    black |= (1LL << i);
                }
            }
        }
    }
    int get(int x, int y) const {
        int i = y * 8 + x;
        return ((black >> i) & 1) - ((white >> i) & 1);
    }
    bool can_flip(int x, int y, int dx, int dy) const;
    bool is_valid(int x, int y) const;
    std::vector<int> gen_moves() const;
    Board apply_move(int x, int y) const;
    Board rotate90() const {
        uint64_t black1 = 0LL, white1 = 0LL;
        for (size_t y = 0; y < 8; y++) {
            for (size_t x = 0; x < 8; x++) {
                uint64_t i = y * 8 + x;
                uint64_t i0 = (7 - x) * 8 + y;
                // std::cerr << "i=" << i << "i0=" << i0 << ",black=" << ((black >> i0) & 1LL) << ",white=" << ((white >> i0) & 1LL)  << std::endl;
                black1 |= ((black >> i) & 1LL) << i0;
                white1 |= ((white >> i) & 1LL) << i0;
            }
        }
        return Board(black1, white1);
    }
    Board hflip() const {
        uint64_t black1 = 0LL, white1 = 0LL;
        for (size_t y = 0; y < 8; y++) {
            for (size_t x = 0; x < 8; x++) {
                uint64_t i = y * 8 + x;
                uint64_t i0 = y * 8 + (7 - x);
                // std::cerr << "i=" << i << "i0=" << i0 << ",black=" << ((black >> i0) & 1LL) << ",white=" << ((white >> i0) & 1LL)  << std::endl;
                black1 |= ((black >> i) & 1LL) << i0;
                white1 |= ((white >> i) & 1LL) << i0;
            }
        }
        return Board(black1, white1);
    }
    Board flip_turn() const {
        return Board(white, black).normalize();
    }
    Board normalize() const;
};

bool operator<(Board const& b1, Board const& b2) {
    if (b1.black == b2.black) {
        return b1.white < b2.white;
    }
    return b1.black < b2.black;
}

bool operator==(Board const& b1, Board const& b2) {
    return b1.black == b2.black && b1.white == b2.white;
}

bool operator!=(Board const& b1, Board const& b2) {
    return b1.black != b2.black || b1.white != b2.white;
}



bool Board::can_flip(int x, int y, int dx, int dy) const {
    int x1 = x + dx, y1 = y + dy;
    if (x1 < 0 || 8 <= x1 || y1 < 0 || 8 <= y1 || get(x1, y1) != -1) {
        return false;
    }
    x1 += dx;
    y1 += dy;
    while (0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8 && get(x1, y1) == -1) {
        x1 += dx;
        y1 += dy;
    }
    return 0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8 && get(x1, y1) == 1;
}
bool Board::is_valid(int x, int y) const {
    int i = y * 8 + x;
    if ((black | white) & (1LL << i)) {
        return false;
    }
    for (int dy = -1; dy < 2; dy++) {
        for (int dx = -1; dx < 2; dx++) {
            if (dx == 0 && dy == 0) continue;
            if (can_flip(x, y, dx, dy)) return true;
        }
    }
    return false;
}

Board Board::apply_move(int x, int y) const {
    uint64_t black1 = black, white1 = white;
    black1 |= (1LL << (y * 8 + x));

    assert(is_valid(x, y));
    for (int dy = -1; dy < 2; dy++) {
        for (int dx = -1; dx < 2; dx++) {
            if (dx == 0 && dy == 0) continue;
            int x1 = x + dx, y1 = y + dy;
            if (x1 < 0 || 8 <= x1 || y1 < 0 || 8 <= y1 || get(x1, y1) != -1) {
                continue;
            }
            std::vector<int> fs(1, y1 * 8 + x1);
            x1 += dx;
            y1 += dy;
            while (0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8 && get(x1, y1) == -1) {
                fs.push_back(y1 * 8 + x1);
                x1 += dx;
                y1 += dy;
            }
            if (0 <= x1 && x1 < 8 && 0 <= y1 && y1 < 8 && get(x1, y1) == 1) {
                for (auto i: fs) {
                    white1 &= ~(1LL << i);
                    black1 |= (1LL << i);
                }
            }
        }
    }
    return Board(black1, white1);
}
std::vector<int> Board::gen_moves() const {
    std::vector<int> ans;
    for (int i = 0; i < 64; i++) {
        if (is_valid(i & 7, i >> 3)) ans.push_back(i);
    }
    return ans;
}

Board Board::normalize() const {
    Board b(*this);
    Board minb(*this);
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 4; j++) {
            b = b.rotate90();
            if (b < minb) {
                minb = b;
            }
        }
        b = b.hflip();
    }
    return minb;
}

std::ostream& operator<<(std::ostream& os, Board const& b) {
    for (size_t i = 0; i < 64; i++) {
        if ((b.black & (1LL << i)) != 0) {
            os << 'X';
        } else if ((b.white & (1LL << i)) != 0) {
            os << 'O';
        } else {
            os << '-';
        }
    }
    return os << " X;";
}

std::ostream& pp(std::ostream& os, Board const& b, int hx = -1, int hy = -1) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            size_t i = y * 8 + x;
            if (x == hx && y == hy) {
                os << '*';
            }else if ((b.black & (1LL << i)) != 0) {
                os << 'X';
            } else if ((b.white & (1LL << i)) != 0) {
                os << 'O';
            } else {
                os << '-';
            }
        }
        os << std::endl;
    }
    return os;
}

void test_normalize() {
    std::ifstream ifs("empty50_tasklist_edax_knowledge.csv");
    std::string line;
    size_t i = 0;
    while (std::getline(ifs, line)) {
        if (++i != 1) {
            Board b(line);
            if (b.normalize() != b) {
                std::cout << "####" << std::endl;
                pp(std::cout, b);
                pp(std::cout, b.normalize());
                std::cout << line << std::endl;
                std::cout << b << std::endl;
                std::cout << b.normalize() << std::endl;
            }
        }
    }
}

void test_genmove() {
    Board b("------------O------OOX-----XOX----XXOO----XO-O------------------ X;");
    std::vector<int> moves = b.gen_moves();
    for (auto i : moves) {
        std::cout << "#####" << std::endl;
        pp(std::cout, b, i % 8, i / 8);
        Board b1 = b.apply_move(i % 8, i / 8);
        pp(std::cout, b1);
        Board b2 = b1.flip_turn();
        pp(std::cout, b2);
    }
}

int popcount(uint64_t v) {
    int ans = 0;
    while (v != 0) {
        ans += 1;
        v &= (v - 1);
    }
    return ans;
}

int calc_value(Board const& b) {
    return popcount(b.black) - popcount(b.white);
}

void test_board_ply_10() {
    std::vector<std::vector<Board> > bs(11);
    bs[0].push_back(Board("---------------------------OX------XO--------------------------- X"));
    for (size_t i = 0; i < 10; i++) {
        std::set<Board> visited;
        for (auto &b: bs[i]) {
            std::vector<int> moves = b.gen_moves();
            if (moves.size() == 0) {
                Board b1 = b.flip_turn();
                std::vector<int> moves1 = b1.gen_moves();
                if (moves1.size() == 0) {
                    std::cout << "both nomove" << std::endl;
                }
                pp(std::cout, b);
                for (int j: moves1) {
                    Board b2 = b1.apply_move(j % 8, j / 8).flip_turn().normalize();
                    if (visited.find(b2) == visited.end()) {
                        visited.insert(b2);
                        bs[i + 1].push_back(b2);
                    }
                }

            } else {
                for (int j: b.gen_moves()) {
                    Board b1 = b.apply_move(j % 8, j / 8).flip_turn().normalize();
                    if (visited.find(b1) == visited.end()) {
                        visited.insert(b1);
                        bs[i + 1].push_back(b1);
                    }
                }
            }
        }
        std::cerr << "bs[" << (i + 1) << "].size() = " << bs[i + 1].size() << std::endl;
    }
    std::set<Board> visited;
    for (auto &b: bs[10]) {
        std::vector<int> moves = b.gen_moves();
        if (moves.size() == 0) {
            Board b1 = b.flip_turn();
            if (visited.find(b1) == visited.end()) {
                std::vector<int> moves1 = b1.gen_moves();
                if (moves1.size() == 0) {
                    std::cout << "both nomove" << std::endl;
                } else {
                    visited.insert(b1);
                }
            }
        } else {
            visited.insert(b);
        }
    }
    std::cout << "visited.size() = " << visited.size() << std::endl;
//    test_genmove();
}

class Table {
    public:
    std::map<Board, int> b2i;
    std::vector<Board> boards;
    std::vector<int> value0; // can achieve points more than or equal to zero
    std::vector<int> value1; // can achieve points more than zero
    std::vector<std::vector<int> > nexts;
    std::vector<std::vector<int> > prevs;
    Table() {}
    int to_index(Board const& b) {
        if (b2i.find(b) != b2i.end()) {
            return b2i[b];
        }
        int i = boards.size();
        b2i[b] = i;
        boards.push_back(b);
        value0.push_back(0);
        value1.push_back(0);
        nexts.push_back(std::vector<int>());
        prevs.push_back(std::vector<int>());
        return i;
    }
};

void test_numbering_ply_10() {
    std::vector<std::vector<Board> > bs(11);
    Table t;
    Board init_b = Board("---------------------------OX------XO--------------------------- X").normalize();
    bs[0].push_back(init_b);
    for (size_t i = 0; i < 10; i++) {
        std::cerr << "i=" << i << std::endl;
        std::set<Board> visited;
        for (auto &b: bs[i]) {
            int j = t.to_index(b);
            std::vector<int> moves = b.gen_moves();
            if (moves.size() == 0) {
                Board b1 = b.flip_turn();
                int j1 = t.to_index(b1);
                std::vector<int> moves1 = b1.gen_moves();
                if (moves1.size() == 0) {
                    int v = calc_value(b);
                    t.value0[j] = (v >= 0 ? 1 : -1);
                    t.value1[j] = (v > 0 ? 1: -1);
                } else {
                    t.nexts[j].push_back(j1);
                    t.prevs[j1].push_back(j);
                    for (int m1: moves1) {
                        Board b2 = b1.apply_move(m1 % 8, m1 / 8).flip_turn().normalize();

                        if (visited.find(b2) == visited.end()) {
                            visited.insert(b2);
                            bs[i + 1].push_back(b2);
                        }
                        int j2 = t.to_index(b2);
                        t.nexts[j1].push_back(j2);
                        t.prevs[j2].push_back(j1);
                    }
                }
            } else {
                for (int m: b.gen_moves()) {
                    Board b1 = b.apply_move(m % 8, m / 8).flip_turn().normalize();
                    if (visited.find(b1) == visited.end()) {
                        visited.insert(b1);
                        bs[i + 1].push_back(b1);
                    }
                    int j1 = t.to_index(b1);
                    t.nexts[j].push_back(j1);
                    t.prevs[j1].push_back(j);
                }
            }
        }
    }
    for (auto &b: bs[10]) {
        int j = t.to_index(b);
        std::vector<int> moves = b.gen_moves();
        if (moves.size() == 0) {
            Board b1 = b.flip_turn();
            int j1 = t.to_index(b1);
            std::vector<int> moves1 = b1.gen_moves();
            if (moves1.size() == 0) {
                int v = calc_value(b);
                t.value0[j] = (v >= 0 ? 1 : -1);
                t.value1[j] = (v > 0 ? 1: -1);
            } else {
                t.nexts[j].push_back(j1);
                t.prevs[j1].push_back(j);
            }
        }
    }
    std::cout << "board.size()=" << t.boards.size() << std::endl;
    std::ifstream ifs("empty50_tasklist_edax_knowledge.csv");
    std::string line;
    int lno = 0;
    while (std::getline(ifs, line)) {
        if (++lno == 1) continue;
        std::vector<std::string> cs;
        size_t last = 0;
        size_t found = line.find(',');
        while (found != std::string::npos) {
            cs.push_back(std::string(line.begin() + last, line.begin() + found));
            last = found + 1;
            found = line.find(',', last);
        }
        cs.push_back(std::string(line.begin() + last, line.end()));
        int j = t.to_index(Board(cs[0]).normalize());
        int v = atoi(cs[2].c_str());
        t.value0[j] = (v >= 0 ? 1 : -1);
        t.value1[j] = (v > 0 ? 1: -1);
    }
    size_t n = t.value0.size();
    std::vector<size_t> ocount0(n);
    std::vector<size_t> ocount1(n);
    std::deque<size_t> q0;
    std::deque<size_t> q1;
    for (size_t i = 0; i < n; i++) {
        ocount0[i] = ocount1[i] = t.nexts[i].size();
        if (t.value0[i] != 0) {
            q0.push_back(i);
            ocount0[i] = 0;
        }
        if (t.value1[i] != 0) {
            q1.push_back(i);
            ocount1[i] = 0;
        }
    }
    while (!q0.empty() || !q1.empty()) {
        if (!q0.empty()) {
            size_t i = q0.front(); q0.pop_front();
            assert(t.value0[i] != 0);
            if (t.value0[i] > 0) {
                for (size_t j: t.prevs[i]) {
                    if (ocount1[j] > 0) {
                        if (--ocount1[j] == 0) {
                            t.value1[j] = -1;
                            q1.push_back(j);
                        }
                    }
                }
            } else {
                for (size_t j: t.prevs[i]) {
                    if (ocount1[j] > 0) {
                        ocount1[j] = 0;
                        t.value1[j] = 1;
                        q1.push_back(j);
                    }
                }               
            }
        }
        if (!q1.empty()) {
            size_t i = q1.front(); q1.pop_front();
            assert(t.value1[i] != 0);
            if (t.value1[i] > 0) {
                for (size_t j: t.prevs[i]) {
                    if (ocount0[j] > 0) {
                        if (--ocount0[j] == 0) {
                            t.value0[j] = -1;
                            q0.push_back(j);
                        }
                    }
                }
            } else {
                for (size_t j: t.prevs[i]) {
                    if (ocount0[j] > 0) {
                        ocount0[j] = 0;
                        t.value0[j] = 1;
                        q0.push_back(j);
                    }
                }               
            }
        }
    }
    int init_i = t.to_index(init_b);
    std::cout << t.value0[init_i] << "," << t.value1[init_i] << std::endl;
//    test_genmove();
}

int main() {
//    test_normalize();
//    test_board_ply_10();
    test_numbering_ply_10();
}