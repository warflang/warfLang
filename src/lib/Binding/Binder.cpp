// Copyright (c) 2022 F. Lotfi All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "Binder.h"
#include "BoundAssignmentExpressionNode.h"
#include "BoundBinaryExpressionNode.h"
#include "BoundIdentifierExpressionNode.h"
#include "BoundLiteralExpressionNode.h"
#include "BoundUnaryExpressionNode.h"
#include "Syntax/AssignmentExpressionNode.h"
#include "Syntax/BinaryExpressionNode.h"
#include "Syntax/IdentifierExpressionNode.h"
#include "Syntax/LiteralExpressionNode.h"
#include "Syntax/ParenthesizedExpressionNode.h"
#include "Syntax/UnaryExpressionNode.h"

#include "Symbol/SymbolTableMgr.h"
#include "Symbol/VariableSymbol.h"

#include <sstream>

std::unique_ptr<BoundExpressionNode>
Binder::BindExpression(ExpressionNode *node) {

  if (LiteralExpressionNode *literal =
          dynamic_cast<LiteralExpressionNode *>(node)) {
    return std::move(BindLiteralExpression(literal));
  }
  if (UnaryExpressionNode *unaryExpression =
          dynamic_cast<UnaryExpressionNode *>(node)) {
    return std::move(BindUnaryExpression(unaryExpression));
  }
  if (BinaryExpressionNode *binaryExpression =
          dynamic_cast<BinaryExpressionNode *>(node)) {
    return std::move(BindBinaryExpression(binaryExpression));
  }
  if (ParenthesizedExpressionNode *parenthesizedExpression =
          dynamic_cast<ParenthesizedExpressionNode *>(node)) {
    return std::move(BindExpression(parenthesizedExpression->Expression()));
  }
  if (AssignmentExpressionNode *assignmentExpression =
          dynamic_cast<AssignmentExpressionNode *>(node)) {
    return std::move(BindAssignmentExpression(assignmentExpression));
  }
  if (IdentifierExpressionNode *identifierExpression =
          dynamic_cast<IdentifierExpressionNode *>(node)) {
    return std::move(BindIdentifierExpression(identifierExpression));
  }
  std::stringstream diagmsg;
  diagmsg << "Unexpected syntax " << SyntaxTokenToStrMap.at(node->Type());
  mDiagnostics.push_back(diagmsg.str());
  throw diagmsg.str();
  return nullptr;
}

std::unique_ptr<BoundExpressionNode>
Binder::BindLiteralExpression(LiteralExpressionNode *literal) {
  return std::make_unique<BoundLiteralExpressionNode>(literal);
}

std::unique_ptr<BoundExpressionNode>
Binder::BindUnaryExpression(UnaryExpressionNode *unary) {
  auto boundOperand = BindExpression(unary->Operand());
  const std::shared_ptr<BoundUnaryOperator> boundOperator =
      BoundUnaryOperator::Bind(unary->Operator()->Type(),
                               boundOperand->GetType());
  if (boundOperator == BoundUnaryOperator::GetBindFailure()) {

    std::stringstream diagmsg;
    diagmsg << "Unary operator "
            << SyntaxTokenToStrMap.at(unary->Operator()->Type())
            << " is not defined for type " << boundOperand->GetType() << ".";
    mDiagnostics.push_back(diagmsg.str());
    return boundOperand;
  }
  return std::make_unique<BoundUnaryExpressionNode>(boundOperator,
                                                    std::move(boundOperand));
}

std::unique_ptr<BoundExpressionNode>
Binder::BindBinaryExpression(BinaryExpressionNode *binary) {
  auto boundLeft = BindExpression(binary->Left());
  auto boundRight = BindExpression(binary->Right());
  const std::shared_ptr<BoundBinaryOperator> boundOperator =
      BoundBinaryOperator::Bind(binary->Operator()->Type(),
                                boundLeft->GetType(), boundRight->GetType());
  if (boundOperator == BoundBinaryOperator::GetBindFailure()) {
    std::stringstream diagmsg;
    diagmsg << "Binary operator " << binary->Operator()->Text()
            << " is not defined for types " << boundLeft->GetType() << " and "
            << boundRight->GetType() << "." << std::endl;
    mDiagnostics.push_back(diagmsg.str());
    return boundLeft;
  }
  return std::make_unique<BoundBinaryExpressionNode>(
      std::move(boundLeft), boundOperator, std::move(boundRight));
}

std::unique_ptr<BoundExpressionNode>
Binder::BindAssignmentExpression(AssignmentExpressionNode *assignment) {
  std::string name = assignment->IdentifierToken()->Text();
  auto boundExpression = BindExpression(assignment->Expression());
  auto newVar =
      std::make_shared<VariableSymbol>(name, boundExpression->GetType());
  auto existingVariable = SymbolTableMgr::find(name);
  if (existingVariable != VariableSymbol::failSymbol()) {
    SymbolTableMgr::modify(newVar, existingVariable->GetScopeName());
  } else {
    SymbolTableMgr::insert(newVar);
  }

  return std::make_unique<BoundAssignmentExpressionNode>(
      newVar, std::move(boundExpression));
}

std::unique_ptr<BoundExpressionNode>
Binder::BindIdentifierExpression(IdentifierExpressionNode *identifier) {
  std::string name = identifier->IdentifierToken()->Text();
  std::shared_ptr<VariableSymbol> variable = SymbolTableMgr::find(name);
  if (variable == VariableSymbol::failSymbol()) {
    std::stringstream diagmsg;
    diagmsg << "Undefined name: " << name << "Starting at Position "
            << identifier->IdentifierToken()->Position() << " Ending at: "
            << identifier->IdentifierToken()->Position() + name.size() << "."
            << std::endl;
    mDiagnostics.push_back(diagmsg.str());
    return std::make_unique<BoundLiteralExpressionNode>(0);
  }

  return std::make_unique<BoundIdentifierExpressionNode>(variable);
}