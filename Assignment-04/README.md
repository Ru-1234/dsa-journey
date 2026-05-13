# Assignment 4 – CS2001 Data Structures

## Adaptive Indexing & Huffman Communication System

## 📌 Overview

This assignment contains **two independent systems**:

### Sender–Receiver Encoding System

A file compression and decompression system using **Huffman Coding** to efficiently encode text data and then reconstruct it back to its original form.

### Adaptive Indexing & Consistency Engine

A large-scale transaction processing system that maintains **multiple synchronized tree structures** (Binary Tree, BST, AVL Tree) with dynamic updates, recalibration, and anomaly handling.

---

# Question 1 – Sender & Receiver System

## Description

This system compresses and decompresses text files using **Huffman Coding**.

### Sender Program

* Reads input text file - `input.txt`
* Calculates character frequencies
* Builds Huffman Tree
* Assigns binary codes (0 = left, 1 = right)
* Encodes text into binary format
* Stores:

  * `output.txt` → encoded text
  * `codes.txt` → character codes

---

### Receiver Program

* Reads:

  * `output.txt`
  * `codes.txt`
* Decodes binary string back into original text
* Outputs result to:

  * `decoded.txt`

---

### Files

* `sender.cpp`
* `receiver.cpp`

---

### Key Concepts Used

* Huffman Tree Construction
* Greedy Algorithm
* Frequency Mapping
* Binary Encoding & Decoding

---

# Question 2 – Adaptive Indexing Engine

## Description

A large-scale transaction processing system built on **IEEE-CIS Fraud Dataset subset (50,000 rows)**.

It maintains three synchronized structures:

### 1. Binary Tree

* Stores transactions in **level-order insertion**

### 2. Binary Search Tree (BST)

* Indexed by **Transaction Amount**

### 3. AVL Tree

* Indexed by **Risk Score / Fraud Probability**
* Maintains balance using rotations

---

## Core Functionalities

### Streaming Ingestion

* Reads CSV line-by-line
* No bulk loading allowed
* Inserts into all 3 trees simultaneously

---

### Adaptive Risk Recalibration

* Dynamically updates risk formula
* AVL tree adjusts without full rebuild

---

### Disk-Based Reload

* Removes last K transactions
* Reloads from file pointer manually

---

### Anomaly Window Extraction

* Extracts transactions where amount ≥ X
* Builds BST → converts to AVL

---

### Drift Detection

* Detects structural mismatch between BST & AVL
* Fixes affected subtrees only

---

### Temporal Range Isolation

* Extracts nodes in time range [T1, T2]
* Builds separate AVL tree without duplication

---

### Consistency Verification

Ensures:

* BST in-order = sorted
* AVL is balanced
* Binary Tree preserves insertion order

---

### Memory Compression

* Compresses BST into minimal height tree
* Rebuilds AVL using rotations only

---

### Checkpointing

* Saves and reloads tree states manually (no libraries)

---

## Constraints Followed

* No STL usage
* No full recursion for core logic
* No array-based full dataset storage
* Manual tree rotations implemented
* File handling done manually

---

## Key Concepts Used

* Binary Trees
* BST Operations
* AVL Rotations
* File Streaming
* Consistency Algorithms
* Memory Optimization

---

## 🚀 How to Run

```bash
g++ sender.cpp -o sender
g++ receiver.cpp -o receiver
g++ q2.cpp -o engine
```

---

## 📌 Output Files Generated

* `output.txt`
* `codes.txt`
* `decoded.txt`

---
