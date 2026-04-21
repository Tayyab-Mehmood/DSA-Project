
# Student Plagiarism Detection System

## Description

This project is a plagiarism detection system that analyzes student text files and detects similarity between them. It identifies which student copied from whom and calculates the percentage of similarity using a fingerprint-based comparison approach.

---

## Features

* Compares multiple student submissions
* Calculates similarity percentage
* Detects possible source of copying
* Works with multiple datasets
* Generates a plagiarism report

---

## Technologies and Concepts Used

* Language: C++
* File Handling for reading text files
* Data Structures:

  * Linked List
  * Queue (implemented using Linked List)
  * Hashing
* Algorithms:

  * K-gram fingerprinting
  * Pairwise comparison

---

## Project Structure

* Input folders containing student text files
* Main C++ source file
* Output file: `plagiarism_report.csv`

---

## How the Project Works

### 1. File Input

The system reads multiple text files where each file represents a student's submission.

### 2. Tokenization

Text is broken into words and stored in a queue implemented using a linked list to maintain order.

### 3. K-Gram Generation

Words are grouped into k-grams (k = 3). These k-grams represent sequences of words used for comparison.

### 4. Hashing (Fingerprinting)

Each k-gram is converted into a numeric hash value (fingerprint) for efficient comparison.

### 5. Storage

Fingerprints are stored in a linked list along with their corresponding file identifiers.

### 6. Similarity Calculation

Each file is compared with every other file. Common fingerprints are counted and similarity percentage is calculated using:

Similarity = (Common Fingerprints / Maximum Fingerprints of both files) × 100

### 7. Output

Results are displayed and saved in a CSV file showing similarity percentages and detected plagiarism cases.

---

## How to Run

1. Clone the repository
2. Open the project in a C++ IDE (e.g., Visual Studio)
3. Compile and run the program
4. Check the output in `plagiarism_report.csv`

---

## Key Learning Outcomes

* Implementation of linked lists and queues without using STL
* Use of hashing for pattern matching
* Application of data structures in a real-world problem
* Understanding of string processing techniques

---


## Future Improvements

* Add graphical user interface
* Support additional file formats such as PDF and DOCX
* Optimize performance using advanced algorithms
* Improve accuracy using NLP techniques

---

## Contributors

This is a team project developed by:

* Tayyab Mehmood
* Zaib Saadat ([https://github.com/Zaib-saadat](https://github.com/Zaib-saadat))


