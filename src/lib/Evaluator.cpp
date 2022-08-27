// Copyright (c) 2022 F. Lotfi All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Evaluator.h"
#include "Binding/BoundAssignmentExpressionNode.h"
#include "Binding/BoundBinaryExpressionNode.h"
#include "Binding/BoundIdentifierExpressionNode.h"
#include "Binding/BoundLiteralExpressionNode.h"
#include "Binding/BoundUnaryExpressionNode.h"

std::unordered_map<std::string, Value> Evaluator::variables;

BoundExpressionNode *Evaluator::Root() const { return mRootExpression.get(); }
Value Evaluator::Evaluate() { return EvaluateRec(mRootExpression.get()); }

Evaluator::Evaluator(std::unique_ptr<BoundExpressionNode> root)
    : mRootExpression(std::move(root)) {}

Value Evaluator::EvaluateRec(BoundExpressionNode *node) {
  if (BoundLiteralExpressionNode *literal =
          dynamic_cast<BoundLiteralExpressionNode *>(node)) {
    return literal->GetValue();
  }
  if (BoundIdentifierExpressionNode *identifierExpression =
          dynamic_cast<BoundIdentifierExpressionNode *>(node)) {
    std::string name = identifierExpression->Name();
    if (Evaluator::variables.count(name) > 0) {
      return variables[name];
    } else {
      return Value();
    }
  }
  if (BoundAssignmentExpressionNode *assignmentExpression =
          dynamic_cast<BoundAssignmentExpressionNode *>(node)) {
    auto rightSide = EvaluateRec(assignmentExpression->BoundExpression());
    Evaluator::variables[assignmentExpression->Identifier()] = rightSide;
    return rightSide;
  }
  if (BoundUnaryExpressionNode *unaryExpression =
          dynamic_cast<BoundUnaryExpressionNode *>(node)) {
    auto opType = unaryExpression->OperatorType();
    auto operand = EvaluateRec(unaryExpression->Operand());
    switch (opType) {
    case BoundUnaryOperatorType::Identity:
      return operand;
    case BoundUnaryOperatorType::Negation:
      return -operand;
    case BoundUnaryOperatorType::LogicalNegation:
      return !operand;
    default:
      mVecErrors.push_back("EvaluatorError: Unexpected unary operator: " +
                           BoundUnaryTypeStrMap.at(opType));
    }
  }
  if (BoundBinaryExpressionNode *binaryExpression =
          dynamic_cast<BoundBinaryExpressionNode *>(node)) {
    auto left = EvaluateRec(binaryExpression->Left());
    auto right = EvaluateRec(binaryExpression->Right());
    auto opType = binaryExpression->OperatorType();
    switch (opType) {
    case BoundBinaryOperatorType ::Addition:
      return left + right;
    case BoundBinaryOperatorType ::Subtraction:
      return left - right;
    case BoundBinaryOperatorType ::Multiplication:
      return left * right;
    case BoundBinaryOperatorType ::Division:
      return left / right;
    case BoundBinaryOperatorType::Equals:
      return left == right;
    case BoundBinaryOperatorType::NotEquals:
      return left != right;
    case BoundBinaryOperatorType::LogicalAnd:
      return left && right;
    case BoundBinaryOperatorType::LogicalOr:
      return left || right;
    default:
      mVecErrors.push_back("EvaluatorError: Unexpected binary operator: " +
                           BoundBinaryTypeStrMap.at(opType));
      return Value();
    }
  }
  return Value();
}