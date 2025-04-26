#ifndef Life_hpp
#define Life_hpp

#include <vector>
#include <iostream>
#include <cassert>
#include <type_traits>

using namespace std;

// Types of Cells: Fredkin or Conway
enum class CellKind { 
    Fredkin,
    Conway
};

// Struct to hold the next generation state for a cell
struct Generation {
    CellKind kind_of_cell;
    bool alive;
    int age;

    Generation(CellKind k = CellKind::Fredkin, bool a = false, int age = 0)
        : kind_of_cell(k), alive(a), age(age) {}
};

// Abstract base class for a Cell
class AbstractCell {
protected:
    bool _state; //boolean if cell is dead or alive

public:
    AbstractCell(bool state = false) : _state(state) {}
    virtual ~AbstractCell() = default;
    // Create a copy of the cell
    virtual AbstractCell* clone() const = 0;
    // Evolve the cell based on neighbor count
    virtual Generation evolve(int neighbors) = 0;
    // Print the cell's state
    virtual void print(ostream& os) const = 0;
    // Check if the cell affects a neighbor in a given direction
    virtual bool affectsDirection(int dr, int dc) const = 0;
    // Count if the cell is alive for population purposes
    virtual void next_Evolution(int& count) const = 0;
};

// Concrete class for Conway cells
class ConwayCell : public AbstractCell {
public:
    ConwayCell(bool state = false) : AbstractCell(state) {}

    // Make a copy of this cell
    AbstractCell* clone() const override {
        return new ConwayCell(*this);
    }

    // Update Conway cell based on neighbor rules
    Generation evolve(int neighbors) override {
        //check if cell is alive or dead
        if (_state) { 
            // Alive cell survives with 2 or 3 neighbors
            _state = (neighbors == 2 || neighbors == 3);
        } else {
            // Dead cell comes alive with exactly 3 neighbors
            _state = (neighbors == 3);
        }
        return Generation(CellKind::Conway, _state);
    }

    // Update wrapper for evolve
    void update(int neighbors) { 
        evolve(neighbors); 
    }

    // Print '*' if alive, '.' if dead
    void print(ostream& os) const override {
        os << (_state ? '*' : '.');
    }

    // Conway cells affect all 8 directions
    bool affectsDirection(int, int) const override {
        return true;
    }

    // If alive, add 1 to count
    void next_Evolution(int& count) const override {
        if (_state) ++count;
    }
};

// Concrete class for Fredkin cells
class FredkinCell : public AbstractCell {
private:
    size_t _age = 0; // How long the cell has been alive

public:
    FredkinCell(bool state = false, size_t age = 0)
        : AbstractCell(state), _age(age) {}

    // Make a copy of this cell
    AbstractCell* clone() const override {
        return new FredkinCell(*this);
    }

    // Update Fredkin cell based on neighbor rules
    Generation evolve(int neighbors) override {
        bool was_alive = _state;
        // Checks if cell is dead or alive
        if (_state) {
            // Alive Fredkin cells die with 0, 2, or 4 neighbors
            _state = !(neighbors == 0 || neighbors == 2 || neighbors == 4);
        } else {
            // Dead Fredkin cells come alive with 1 or 3 neighbors
            _state = (neighbors == 1 || neighbors == 3);
        }

        // Increment age if still alive
        _age += (was_alive && _state) ? 1 : 0;

        return Generation(CellKind::Fredkin, _state, static_cast<int>(_age));
    }

    // Update wrapper for evolve
    void update(int neighbors) { 
        evolve(neighbors); 
    }

    // Print character based on age or dead symbol '-'
    void print(ostream& os) const override {
        // If the cell is alive
        if (_state) {
           // If the cell's age is 10 or more, print '+'
            if (_age >= 10) os << '+';
            else os << static_cast<char>('0' + _age);
        } else {
            os << '-'; // cell is dead
        }
    }

    // Fredkin cells only affect cardinal directions (no diagonals)
    bool affectsDirection(int dr, int dc) const override {
        return (dr * dc) == 0;
    }

    // If alive, add 1 to count
    void next_Evolution(int& count) const override {
        if (_state) ++count;
    }
};

// Wrapper class for any type of cell 
class Cell {
private:
    AbstractCell* _cell;

public:
    Cell() : _cell(new FredkinCell()) {}
    Cell(bool s) : _cell(new FredkinCell(s)) {}
    Cell(const Cell& other) : _cell(other._cell->clone()) {}
    Cell(AbstractCell* ptr) : _cell(ptr) {}
   
    ~Cell() { delete _cell; }

    // Copy assignment
    Cell& operator=(const Cell& other) {
        // check for self assignment
        if (this != &other) {
            // clone the other cell to create a separate copy
            AbstractCell* new_cell = other._cell->clone();
            delete _cell;
            _cell = new_cell;
        }
        return *this;
    }

    // Update the internal cell with neighbor count
    void update(int neighbor_count) {
        assert(_cell != nullptr);
        Generation state = _cell->evolve(neighbor_count);

        // Special case: after Fredkin cell reaches age 2, change into Conway
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

// Main Life class handling the simulation
template <typename T>
class Life {
private:
    vector<vector<T>> _grid;  // Grid of cells
    size_t _rows;
    size_t _cols;
    int _generation = 0;  

    // Compute the number of alive neighbors for every cell
    vector<vector<int>> compute_neighbor_counts() const {
        assert(!_grid.empty() && !_grid[0].empty());

        vector<vector<int>> counts(_rows, vector<int>(_cols, 0));

        // Directions: N, S, W, E, and diagonals
        const vector<pair<int, int>> directions = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}, 
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };

        for (size_t r = 0; r < _rows; ++r) {
            for (size_t c = 0; c < _cols; ++c) {
                int alive_check = 0;
                _grid[r][c].next_Evolution(alive_check);
                //checks whether alive or not, then it skip dead cells
                if (alive_check == 0) continue;

                for (const auto& [dr, dc] : directions) {
                    int nr = static_cast<int>(r) + dr;
                    int nc = static_cast<int>(c) + dc;

                    // This makes sure we are inside the grid
                    if (nr >= 0 && nr < static_cast<int>(_rows) &&
                        nc >= 0 && nc < static_cast<int>(_cols)) {

                        // Only increase neighbor count if the neighbor is affected
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
    // Initialize the grid
    Life(size_t rows, size_t cols) 
        : _grid(rows, vector<T>(cols)), _rows(rows), _cols(cols) {}

    // Replace a specific cell with a live one
    void replace_cell(int r, int c) {
        assert(r >= 0 && r < static_cast<int>(_rows));
        assert(c >= 0 && c < static_cast<int>(_cols));
        //Checks if the type T is not a cell
        if constexpr (!std::is_same_v<T, Cell>) {
            // If using FredkinCell or ConwayCell directly
            _grid[r][c] = T(true);
        } else {
            // If using polymorphic Cell wrapper
            _grid[r][c] = Cell(new FredkinCell(true));
        }
    }

    // Evolve the whole grid by one generation
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

    // Print a specific cell
    void print_cell(size_t r, size_t c, ostream& os) const {
        assert(r < _rows && c < _cols);
        _grid[r][c].print(os);
    }

    // Return the total number of alive cells
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
