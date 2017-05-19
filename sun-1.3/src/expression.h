/* Copyright 2017 Nicholas C. Strauss (strauss@positive-internet.com)
   This program is distributed under the terms of the GNU General Public License.
   See File COPYING for full details. 
*/
/*
 * Expression.h
 * Definition of the structure used to build the syntax tree.
 */
#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

typedef enum tagEOperationType
  {
    eVALUE,
    eVARIABLE,    
    eLESS,
    eMORE,
    eAND,
    eOR,
    ePLUS,
    eMINUS,
    eTIMES,
    eDIVISION,
  } EOperationType;

typedef struct tagSExpression
{
  EOperationType type;///< type of operation

  bool      truth;
  char     *cvalue;
  double    value;
  struct tagSExpression *left; ///< left side of the tree
  struct tagSExpression *middle; // middle tree (not used often)
  struct tagSExpression *right;///< right side of the tree
} SExpression;


SExpression *createNumber(double value);
SExpression *createVariable(char *cvalue);
SExpression *createOperation(EOperationType type, SExpression *left, SExpression *right);
SExpression *createUnaryOperation(EOperationType type, SExpression *right);
SExpression *createTriamory(EOperationType type, SExpression *left, SExpression *middle, SExpression *right);
void deleteExpression(SExpression *b);

#endif // __EXPRESSION_H__
