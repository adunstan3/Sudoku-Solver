//
// Created by Andrew Dunstan on 10/29/20.
//

#include "NodeHandler.h"
#include <vector>
#include <iostream>

using namespace std;

Node::Node(){
    left = nullptr;
    right = nullptr;
    up = nullptr;
    down = nullptr;
    column = nullptr;
    name = "empty";
    columnSize = 0;
}


NodeHandler::NodeHandler(vector<vector<bool>> matrix, vector<string> colNames, vector<string> rowNames){
    //Create a blank 2 dimentional circular doubly linked list the same size as the matrix

    //Create the master head: our access point to the whole list structure
    master = make_shared<Node>();
    master->left = master;
    master->right = master;
    master -> name = "master";

    //Start by making the column head circular doubly linked list with the master head then add the nodes to each column
    for(int colNum = 0; colNum < matrix.size(); colNum++) {
        //Make a new column head and insert it to the left of the master in a circular doubly linked list
        shared_ptr<Node> currentColumnHead = make_shared<Node>();
        currentColumnHead -> left = master -> left;
        currentColumnHead -> right = master;
        master -> left -> right = currentColumnHead;
        master -> left = currentColumnHead;
        //The first column head has to be set to the master right manualy
        if(master -> right == master){
            master -> right = currentColumnHead;
        }
        currentColumnHead -> name = "Column Head "+to_string(colNum) + ": " + colNames[colNum];

        currentColumnHead -> up = currentColumnHead;
        currentColumnHead -> down = currentColumnHead;

        //For every column head make a virtical circular doubly linked list of nodes
        for (int rowNum = 0; rowNum < matrix[0].size(); rowNum++){
            //Create the vertical circular links in the main 2d link structure
            shared_ptr<Node> currentNode = make_shared<Node>();
            currentNode -> up = currentColumnHead -> up;
            currentNode -> down = currentColumnHead;
            currentColumnHead -> up -> down = currentNode;
            currentColumnHead -> up = currentNode;
            //The first node has to be set to the column head manualy
            if(currentColumnHead -> down == currentColumnHead){
                currentColumnHead -> down = currentNode;
            }
            currentNode -> column = currentColumnHead;
            currentNode -> rowName = rowNames[rowNum];
            currentNode -> name = "Column: " + to_string(colNum) + ", Row: "+to_string(rowNum);
        }
    }

    //Now go back in and add the links for the horizontal doubly linked lists in the main 2d list structure
    auto leftNodeStart = master -> right;
    auto rightNodeStart = master -> right -> right;
    auto leftNodeSliding = leftNodeStart;
    auto rightNodeSliding = rightNodeStart;

    for (int rowNum = 0; rowNum < matrix[0].size(); rowNum++) {
        //Move everything down a row
        leftNodeStart = leftNodeStart -> down;
        rightNodeStart = rightNodeStart -> down;

        //Make copy nodes that will move horozontaly along the 2d list
        leftNodeSliding = leftNodeStart;
        rightNodeSliding = rightNodeStart;

        //link all of the left and right links for this row
        for(int colNum = 0; colNum < matrix.size()-2; colNum++) {
            //set the horizontal links
            leftNodeSliding -> right = rightNodeSliding;
            rightNodeSliding -> left = leftNodeSliding;

            //move links to next position to the right
            leftNodeSliding = rightNodeSliding;
            rightNodeSliding = rightNodeSliding -> up -> right -> down;
        }
        leftNodeSliding -> right = rightNodeSliding;
        rightNodeSliding -> left = leftNodeSliding;

        //The interior left and right links are done now wrap around the edges making it circular
        rightNodeSliding -> right = leftNodeStart;
        leftNodeStart -> left = rightNodeSliding;
    }

    //Blank 2d list is finished now go in and remove the matrix 0s to give the list structure leaving the 1s
    //Count up the number of 1s in each column and set it to column size
    auto nodeSliding = master -> right;
    auto nodeStart = master -> right -> down;
    for (int rowNum = 0; rowNum < matrix[0].size(); rowNum++) {
        //Move everything down a row
        nodeSliding = nodeStart;
        nodeStart = nodeStart -> down;

        for (int colNum = 0; colNum < matrix.size(); colNum++) {
            nodeSliding = nodeSliding -> right;
            if(matrix[colNum][rowNum]==0){
                removeNode(nodeSliding -> left);
            }
        }
        //cout << nodeSliding -> name << endl;
    }

    auto currentColumn = master -> right;
    for(int colNum = 0; colNum < matrix.size(); colNum++) {
        int columnSize = 0;
        for (int rowNum = 0; rowNum < matrix[0].size(); rowNum++) {
            if(matrix[colNum][rowNum]==1){
                columnSize++;
            }
        }
        currentColumn -> columnSize = columnSize;

        if(colNum <= matrix.size()-1) {
            currentColumn = currentColumn -> right;
        }
    }

}

void NodeHandler::removeNode(shared_ptr<Node> nodeToRemove){
    //Set the neighbor links to the new destinations
    //This data structure is so cool!
    nodeToRemove -> right -> left = nodeToRemove -> left;
    nodeToRemove -> left -> right = nodeToRemove -> right;
    nodeToRemove -> up -> down = nodeToRemove -> down;
    nodeToRemove -> down -> up = nodeToRemove -> up;

    //clear all the pointers in the node
    nodeToRemove -> right = nullptr;
    nodeToRemove -> left = nullptr;
    nodeToRemove -> up = nullptr;
    nodeToRemove -> right = nullptr;
    nodeToRemove -> column = nullptr;
}

