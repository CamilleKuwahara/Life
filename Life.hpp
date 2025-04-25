#ifndef Life_hpp
#define Life_hpp

#include <vector>
#include <cstddef>
#include <iostream>

using namespace std;


enum class CellKind { 
    Fredkin,
    Conway
};

struct Generation {
    CellKind kind_of_cell;
    bool alive;
    int age;

    Generation(CellKind k = CellKind::Fredkin, bool a = false, int age = 0)
        : kind_of_cell(k), alive(a), age(age) {}
};

// AbstractCell base class
class AbstractCell {
protected:
    bool _state; // boolean whether the cell is alive

public:
    AbstractCell(bool state) : _state(state) {}
    virtual ~AbstractCell() = default;
    //Clone method for copying
    virtual AbstractCell* clone() const = 0;
    //Cell evolves between generations
    virtual Generation evolve(int neighbors) = 0;
    // Returns the current state of the cell
    virtual bool state() const = 0;
    //check if cell is alive
    virtual bool affectsDirection(int dr, int dc) const = 0;

    bool aliveState() const { return _state; } 
};

// ConwayCell
class ConwayCell : public AbstractCell {
public:
    ConwayCell(bool state = false) : AbstractCell(state) {}
    ConwayCell(bool state) : AbstractCell(state) {}
    //destructor
    ~ConwayCell() = default;
    //updates the state
    Generation evolve(int neighbors) override {
        if (_state) {
            _state = (neighbors == 2 || neighbors == 3);
        } else {
            _state = (neighbors == 3);
        }
        return Generation(CellKind::Conway, _state);
    }

    AbstractCell* clone() const override {
        return new ConwayCell(_state);
    }

    bool affectsDirection(int, int) const override {
        return true;
    }

    bool alive() const {
        return _state;
    }

};

// FredkinCell
class FredkinCell : public AbstractCell {
private:
    size_t _age; //Tracks the number of evolutiosn while alive

public:
    FredkinCell(bool state = false, size_t age = 0)
    : AbstractCell(state), _age(age) {}

        
    ~FredkinCell() = default;
    //increments age if alive
    Generation evolve(int neighbors) override {
        bool was_alive = _state;

        if (_state && (neighbors == 0 || neighbors == 2 || neighbors == 4)) {
            _state = false;
        } else if (!_state && (neighbors == 1 || neighbors == 3)) {
            _state = true;
        }

        if (was_alive && _state) {
            ++_age;
        }

        return Generation(CellKind::Fredkin, _state, static_cast<int>(_age));
    }


    AbstractCell* clone() const override {
        return new FredkinCell(*this);
    }
    bool affectsDirection(int dr, int dc) const override {
        return (dr * dc) == 0;
    }

};

// Cell wrapper to manage dynamic AbstractCe instances
class Cell {
private:
    AbstractCell* _pointer;

public:
     Cell() : _cell(new FredkinCell()) {}
    //deep copy
    Cell(bool state) : _cell(new FredkinCell(state)) {}
    Cell(AbstractCell* cell) : _pointer(cell->clone()) {}
    //copy constructor for uses clone
    Cell(const Cell& other) : _pointer(other._pointer->clone()) {}
    //move constructor
    Cell(Cell&& other) noexcept : _pointer(other._pointer) { other._pointer = nullptr; }

    Cell& operator=(const Cell& other) {
        if (this != &other) {
            AbstractCell* temp = other._cell->clone();
            delete _cell;
            _cell = temp;
        }
        return *this;
    }
    ~Cell() {
        delete _pointer;
    }

    bool alive() const { return _cell->aliveState(); } 

  
};

// Life class template
template<typename T>
    class Life {
    private:
        vector<vector<T>> _grid;
    
        vector<vector<int>> compute_neighbors() const {
            assert(!_grid.empty() && !_grid[0].empty());
            size_t rows = _grid.size();
            size_t cols = _grid[0].size();
            vector<vector<int>> counts(rows, vector<int>(cols, 0));
    
            const vector<pair<int, int>> directions = {
                {-1,0}, {1,0}, {0,-1}, {0,1},
                {-1,-1}, {-1,1}, {1,-1}, {1,1}
            };
    
            for (size_t r = 0; r < rows; ++r) {
                for (size_t c = 0; c < cols; ++c) {
                    if (!_grid[r][c].alive()) continue;
    
                    for (const auto& [dr, dc] : directions) {
                        int nr = static_cast<int>(r) + dr;
                        int nc = static_cast<int>(c) + dc;
    
                        if (nr >= 0 && nr < static_cast<int>(rows) && nc >= 0 && nc < static_cast<int>(cols)) {
                            if (_grid[nr][nc].affects(-dr, -dc)) {
                                ++counts[nr][nc];
                            }
                        }
                    }
                }
            }
            return counts;
        }
    
    public:
        Life(size_t rows, size_t cols) : _grid(rows, vector<T>(cols)) {}
    
        void eval() {
            auto neighbors = compute_neighbors();
            for (size_t r = 0; r < _grid.size(); ++r) {
                for (size_t c = 0; c < _grid[r].size(); ++c) {
                    _grid[r][c].update(neighbors[r][c]);
                }
            }
        }
    
        void add_alive(int r, int c) {
            assert(r >= 0 && r < static_cast<int>(_grid.size()));
            assert(c >= 0 && c < static_cast<int>(_grid[0].size()));
            _grid[r][c] = T(true);
        }
    
        int population() const {
            int total = 0;
            for (const auto& row : _grid) {
                for (const auto& cell : row) {
                    total += cell.alive();
                }
            }
            return total;
        }
    
        void print_board() const {
            for (const auto& row : _grid) {
                for (const auto& cell : row) {
                    cell.render(cout);
                }
                cout << '\n';
            }
        }
    };
    
#endif // Life_hpp