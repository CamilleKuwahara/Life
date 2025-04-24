#ifndef Life_hpp
#define Life_hpp

#include <vector>
#include <cstddef>
#include <iostream>

using namespace std;

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
    virtual void evolve() = 0;
    // Returns the current state of the cell
    virtual bool state() const = 0;
    //check if cell is alive
    bool isAlive() const {
        return _state;
    }
};

// ConwayCell
class ConwayCell : public AbstractCell {
public:
    ConwayCell() : AbstractCell(false) {}
    ConwayCell(bool state) : AbstractCell(state) {}
    //destructor
    ~ConwayCell() = default;
    //updates the state
    void evolve() override {
        _state = !_state;
    }

    AbstractCell* clone() const override {
        return new ConwayCell(_state);
    }

    bool state() const override {
        return _state;
    }

};

// FredkinCell
class FredkinCell : public AbstractCell {
private:
    size_t _age; //Tracks the number of evolutiosn while alive

public:
    FredkinCell(bool state = false) : AbstractCell(state), _age(0) {}
    
    ~FredkinCell() = default;
    //increments age if alive
    void evolve() override {
        if (_state) {
            ++_age;
        }
        _state = !_state;
    }

    AbstractCell* clone() const override {
        return new FredkinCell(*this);
    }

};

// Cell wrapper to manage dynamic AbstractCe instances
class Cell {
private:
    AbstractCell* _pointer;

public:
    //deep copy
    Cell(AbstractCell* cell) : _pointer(cell->clone()) {}
    //copy constructor for uses clone
    Cell(const Cell& other) : _pointer(other._pointer->clone()) {}
    //move constructor
    Cell(Cell&& other) noexcept : _pointer(other._pointer) { other._pointer = nullptr; }

    Cell& operator=(Cell&& other) noexcept {
        if (this != &other) {
            delete _pointer;
            _pointer = other._pointer;
            other._pointer = nullptr;
        }
        return *this;
    }

    ~Cell() {
        delete _pointer;
    }

    bool alive() const {
        return _pointer->isAlive();
    }
  
};

// Life class template
template <typename T>
class Life {
private:
    vector<vector<T>> _grid; // 2D grid of cells

public:
    // Initializes grid
    Life(size_t rows, size_t cols) {
        _grid.resize(rows, vector<T>(cols));
    }
    // Advances the simulation to one generation
    void eval() {
        for_each(_grid.begin(), _grid.end(), [](auto& row) {
            for_each(row.begin(), row.end(), [](auto& cell) {
                cell.evolve();
            });
        });
    }
    
    // Prints the board
    void print() const {
        for (size_t i = 0; i < _grid.size(); ++i) {
            for (size_t j = 0; j < _grid[i].size(); ++j) {
                _grid[i][j].print();
            }
            cout << '\n';
        }
    }
 
};

#endif // Life_hpp