void NodeHandler::coverColumn(shared_ptr<Node> colHeadToCover){
    //Remove the head from the columns list
    colHeadToCover -> left -> right = colHeadToCover -> right;
    colHeadToCover -> right -> left = colHeadToCover -> left;

    //Move down the column
    auto colNodeToCover = colHeadToCover -> down;
    auto rowNodeToCover = colNodeToCover;
    while(colNodeToCover != colHeadToCover){

        //Move down the rows linked to the column to the right
        rowNodeToCover = colNodeToCover -> right;
        while(rowNodeToCover != colNodeToCover){
            //cut out the row above and below update the column sizes
            rowNodeToCover -> up -> down = rowNodeToCover -> down;
            rowNodeToCover -> down -> up = rowNodeToCover -> up;
            rowNodeToCover -> column -> columnSize --;
            rowNodeToCover = rowNodeToCover -> right;
        }

        colNodeToCover = colNodeToCover -> down;
    }
}

void NodeHandler::uncoverColumn(shared_ptr<Node> colHeadToUncover){
    //Move up the column
    auto colNodeToUncover = colHeadToUncover -> up;
    auto rowNodeToUncover = colHeadToUncover;
    while(colNodeToUncover != colHeadToUncover){

        //Move down the rows linked to the column to the left
        rowNodeToUncover = colNodeToUncover -> left;
        while(rowNodeToUncover != colNodeToUncover){
            //Add in the row nodes above and below update the column sizes
            rowNodeToUncover -> up -> down = rowNodeToUncover;
            rowNodeToUncover -> down -> up = rowNodeToUncover;
            rowNodeToUncover -> column -> columnSize ++;
            rowNodeToUncover = rowNodeToUncover -> left;
        }
        colNodeToUncover = colNodeToUncover -> up;
    }

    //Add the head of the column back into the list
    colHeadToUncover -> left -> right = colHeadToUncover;
    colHeadToUncover -> right -> left = colHeadToUncover;
}

shared_ptr<Node> NodeHandler::findSmallestCol(){
    int smallestSize = 99999999;
    auto currentColumn = master -> right;
    auto smallestColumn = currentColumn;
    while(currentColumn != master){
        if(currentColumn -> columnSize < smallestSize){
            smallestSize = currentColumn -> columnSize;
            smallestColumn = currentColumn;
        }
        currentColumn = currentColumn -> right;
    }

    return smallestColumn;
}


void NodeHandler::printListColumns(){
    auto nodeSliding = master -> right;
    auto currentColumn = nodeSliding;
    while(nodeSliding != master){
       cout << nodeSliding->name << endl;
       currentColumn = nodeSliding;
       nodeSliding = nodeSliding -> down;
       while(nodeSliding != currentColumn){
           //cout << nodeSliding->name << " | " << nodeSliding->rowName << endl;
           nodeSliding = nodeSliding -> down;
       }
       nodeSliding = nodeSliding -> right;
    }
}

void NodeHandler::removeStartingCells(vector<int> colsToRemove){
    auto colNodeToRemove = master;

    //Find and store the pointers to remove
    vector<shared_ptr<Node>> pointersToRemove;
    for(auto colToRemove: colsToRemove){
        colNodeToRemove = master;
        for(int colNum = 0; colNum < colToRemove+1; colNum++){
            colNodeToRemove = colNodeToRemove -> right;
        }

        pointersToRemove.push_back(colNodeToRemove);
        //cout << colNodeToRemove -> name << endl;
    }

    //Remove the found pointers
    for(auto pointerToRemove: pointersToRemove){
        coverColumn(pointerToRemove);
        removeNode(pointerToRemove);
    }
}

void NodeHandler::search(int depth, int &numSolutions, vector<string> &solutionStrings){
    if(master -> right == master){
        //the 2d list is empty you have found the solution!
        //print out the solution
        solutionStrings.clear();

        for(auto solutionRowNode : posibleSolution){
            string cellSolution =  solutionRowNode ->rowName;
            solutionStrings.push_back(cellSolution);
        }

        numSolutions ++;
        return;
    }

    auto currentCol = findSmallestCol();
    coverColumn(currentCol);

    //slide down the column
    auto rowSlider = currentCol -> down;
    while(rowSlider != currentCol){
        if(posibleSolution.size() == depth){
            //there isn't space for the row so push it back
            posibleSolution.push_back(rowSlider);
        }else{
            //we have already visited this depth so the vector has a spot for this depth's row
            posibleSolution[depth] = rowSlider;
        }


        //slid across the row and cover the columns with 1s
        auto colSlider = rowSlider -> right;
        while(colSlider != rowSlider){
            coverColumn(colSlider -> column);

            colSlider = colSlider -> right;
        }

        //recursivly search the next depth
        search(depth+1, numSolutions, solutionStrings);

        //this branch was unsucessfull so reset and prepare to try a different branch

        //reset variables
        rowSlider = posibleSolution[depth];
        currentCol = rowSlider -> column;

        //slide along the row and uncover the columns with 1s
        colSlider = rowSlider -> left;
        while(colSlider != rowSlider){
            uncoverColumn(colSlider -> column);

            colSlider = colSlider -> left;
        }

        rowSlider = rowSlider -> down;
    }

    uncoverColumn(currentCol);
}

/*
void NodeHandler::printSolution(){
    cout << "You found a solution!" << endl;
    cout << "----------------------------------------------------------------------------------------------------" << endl;



    int col, row, value;
    for(auto solutionRowNode : posibleSolution){
        string cellSolution =  solutionRowNode ->rowName;

    }
}
*/