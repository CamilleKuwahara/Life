#include "Life.hpp"
#include <iostream>
#include <string>

int main() {
    int test_cases;
    std::cin >> test_cases;
    std::string line;
    std::getline(std::cin, line); // consume newline after test count
    std::getline(std::cin, line); // consume blank line
    
    for (int t = 0; t < test_cases; ++t) {
        int rows, cols;
        std::cin >> rows >> cols;
        Life<FredkinCell> simulation(rows, cols);
        std::cout << "*** Life<FredkinCell> " << rows << "x" << cols << " ***\n";
        
        int num_cells;
        std::cin >> num_cells;
        for (int i = 0; i < num_cells; ++i) {
            int r, c;
            std::cin >> r >> c;
            simulation.replace_cell(r, c);
        }
        
        int generations, frequency;
        std::cin >> generations >> frequency;
        
        // Display generation 0 (initial state)
        std::cout << "\n";
        std::cout << "Generation = 0, Population = " << simulation.population() << "." << std::endl;
        
        // Print grid
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                // Need to access cells and print them
                simulation.print_cell(r, c, std::cout);
            }
            std::cout << '\n';
        }
        
        // Now evolve for the specified number of generations
        for (int gen = 1; gen <= generations; ++gen) {
            simulation.eval(); // Evolve to next generation
            
            if (gen % frequency == 0) {
                std::cout << "\n";
                std::cout << "Generation = " << gen << ", Population = " << simulation.population() << "." << std::endl;
                
                // Print grid
                for (size_t r = 0; r < rows; ++r) {
                    for (size_t c = 0; c < cols; ++c) {
                        simulation.print_cell(r, c, std::cout);
                    }
                    std::cout << '\n';
                }
            }
        }
        
        if (t < test_cases - 1) {
            std::getline(std::cin, line); // consume leftover newline
            std::getline(std::cin, line); // consume blank line between tests
            std::cout << "\n";
        }
    }
    return 0;
}