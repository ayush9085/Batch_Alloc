# Automated Batch Allocation System (C)

The Automated Batch Allocation System is a modular C program designed to automatically assign students into batches for labs, tutorials, and academic activities. Manual allocation becomes inefficient and error-prone with large datasets, so this project uses sorting algorithms, file handling, and dynamic memory allocation to automate the process for 200+ students.

## Features
- Add new students dynamically
- View full student database
- Allocate batches using multiple strategies
- View allocated batches with capacity info
- Save and load CSV data
- Search student by roll/SAP ID
- Handles 200+ students efficiently

## Allocation Strategies
- Marks-based
- Alphabetical
- Reverse Alphabetical
- Random allocation
- Roll number-based

## Project Structure
project/
├── src/ (main logic files)
├── include/ (header files)
├── data/ (students.csv)
├── tests/ (test cases)
├── Makefile
└── README.md

## C Concepts Used
- Structures for Student and Batch data
- Dynamic memory allocation (malloc, calloc, realloc)
- File handling (CSV read/write)
- Sorting using qsort with custom comparators
- Modular programming with separate .c/.h modules
- Pointers and arrays for efficient data operations

## Input Format (students.csv)
id,name,marks  
101,Asha,90  
102,Raj,75  
103,Simran,88  

## Expected Output
Main Menu:
1. Add next student
2. View database
3. Allocate batches
4. View allocated batches
5. Save database
6. Load database
7. Search student by roll
8. Exit

Allocation Summary Example:
Batch A: 30/30  
Batch B: 30/30  
Batch C: 30/30  
Batch D: 30/30  
Unallocated Students: 80

## Build & Run
make  
./batch_alloc  

Clean:
make clean  

Run Tests:
make test  

## Contributing
Keep modules clean, commented, and separate. Follow consistent naming and structure.

## License
MIT
