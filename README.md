# Student Plagiarism Detection System
Screenshots / Sample Output 
• Menu Screen Output 
<img width="576" height="192" alt="image" src="https://github.com/user-attachments/assets/b24f2b58-8487-470a-9ce0-dd2111faad79" />
 • Progress Bar while Scanning 
 <img width="576" height="317" alt="image" src="https://github.com/user-attachments/assets/190c6d9c-0175-4c61-b68d-cdf6a8823ba0" />
Plagiarism Comparison Table 
<img width="576" height="237" alt="image" src="https://github.com/user-attachments/assets/5816d222-03ae-4ec0-add6-12cf8acb240c" />
• CSV Export Confirmation 
<img width="576" height="136" alt="image" src="https://github.com/user-attachments/assets/f8eac4c4-2532-4fb7-9928-f944f4acea8a" />
<img width="900" height="559" alt="image" src="https://github.com/user-attachments/assets/69963fc4-fdc1-4d66-a650-bb9ca47897bb" />
<img width="900" height="149" alt="image" src="https://github.com/user-attachments/assets/7871ce8d-61a8-4ec1-826b-b6deb36984c0" />










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


