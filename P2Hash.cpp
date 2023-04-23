//============================================================================
// Name        : P2Hash.cpp
// Author      : Lukas Pentowski
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <fstream>
#include <string> // atoi
#include <vector>
#include <sstream>
#include "CSVparser.hpp"


//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(std::string str, char ch);

// define a structure to hold Course information
struct Course {
    std::string courseId; // unique identifier
    std::string name;
    std::vector<std::string> prereq;
    Course() {
       
    }
};

class HashTable {

private:
    // Define structures to hold bids
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a bid
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a bid and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    std::vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    Course Search(std::string courseId);
};
/**
 * Default constructor
 */
HashTable::HashTable() {

    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}

HashTable::HashTable(unsigned int size) {
    
    this->tableSize = size;
    // resize nodes size
    nodes.resize(tableSize);

}

/**
 * Destructor
 */
HashTable::~HashTable() {
    // FIXME (4): Implement logic to free storage when a class is destroyed.

    // erase nodes beginning
    nodes.erase(nodes.begin());
}

unsigned int HashTable::hash(int key) {

    // return key tableSize
    return key % tableSize;
}

/**
 * Insert a course
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Course course) {
    
    // create the key for the given bid
    unsigned key = hash(atoi(course.courseId.c_str()));

    // retrieve node using key
    Node* oldNode = &(nodes.at(key));

    // if no entry found for the key
    if (oldNode == nullptr) {

        // assign this node to the key position
        Node* newNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*newNode));
    }

    // else if node is not used
    else {
        // assing old node key to UNIT_MAX, set to key, set old node to bid and old node next to null pointer
        if (oldNode->key == UINT_MAX) {
            oldNode->key = key;
            oldNode->course = course;
            oldNode->next = nullptr;
        }
        // else find the next open node
        else {
            while (oldNode->next != nullptr) {
                oldNode = oldNode->next;
            }
            // add new newNode to end
            oldNode->next = new Node(course, key);
        }
    }

}

/**
 * Print all courses
 */
void HashTable::PrintAll() {

    /*For loop that goes through the size of the table.  Using the */
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        unsigned key = hash(atoi(nodes[i].course.courseId.c_str()));
        Node* node = &(nodes.at(key));
        //   if key not equal to UINT_MAx
        if (node->key != UINT_MAX) {
            // output key, bidID, title, amount and fund
            std::cout << node->course.courseId << ", " << node->course.name << std::endl;
            // node is equal to next iter
            node = node->next;
            // while node not equal to nullptr
            while (node != nullptr) {
                // output key, bidID, title, amount and fund
                /*I added spacing before the key value for ease of reading and understanding items are part
                of same bucket*/
                std::cout << node->course.courseId << ", " << node->course.name << std::endl;
                // node is equal to next node
                node = node->next;
            }
        }
    }
}

Course HashTable::Search(std::string bidId) {
    Course course;

    // create the key for the given bid
    unsigned key = hash(atoi(course.courseId.c_str()));
    Node* node = &(nodes.at(key));
    // if entry found for the key
    if (node != nullptr && node->key != UINT_MAX && node->course.courseId.compare(bidId) == 0) {
        //return node bid
        return node->course;
    }
    // if no entry found for the key
    if (node == nullptr || node->key == UINT_MAX) {
        // return bid
        return course;
    }

    // while node not equal to nullptr
    while (node != nullptr) {
        // if the current node matches, return it
        if (node->key != UINT_MAX && node->course.courseId.compare(bidId) == 0) {
            return node->course;
        }
        //node is equal to next node
        node = node->next;
    }

    return course;
}

void displayCourse(Course course) {
    std::cout << course.courseId << ", " << course.name << std::endl;
    std::cout << "Prerequisites: ";
    
    unsigned int i = 0;
    for (i = 0; i < course.prereq.size(); i++) {
        std::cout << course.prereq.at(i);
        if (i != course.prereq.size() - 1)
            std::cout << ", ";
    }
    if (i == 0)
        std::cout << "No prerequisites required.";
    std::cout << std::endl;
    return;
}


void loadCourses(std::string csvPath, HashTable* hashTable) {
    std::cout << "Loading CSV file " << csvPath << std::endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    //vector<string> header = file.getHeader();
    //for (auto const& c : header) {
    //     cout << c << " | ";
    // }
    // cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Course course;
            course.courseId = file[i][0];
            course.name = file[i][1];
            for (unsigned int j = 2; j < file.getRow(i).size(); j++) {
                course.prereq.push_back(file[i][j]);
            }

            
            // push this course to the end
            hashTable->Insert(course);
        }
    }
    catch (csv::Error& e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(std::string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    std::string csvPath, searchKey;
    
    // Define a timer variable
    clock_t ticks;

    // Define a hash table to hold all the courses
    HashTable* courseTable;

    Course course;
    courseTable = new HashTable();

    std::cout << "Welcome to the course planner." << std::endl;

    int choice = 0;
    while (choice != 9) {
        std::cout << "  1. Load Data Structure." << std::endl;
        std::cout << "  2. Print Course List." << std::endl;
        std::cout << "  3. Print Course." << std::endl;
        std::cout << "  9. Exit" << std::endl;
        std::cout << "What would you like to do?: ";
        std::cin >> choice;

        switch (choice) {
            
        case 1:
           

            std::cout << "Enter the file that contains the course information: ";
            std::cin >> csvPath;
            

            loadCourses(csvPath, courseTable);
            csvPath = "";
            
             break;

        case 2:
            courseTable->PrintAll();
            break;

        case 3:
            std::cout << "What course do you want to know about? ";
            std::cin >> searchKey;

            course = courseTable->Search(searchKey);

            if (!course.courseId.empty()) {
                displayCourse(course);
            }
            else {
                std::cout << "Course Id " << searchKey << " not found." << std::endl;
            }

           
            break;
        }
    }

    std::cout << "Thank you for using the course planner!" << std::endl;

    return 0;
}
