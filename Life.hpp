// -------------
// Life.hpp
// -------------

#ifndef Life_hpp
#define Life_hpp

#include <vector>
#include <cstddef>

class AbstractCell {
protected:
    bool _state;

public:
    AbstractCell(bool state);
    virtual ~AbstractCell();
    virtual AbstractCell* clone() const = 0;
    virtual void evolve() = 0;
};

class ConwayCell : public AbstractCell {
public:
    ConwayCell(bool state);
    ConwayCell* clone() const override;
    void evolve() override;
};

class FredkinCell : public AbstractCell {
private:
    size_t _age;

public:
    FredkinCell(bool state, size_t age);
    FredkinCell* clone() const override;
    void evolve() override;
};

class Cell {
private:
    AbstractCell* _p;

public:
    Cell(AbstractCell*);
    Cell(const Cell&);
    Cell(Cell&&);
    Cell& operator=(const Cell&);
    Cell& operator=(Cell&&);
    ~Cell();
    void evolve();
};

template <typename T>
class Life {
private:
    std::vector<std::vector<T>> _grid;

public:
    Life(size_t rows, size_t cols);
    void eval();
};



#endif // Life_HPP
