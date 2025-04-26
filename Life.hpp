#ifndef Life_hpp
#define Life_hpp

#include <vector>
#include <iostream>
#include <cassert>
#include <type_traits>

using namespace std;

enum class CellKind { 
    Fredkin,
    Conway
};

// Forward declaration of Generation struct
struct Generation {
    CellKind kind_of_cell;
    bool alive;
    int age;

    Generation(CellKind k = CellKind::Fredkin, bool a = false, int age = 0)
        : kind_of_cell(k), alive(a), age(age) {}
};

// Abstract cell base class
class AbstractCell {
protected:
    bool _state;
    
public:
    AbstractCell(bool state = false) : _state(state) {}
    virtual ~AbstractCell() = default;
    
    virtual AbstractCell* clone() const = 0;
    virtual Generation evolve(int neighbors) = 0;
    virtual void print(ostream& os) const = 0;
    virtual bool affectsDirection(int dr, int dc) const = 0;
    virtual void next_Evolution(int& count) const = 0;
};

// Conway Cell implementation
class ConwayCell : public AbstractCell {
public:
    ConwayCell(bool state = false) : AbstractCell(state) {}
    
    AbstractCell* clone() const override {
        return new ConwayCell(*this);
    }
    
    Generation evolve(int neighbors) override {
        if (_state) {
            _state = (neighbors == 2 || neighbors == 3);
        } else {
            _state = (neighbors == 3);
        }
        return Generation(CellKind::Conway, _state);
    }
    
    void update(int neighbors) { 
        evolve(neighbors); 
    }
    
    void print(ostream& os) const override {
        os << (_state ? '*' : '.');
    }
    
    bool affectsDirection(int, int) const override {
        return true; // Conway cells affect all 8 neighbors
    }
    
    void next_Evolution(int& count) const override {
        if (_state) ++count;
    }
};

// Fredkin Cell implementation
class FredkinCell : public AbstractCell {
private:
    size_t _age = 0;
    
public:
    FredkinCell(bool state = false, size_t age = 0)
        : AbstractCell(state), _age(age) {}
    
    AbstractCell* clone() const override {
        return new FredkinCell(*this);
    }
    
    Generation evolve(int neighbors) override {
        bool was_alive = _state;
        
        if (_state) {
            _state = !(neighbors == 0 || neighbors == 2 || neighbors == 4);
        } else {
            _state = (neighbors == 1 || neighbors == 3);
        }
        
        // Increment age if the cell stayed alive
        _age += (was_alive && _state) ? 1 : 0;
        
        return Generation(CellKind::Fredkin, _state, static_cast<int>(_age));
    }
    
    void update(int neighbors) { 
        evolve(neighbors); 
    }
    
    void print(ostream& os) const override {
        if (_state) {
            if (_age >= 10) os << '+';
            else os << static_cast<char>('0' + _age);
        } else {
            os << '-';
        }
    }
    
    bool affectsDirection(int dr, int dc) const override {
        return (dr * dc) == 0; // Fredkin cells affect only cardinal directions
    }
    
    void next_Evolution(int& count) const override {
        if (_state) ++count;
    }
};

// Cell wrapper class
class Cell {
private:
    AbstractCell* _cell;
    
public:
    Cell() : _cell(new FredkinCell()) {}
    Cell(bool s) : _cell(new FredkinCell(s)) {}
    Cell(AbstractCell* ptr) : _cell(ptr) {}
    Cell(const Cell& other) : _cell(other._cell->clone()) {}
    ~Cell() { delete _cell; }
    
    Cell& operator=(const Cell& other) {
        if (this != &other) {
            AbstractCell* new_cell = other._cell->clone();
            delete _cell;
            _cell = new_cell;
        }
        return *this;
    }
    
    void update(int neighbor_count) {
        assert(_cell != nullptr);
        Generation state = _cell->evolve(neighbor_count);
        
        // Handle Cell-of-Life evolution (Fredkin -> Conway after age 2)
        if (state.kind_of_cell == CellKind::Fredkin && 
            state.alive && 
            state.age == 2) {
            delete _cell;
            _cell = new ConwayCell(true);
        }
    }
    
    // Interface methods
    void print(ostream& os) const { _cell->print(os); }
    bool affectsDirection(int dr, int dc) const { return _cell->affectsDirection(dr, dc); }
    void next_Evolution(int& count) const { _cell->next_Evolution(count); }
};

// Life template class
template <typename T>
class Life {
private:
    vector<vector<T>> _grid;
    size_t _rows;
    size_t _cols;
    int _generation = 0;
    
    vector<vector<int>> compute_neighbor_counts() const {
        assert(!_grid.empty() && !_grid[0].empty());
        
        vector<vector<int>> counts(_rows, vector<int>(_cols, 0));
        
        // Directions: for north, east, south, west, and diagonals
        const vector<pair<int, int>> directions = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},    // N, S, W, E
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}   // diagonals
        };
        
        // For each cell in the grid
        for (size_t r = 0; r < _rows; ++r) {
            for (size_t c = 0; c < _cols; ++c) {
                // Check if the cell is alive using next_Evolution
                int alive_check = 0;
                _grid[r][c].next_Evolution(alive_check);
                if (alive_check == 0) continue; // Skip dead cells
                
                for (const auto& [dr, dc] : directions) {
                    int nr = static_cast<int>(r) + dr;
                    int nc = static_cast<int>(c) + dc;
                    
                    // Check bounds
                    if (nr >= 0 && nr < static_cast<int>(_rows) &&
                        nc >= 0 && nc < static_cast<int>(_cols)) {
                        
                        // Check if the neighbor counts this direction
                        if (_grid[nr][nc].affectsDirection(-dr, -dc)) {
                            ++counts[nr][nc];
                        }
                    }
                }
            }
        }
        
        return counts;
    }
    
public:
    Life(size_t rows, size_t cols) 
        : _grid(rows, vector<T>(cols)), _rows(rows), _cols(cols) {}
    
   
    void replace_cell(int r, int c) {
        assert(r >= 0 && r < static_cast<int>(_rows));
        assert(c >= 0 && c < static_cast<int>(_cols));
        if constexpr (!std::is_same_v<T, Cell>) {
            _grid[r][c] = T(true);
        } else {
            _grid[r][c] = Cell(new FredkinCell(true));
        }        
    }

    
    void eval() {
        auto neighbors = compute_neighbor_counts();
        
        for (size_t r = 0; r < _rows; ++r) {
            size_t c = 0;
            for (auto& cell : _grid[r]) {
                cell.update(neighbors[r][c]);
                ++c;
            }
        }
        
        ++_generation;
    }
    

    void print_cell(size_t r, size_t c, ostream& os) const {
        assert(r < _rows && c < _cols);
        _grid[r][c].print(os);
    }
    
    int population() const {
        int total = 0;
        for (const auto& row : _grid) {
            for (const auto& cell : row) {
                int cell_alive = 0;
                cell.next_Evolution(cell_alive);
                total += cell_alive;
            }
        }
        return total;
    }
};

#endif // Life_hpp