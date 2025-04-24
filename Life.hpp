#ifndef Life_hpp
#define Life_hpp

#include <vector>
#include <cstddef>
#include <iostream>

using namespace std;

// AbstractCell base class
class AbstractCell {
protected:
    bool _state;

public:
    AbstractCell(bool state) : _state(state) {}
    virtual ~AbstractCell() = default;

    virtual AbstractCell* clone() const = 0;
    virtual void evolve() = 0;
    virtual bool state() const = 0;
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

    void evolve() override {
        // Dummy evolve rule for compatibility
        // Normally would use neighbors, but this matches your interface
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
    size_t _age;

public:
    FredkinCell(bool state = false) : AbstractCell(state), _age(0) {}
    
    ~FredkinCell() = default;

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

// Cell wrapper
class Cell {
private:
    AbstractCell* _pointer;

public:
    Cell(AbstractCell* cell) : _pointer(cell->clone()) {}
    Cell(const Cell& other) : _pointer(other._pointer->clone()) {}
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
    vector<vector<T>> _grid;

public:
    Life(size_t rows, size_t cols) {
        _grid.resize(rows, vector<T>(cols));
    }

    void eval() {
        for (auto& row : _grid) {
            for (auto& cell : row) {
                cell.evolve();
            }
        }
    }

    void print() const {
        for (const auto& row : _grid) {
            for (const auto& cell : row) {
                cell.print();
            }
            cout << '\n';
        }
    }
};

#endif // Life_hpp
