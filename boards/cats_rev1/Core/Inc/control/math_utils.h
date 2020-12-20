/*
 * math_utils.h
 *
 *  Created on: Dec 15, 2020
 *      Author: Jonas
 */

#ifndef INC_CONTROL_MATH_UTILS_H_
#define INC_CONTROL_MATH_UTILS_H_

#include "cmsis_os.h"
#include "string.h"



#endif /* INC_CONTROL_MATH_UTILS_H_ */

/* Creates the Identity Matrix of size dim */
void eye(int dim, float A[dim][dim]);

/* Calculates the transpose of a Matrix */
void transpose(int m, int n, float A[m][n], float A_T[n][m]);

/* Calculates the addition of two vectors */
void vecadd(int n, float a[n], float b[n], float c[n]);

/* Calculates the subtraction of two vectors */
void vecsub(int n, float a[n], float b[n], float c[n]);

/* Calculates the addition of two matrices */
void matadd(int m, int n, float A[m][n], float B[m][n], float C[m][n]);

/* Calculates the subtraction of two matrices */
void matsub(int m, int n, float A[m][n], float B[m][n], float C[m][n]);

/* Calculates the matrix multiplication of two matrices */
void matmul(int n, int m, int o, float A[n][m], float B[m][o], float C[n][o]);

/* Calculates the product of a matrix and a vector */
void matvecprod(int m, int n, float A[m][n], float b[n], float c[m]);

/* Function to get cofactor of A[p][q] in temp[][]. n is current dimension of A[][] */
void cofactor(int dim, float A[dim][dim], float temp[dim][dim], int p, int q, int n);

/* Recursive function for finding determinant of matrix. n is current dimension of A[][]. */
float determinant(int dim, float A[dim][dim], int n);

/* Function to get adjoint of A[dim][dim] in adj[dim][dim]. */
void adjoint(int dim, float A[dim][dim], float adj[dim][dim]);

/* Function to calculate and store inverse, returns false if matrix is singular */
osStatus_t inverse(int dim, float A[dim][dim], float inverse[dim][dim], float lambda);
