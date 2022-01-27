//
// Created by Andrew Dunstan on 10/29/20.
//

#ifndef SUDOKUSOLVER_NODEHANDLER_H
#define SUDOKUSOLVER_NODEHANDLER_H

#include <vector>
#include <string>


class ColumnHead;

class Node{
public:
    Node();
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::shared_ptr<Node> up;
    std::shared_ptr<Node> down;
    std::shared_ptr<Node> column;
    std::string rowName;
    std::string name;
    int columnSize;
};

class NodeHandler{
private:
    std::shared_ptr<Node> master;
    void removeNode(std::shared_ptr<Node> nodeToRemove);
    void coverColumn(std::shared_ptr<Node> rowHeadToCover);
    void uncoverColumn(std::shared_ptr<Node> rowHeadToCover);
    std::shared_ptr<Node> findSmallestCol();
    std::vector<std::shared_ptr<Node>> posibleSolution;


public:
    NodeHandler(std::vector<std::vector<bool>> baseMatrix, std::vector<std::string> colNames, std::vector<std::string> rowNames);
    void printListColumns();
    void removeStartingCells(std::vector<int> colsToRemove);
    void search(int depth, int &numSolutions, std::vector<std::string> &stringSolution);
};

#endif //SUDOKUSOLVER_NODEHANDLER_H
