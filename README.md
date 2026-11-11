# Parallel K-Means Clustering

## 1. Project Overview

This project implements the K-Means Clustering algorithm using three approaches:

- **Sequential:** Processes data points one by one using a single thread.
- **OpenMP:** Uses multithreading to parallelise the clustering process.
- **P-Threads:** Uses POSIX threads for parallel processing.

The project compares the execution performance of sequential and parallel implementations using execution time, speedup, and efficiency.

## 2. Project Structure

```text
CST435-Ass1GitHub/
│
├── OpenMP/
├── P-Threads/
├── Sequential/
├── datasets/
│   └── sample_dataset_50000_4.txt
│
├── Analysis.pdf
├── LICENSE
├── README.md
└── Sample_Visualization(K=3).png
```

## 3. Requirements

The following software is required:

- Windows 10/11
- MSYS2 UCRT64
- GCC C++ Compiler (g++)
- OpenMP support

### Install GCC using MSYS2

1. Install MSYS2 from https://www.msys2.org/
2. Open the **MSYS2 UCRT64** terminal.
3. Update the package database if required:

```bash
pacman -Syu
```

4. Install the GCC compiler:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc
```

5. Check the installation:

```bash
g++ --version
```

## 4. Dataset Information

The sample dataset is stored in the `datasets` folder.

**Dataset:** `sample_dataset_50000_4.txt`

- Number of data points: 50,000
- Dimensions: 3 (X, Y, Z)
- First line: Number of data points
- Remaining lines: Coordinates of each data point

## 5. Compilation and Execution

Open the MSYS2 UCRT64 terminal and navigate to the project folder.

```bash
cd /c/Users/YourUsername/Desktop/CST435-Ass1GitHub
```

Replace `YourUsername` with your Windows username.

### A. Sequential Implementation

Navigate to the Sequential folder:

```bash
cd Sequential
```

Compile:

```bash
g++ main_sequential.c lab1_io.c Kmeans-Sequential.cpp -fopenmp -o seq.out
```

Execute:

```bash
./seq.out 4 ../datasets/sample_dataset_50000_4.txt b.txt c.txt
```

Arguments:

- `4`: Number of clusters (K).
- Dataset path: Input dataset.
- `b.txt`: Output file containing cluster assignments.
- `c.txt`: Output file containing centroid history.

### B. OpenMP Implementation

Navigate to the OpenMP folder:

```bash
cd ../OpenMP
```

Compile:

```bash
g++ -I../Sequential main_omp.c lab1_io.c Kmeans-OpenMP.cpp -fopenmp -o omp.out
```

Execute:

```bash
./omp.out 4 2 ../datasets/sample_dataset_50000_4.txt b.txt c.txt
```

Arguments:

- `4`: Number of clusters (K).
- `2`: Number of threads.
- Dataset path: Input dataset.
- `b.txt`: Output file containing cluster assignments.
- `c.txt`: Output file containing centroid history.

The number of clusters and threads can be changed.

### C. P-Threads Implementation

The P-Threads implementation is retained from the original project. Its Windows compilation and execution instructions have not yet been verified in the current environment.

## 6. Output Files

Each implementation produces two output files:

| File | Description |
|---|---|
| `b.txt` | Cluster assignment for each data point |
| `c.txt` | Centroid coordinates recorded during iterations |

## 7. Performance Evaluation

The execution time is measured using `omp_get_wtime()`.

The performance of the implementations can be evaluated using:

**Speedup**

Speedup = Sequential Execution Time / Parallel Execution Time

**Efficiency**

Efficiency = (Speedup / Number of Threads) × 100%

For a fair comparison, use the same dataset, number of clusters, and experimental conditions.

## 8. Original Project and Acknowledgement

This project is based on the original implementation by **vinayak1998**:

https://github.com/vinayak1998/Parallel-K-Means-Clustering

The original project was modified to fix issues in centroid calculation, correct centroid iteration counting, and improve execution-time measurement.

The original MIT licence is retained.
