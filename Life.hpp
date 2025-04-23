// -------------
// Life.hpp
// -------------

#ifndef Life_hpp
#define Life_hpp

#include <vector>
#include <cstddef>

//Class life
template <typename T>
class Life {
private:
    std::vector<std::vector<T>> _grid;

public:
    Life(size_t rows, size_t cols);
    void eval();
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


class AbstractCell {
protected:
    //the state of the cell
    bool _state;
    

public:
    AbstractCell(bool state);
    //virtual destructor
    virtual ~AbstractCell() = default;
    virtual AbstractCell* clone() const = 0;
    virtual void evolve() = 0;
};

//Conway Cell
class ConwayCell : public AbstractCell {
public:
    //default constructor
    ConwayCell () = default;
    ConwayCell(bool state);
    ConwayCell* clone() const override;
    //deconstructor
    ~ ConwayCell() = default;
    void evolve() override;
};

//Fredkin Cell cell
class FredkinCell : public AbstractCell {
private:
    size_t _age;

public:
    FredkinCell(bool state, size_t age);
    FredkinCell* clone() const override;
    void evolve() override;
};



#endif // Life_HPP
