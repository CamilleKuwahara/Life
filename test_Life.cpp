#include "gtest/gtest.h"
#include "Life.hpp"
#include <sstream>


TEST(CellTest, test_1) {
    Cell cell(true); 
    cell.update(1);  
    cell.update(1);  

    int is_alive = 0;
    cell.next_Evolution(is_alive);
    EXPECT_EQ(is_alive, 1); // Should be alive after mutation
}

TEST(LifeTest, test_2) {
    Life<Cell> sim(3, 3);
    sim.replace_cell(1, 1); 
    sim.eval();        
    EXPECT_EQ(sim.population(), 4); // Simple test: center spawns neighbors
}

TEST(LifeTest, test_3) {
    Life<ConwayCell> sim(2, 2);
    sim.replace_cell(0, 0);

    std::ostringstream out;
    for (size_t r = 0; r < 2; ++r) {
        for (size_t c = 0; c < 2; ++c) {
            sim.print_cell(r, c, out);
        }
        out << '\n';
    }

    std::string expected = "*.\n..\n";
    EXPECT_EQ(out.str(), expected);
}

TEST(CloneTest,test_4) {
    FredkinCell c(true, 5);
    FredkinCell* clone = dynamic_cast<FredkinCell*>(c.clone());
    ASSERT_NE(clone, nullptr);
    EXPECT_NE(clone, &c);
    delete clone;
}

TEST(CloneTest, test_5) {
    ConwayCell c(true);
    ConwayCell* clone = dynamic_cast<ConwayCell*>(c.clone());
    ASSERT_NE(clone, nullptr);
    EXPECT_NE(clone, &c);
    delete clone;
}

TEST(FredkinToConwayTest,  test_6) {
    Cell cell(true); // FredkinCell
    cell.update(1);  // age = 1
    cell.update(1);  // age = 2 -> should mutate

    int is_alive = 0;
    cell.next_Evolution(is_alive);
    EXPECT_EQ(is_alive, 1);
}

TEST(FredkinSymbolTest,  test_7) {
    FredkinCell c(true, 10);
    std::ostringstream os;
    c.print(os);
    EXPECT_EQ(os.str(), "+");

    FredkinCell c2(true, 15);
    std::ostringstream os2;
    c2.print(os2);
    EXPECT_EQ(os2.str(), "+");
}

TEST(CellAssignment,  test_8) {
    Cell c(true);
    c = c; // Self-assignment
    int alive = 0;
    c.next_Evolution(alive);
    EXPECT_EQ(alive, 1);
}

TEST(LifeConwayTest,  test_9) {
    Life<ConwayCell> sim(3, 3);
    sim.replace_cell(1, 1);
    sim.replace_cell(1, 2);
    sim.replace_cell(2, 1);

    std::ostringstream out_before;
    for (size_t r = 0; r < 3; ++r) {
        for (size_t c = 0; c < 3; ++c) {
            sim.print_cell(r, c, out_before);
        }
        out_before << '\n';
    }

    sim.eval();

    std::ostringstream out_after;
    for (size_t r = 0; r < 3; ++r) {
        for (size_t c = 0; c < 3; ++c) {
            sim.print_cell(r, c, out_after);
        }
        out_after << '\n';
    }

    EXPECT_FALSE(out_before.str().empty());
    EXPECT_FALSE(out_after.str().empty());
}

TEST(LifePopulationTest,  test_10) {
    Life<Cell> sim(2, 2);
    EXPECT_EQ(sim.population(), 0);

    sim.replace_cell(0, 0);
    sim.replace_cell(0, 1);
    sim.replace_cell(1, 0);
    sim.replace_cell(1, 1);

    EXPECT_EQ(sim.population(), 4);
}

TEST(FredkinCellTest, test_11) {
    FredkinCell c(true); // alive

    EXPECT_FALSE(c.evolve(0).alive);
    c = FredkinCell(true);
    EXPECT_FALSE(c.evolve(2).alive);
    c = FredkinCell(true);
    EXPECT_FALSE(c.evolve(4).alive);

    c = FredkinCell(false);
    EXPECT_FALSE(c.evolve(0).alive);
    EXPECT_FALSE(c.evolve(2).alive);
    EXPECT_FALSE(c.evolve(4).alive);

    c = FredkinCell(false);
    EXPECT_TRUE(c.evolve(1).alive);
    c = FredkinCell(false);
    EXPECT_TRUE(c.evolve(3).alive);
}