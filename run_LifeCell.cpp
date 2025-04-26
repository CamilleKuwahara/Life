#include "Life.hpp"
#include <iostream>
#include <string>

int main() {
    int test_cases;
    std::cin >> test_cases;

    std::string line;
    std::getline(std::cin, line); 
    std::getline(std::cin, line); 

    // Loop over each test case
    for (int t = 0; t < test_cases; ++t) {
        int rows, cols;
        std::cin >> rows >> cols;

        // Create a Life simulation with Cell polymorphic type
        Life<Cell> simulation(rows, cols);

        std::cout << "*** Life<Cell> " << rows << "x" << cols << " ***\n";
        
        int num_cells;
        std::cin >> num_cells;

        // Read initial live cells and place them on the grid
        for (int i = 0; i < num_cells; ++i) {
            int r, c;
            std::cin >> r >> c;
            simulation.replace_cell(r, c);
        }
        
        int generations, frequency;
        std::cin >> generations >> frequency;

        std::cout << "\n";
        std::cout << "Generation = 0, Population = " << simulation.population() << "." << std::endl;

        // Print the initial grid
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                simulation.print_cell(r, c, std::cout);
            }
            std::cout << '\n';
        }

        // Evolve for the specified number of generations
        for (int gen = 1; gen <= generations; ++gen) {
            simulation.eval(); // Step simulation to next generation

            // Only print if generation number is a multiple of frequency
            if (gen % frequency == 0) {
                std::cout << "\n";
                std::cout << "Generation = " << gen << ", Population = " << simulation.population() << "." << std::endl;

                // Print the updated grid
                for (size_t r = 0; r < rows; ++r) {
                    for (size_t c = 0; c < cols; ++c) {
                        simulation.print_cell(r, c, std::cout);
                    }
                    std::cout << '\n';
                }
            }
        }

        // If there are more test cases, prepare by consuming blank lines
        if (t < test_cases - 1) {
            std::getline(std::cin, line);
            std::getline(std::cin, line); 
            std::cout << "\n"; 
        }
    }

    return 0;
}
