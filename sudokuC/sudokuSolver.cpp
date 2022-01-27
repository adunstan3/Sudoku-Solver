//
// Created by Andrew Dunstan on 10/20/20.
//

#include "sudokuSolver.h"
#include "NodeHandler.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

SudokuSolver::SudokuSolver(int _boxSize){
    //Create variables
    boxSize = _boxSize;
    size = _boxSize*_boxSize;
    int numSolutions = 0;
    vector<string> stringSolution;

    //set up generic empty sudoku structure
    createBaseMatrix();
    NodeHandler myDLX(sudokuM, colLabels, rowLabels);

    //load and format the user's seed sudoku values
    loadStartingValues();
    startingValuesToColNums();
    bool isValid = checkValidSeedSudoku();

    //If the input is valid edit the generic structure then solve the sudoku
    if(isValid){
        myDLX.removeStartingCells(colToBeRemovedByStartingValues);
        myDLX.search(0, numSolutions, stringSolution);
    }else{
        cout << "not valid!" << endl;
    }

    //writes a blank grid if the sudoku isn't valid, writes the solution if the sudoku is valid
    writeSolution(stringSolution, numSolutions);

    //myDLX.printListColumns();
}

void SudokuSolver::createBaseMatrix(){
    //Initialize the sudoku matrix colum labels. These represent every way you could place a number into a sudoku grid
    //Every square you could place into times all of your number options Ex: C1R4N3 would place the number 3 in column
    //1 and row 4
    string numPlacementPosibility;
    for(int col = 1; col <= size; col ++){
        for(int row = 1; row <= size; row++){
            for(int num = 1; num <= size; num++){
                numPlacementPosibility = "C" + to_string(col);
                numPlacementPosibility += "R" + to_string(row);
                numPlacementPosibility += "N" + to_string(num);
                numPlacementPosibility += "B" + to_string(colRowToBox(col, row));

                rowLabels.push_back(numPlacementPosibility);
                numPlacementPosibility = "";
            }
        }
    }

    //Now initialise all the sudoku matrix rows. These represent all the rules that a specific number placement could
    //Trigger. The 4 types of rules are one number in every square, and one of each number in each box, col and row

    string potentialRule;
    for(int a = 1; a<= size; a++){
        for(int b= 1; b <= size; b++){
            //One number in each cell
            potentialRule = "C" + to_string(a);
            potentialRule += "R" + to_string(b);
            colLabels.push_back(potentialRule);

            //One of each number in each column
            potentialRule.replace(potentialRule.find('R'), 1, "N");
            colLabels.push_back(potentialRule);

            //One of each number in each row
            potentialRule.replace(potentialRule.find('C'), 1, "R");
            colLabels.push_back(potentialRule);

            //One of each number in each box
            potentialRule.replace(potentialRule.find('R'), 1, "B");
            colLabels.push_back(potentialRule);
        }
    }

    //Initialize a blank base sudoku matrix
    for(int col = 0; col <colLabels.size(); col ++){
        vector<bool> temp;
        for(int row = 0; row <rowLabels.size(); row ++){
            temp.push_back(false);
        }
        sudokuM.push_back(temp);
    }

    //Now fill out the base sudoku matrix. Iterate over every number entry and rule and set the squares where entries
    //and rules match to true
    for(int placement = 0; placement < rowLabels.size(); placement++){
        for(int rule = 0; rule < colLabels.size(); rule++){
            string placementStr = rowLabels[placement];
            string ruleStr = colLabels[rule];

            //If the first part of the rule is in the placement, then that placement fullfills part of the rule
            if (placementStr.find(ruleStr.substr(0,2)) != string::npos) {
                //If the second part of the rule is in the placement, then that placement fullfills the rule
                if (placementStr.find(ruleStr.substr(2,2)) != string::npos) {
                    sudokuM[rule][placement] = true;
                }
            }
        }
    }

    //Remove the information about what box the placement is in we don't need it anymore and it will get in the
    //way of future string manipulation.

    for(int placement = 0; placement < rowLabels.size(); placement++){
        rowLabels[placement] = rowLabels[placement].substr(0,6);
    }

}

int SudokuSolver::colRowToBox(int col, int row){
    //Boxes are labeled like reading starting at 1 ending at 9
    // 1 2 3 \n 4 5 6 \n 7 8 9
    int boxCol = (col-1) / boxSize;
    int boxRow = (row-1) / boxSize;
    int boxNum = boxCol + 1 + (boxRow*3);
    return boxNum;
}

