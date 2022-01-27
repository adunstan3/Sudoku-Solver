//
// Created by Andrew Dunstan on 10/20/20.
//

#ifndef SUDOKUC_SUDOKUSOLVER_H
#define SUDOKUC_SUDOKUSOLVER_H

#include <vector>
#include <string>

class SudokuSolver{
private:
    int boxSize;
    int size;
    std::vector<std::string> colLabels;
    std::vector<std::string> rowLabels;
    std::vector<std::vector<bool>> sudokuM;
    void createBaseMatrix();
    int colRowToBox(int col, int row);

    std::vector<std::vector<int>> startingValues;
    void loadStartingValues();

    std::vector<int> colToBeRemovedByStartingValues;
    void startingValuesToColNums();
    bool checkValidSeedSudoku();

    void writeSolution(std::vector<std::string> stringSolution, int numSolutions);

public:
    explicit SudokuSolver(int boxSize);
};

#endif //SUDOKUC_SUDOKUSOLVER_H
