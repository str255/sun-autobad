/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
/*
 * Expression.c
 * Implementation of functions used to build the syntax tree.
 */

#include "expression.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Allocates space for expression
 * @return The expression or NULL if not enough memory
 */
static SExpression *allocateExpression()
{
  SExpression *b = (SExpression *)malloc(sizeof(SExpression));

  if (b == NULL)
    return NULL;

  b->type = eVALUE;
  b->value = 0;
  b->truth = 0;

  b->left = NULL;
  b->middle = NULL;
  b->right = NULL;

  return b;
}

SExpression *createNumber(double value)
{
  SExpression *b = allocateExpression();

  if (b == NULL)
    return NULL;

  b->type = eVALUE;
  b->value = value;
  b->truth = 1;

  return b;
}

SExpression *createVariable(char *cvalue)
{
  SExpression *b = allocateExpression();

  if (b == NULL)
    return NULL;

  b->type = eVARIABLE;
  printf("createVariable: cvalue %s\n", cvalue);
  //  strcpy(b->cvalue, cvalue); 
  b->cvalue = cvalue;
  b->value = 1;
  b->truth = 1;  

  return b;
}

SExpression *createOperation(EOperationType type, SExpression *left, SExpression *right)
{
  SExpression *b = allocateExpression();

  if (b == NULL)
    return NULL;

  b->type = type;
  b->left = left;
  b->right = right;
  b->truth = 0;

  return b;
}

SExpression *createUnaryOperation(EOperationType type, SExpression *right)
{
  SExpression *b = allocateExpression();

  if (b == NULL)
    return NULL;

  b->type = type;
  b->left = NULL;
  b->right = right;
  b->truth = 0;

  return b;
}

SExpression *createTriamory(EOperationType type, SExpression *left, SExpression *middle, SExpression *right)
{
  SExpression *b = allocateExpression();

  if (b == NULL)
    return NULL;

  b->type = type;
  b->left = left;
  b->middle = middle;
  b->right = right;
  b->truth = 0;

  return b;
}

void deleteExpression(SExpression *b)
{
  if (b == NULL)
    return;

  deleteExpression(b->left);
  if (b->middle) deleteExpression(b->middle);
  deleteExpression(b->right);

  free(b);
}