void SudokuSolver::loadStartingValues(){
    //Starting values have rows as small arrays columns as the big array
    ifstream MyReadFile("sudokuNumbers.txt");
    string line;
    for(int lineNum = 0; lineNum<size; lineNum ++){
        getline (MyReadFile, line);
        vector<int> temp;
        for(int charNum = 0; charNum<size; charNum++){
            if(line[charNum] != '-'){
                temp.push_back(int(line[charNum]- '0'));
            }else{
                temp.push_back(-1);
            }
            //cout<< line[charNum];
        }
        startingValues.push_back(temp);
        //cout << endl;
    }
    MyReadFile.close();
}

void SudokuSolver::startingValuesToColNums(){
    int boxNum;
    for(int rowNum = 0; rowNum < startingValues.size(); rowNum++) {
        for (int colNum = 0; colNum < startingValues[0].size(); colNum++) {

            int squareNumber = startingValues[rowNum][colNum];
            if(squareNumber != -1){
                boxNum = colRowToBox(colNum+1, rowNum+1);
                //cout << "boxNum: "<< boxNum << ", colNum: " << colNum << ", rowNum: " << rowNum << ", value: " << squareNumber << endl;

                //Calculate all the column numbers that need to be covered since this number is given
                int cellRuleCol = ((((colNum)*9)+ (rowNum))*4);
                int colRuleCol = ((((colNum)*9)+ (squareNumber-1))*4)+1;
                int rowRuleCol = ((((rowNum)*9)+ (squareNumber-1))*4)+2;
                int boxRuleCol = ((((boxNum-1)*9)+ (squareNumber-1))*4)+3;

                colToBeRemovedByStartingValues.push_back(cellRuleCol);
                colToBeRemovedByStartingValues.push_back(colRuleCol);
                colToBeRemovedByStartingValues.push_back(rowRuleCol);
                colToBeRemovedByStartingValues.push_back(boxRuleCol);
                //cout << cellRuleCol << ", " << colRuleCol << ", " << rowRuleCol << ", " << boxRuleCol << endl;
            }
        }
    }
}

bool SudokuSolver::checkValidSeedSudoku(){
    bool isValid = true;
    //If there are duplicates in this removal column the seed is not valid
    //Sort and check ajaicent values for matches
    sort(colToBeRemovedByStartingValues.begin(), colToBeRemovedByStartingValues.end());
    int first = 0, second = 1;
    while(second < colToBeRemovedByStartingValues.size()){
        if(colToBeRemovedByStartingValues[first] == colToBeRemovedByStartingValues[second]){
            isValid = false;
            return isValid;
        }
        first++;
        second++;
    }
    return isValid;
}

void SudokuSolver::writeSolution(vector<string> stringSolution, int numSolutions){

    //Initialize the 2d array formated Solution to slot the solution digits into
    vector<vector<char>> formatedSolution;
    for(int i = 0; i < size; i++){
        vector<char> temp;
        for(int j = 0; j < size; j++){
            temp.push_back('-');
        }
        formatedSolution.push_back(temp);
    }

    int col, row;
    char value;
    for(auto cellString : stringSolution){
        //cout << cellString << endl;

        col = int(cellString[1]) - '0';
        row = int(cellString[3]) -'0';
        value = cellString[5];

        formatedSolution[col-1][row-1] = value;
    }

    ofstream MyFile("solutions.txt");
    MyFile << numSolutions << endl;
    // Close the file
    for(int col = 0; col < size; col++) {
        for (int row = 0; row < size; row++) {
            MyFile << formatedSolution[row][col];
        }
        MyFile << endl;
    }
    MyFile.close();
}



//Check output of program rows should all add up to 4, cols should all add up to size
/*
int total = 0;
for(int placement = 0; placement < rowLabels.size(); placement++){
    for(int rule = 0; rule < colLabels.size(); rule++){
        total += int(sudokuM[rule][placement]);
    }
    if(total != 4){
        cout << "Error!" << endl;
    }
    total = 0;
}
 */

//Check output of program
/*
for(int i = 100; i< 110; i++){
    cout << "placement: " + rowLabels[i] << endl;
    cout << "rule: " + colLabels[i] << endl;
}
cout << "placements length: " << rowLabels.size() << ", rules length: " << colLabels.size() << endl;
*/


