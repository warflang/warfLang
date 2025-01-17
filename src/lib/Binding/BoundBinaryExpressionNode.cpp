// Copyright (c) 2022 F. Lotfi All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "BoundBinaryExpressionNode.h"

const std::shared_ptr<BoundBinaryOperator> BoundBinaryOperator::sOperators[] = {
    std::make_shared<BoundBinaryOperator>(SyntaxType::UnknownToken,
                                          BoundBinaryOperatorType::Addition,
                                          Type::Unknown),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::PlusToken, BoundBinaryOperatorType::Addition, Type::Number),
    std::make_shared<BoundBinaryOperator>(SyntaxType::MinusToken,
                                          BoundBinaryOperatorType::Subtraction,
                                          Type::Number),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::StarToken, BoundBinaryOperatorType::Multiplication,
        Type::Number),
    std::make_shared<BoundBinaryOperator>(SyntaxType::SlashToken,
                                          BoundBinaryOperatorType::Division,
                                          Type::Number),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::EqualsEqualsToken, BoundBinaryOperatorType::Equals,
        Type::Number, Type::Number, Type::Boolean),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::EqualsEqualsToken, BoundBinaryOperatorType::Equals,
        Type::Boolean, Type::Boolean, Type::Boolean),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::BangEqualsToken, BoundBinaryOperatorType::NotEquals,
        Type::Number, Type::Number, Type::Boolean),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::BangEqualsToken, BoundBinaryOperatorType::NotEquals,
        Type::Boolean, Type::Boolean, Type::Boolean),
    std::make_shared<BoundBinaryOperator>(SyntaxType::AmpersandAmpersandToken,
                                          BoundBinaryOperatorType::LogicalAnd,
                                          Type::Boolean),
    std::make_shared<BoundBinaryOperator>(SyntaxType::PipePipeToken,
                                          BoundBinaryOperatorType::LogicalOr,
                                          Type::Boolean),

    std::make_shared<BoundBinaryOperator>(SyntaxType::AmpersandToken,
                                          BoundBinaryOperatorType::BitwiseAnd,
                                          Type::Number),
    std::make_shared<BoundBinaryOperator>(SyntaxType::PipeToken,
                                          BoundBinaryOperatorType::BitwiseOr,
                                          Type::Number),
    std::make_shared<BoundBinaryOperator>(SyntaxType::HatToken,
                                          BoundBinaryOperatorType::BitwiseXor,
                                          Type::Number),

    std::make_shared<BoundBinaryOperator>(SyntaxType::GreaterToken,
                                          BoundBinaryOperatorType::GreaterThan,
                                          Type::Number),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::GreaterOrEqualsToken,
        BoundBinaryOperatorType::GreaterThanOrEqualTo, Type::Number),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::LessToken, BoundBinaryOperatorType::LessThan, Type::Number),
    std::make_shared<BoundBinaryOperator>(
        SyntaxType::LessOrEqualsToken,
        BoundBinaryOperatorType::LessThanOrEqualTo, Type::Number),
};

BoundBinaryOperator::BoundBinaryOperator(SyntaxType syntaxType,
                                         BoundBinaryOperatorType boundType,
                                         Type type)
    : mSyntaxType(syntaxType), mBoundType(boundType), mLeftOperandType(type),
      mRightOperandType(type), mEvalType(type) {}

BoundBinaryOperator::BoundBinaryOperator(SyntaxType syntaxType,
                                         BoundBinaryOperatorType boundType,
                                         Type leftOperandType,
                                         Type rightOperandType, Type evalType)
    : mSyntaxType(syntaxType), mBoundType(boundType),
      mLeftOperandType(leftOperandType), mRightOperandType(rightOperandType),
      mEvalType(evalType) {}

const std::shared_ptr<BoundBinaryOperator>
BoundBinaryOperator::GetBindFailure() {
  return sOperators[0];
}

const std::shared_ptr<BoundBinaryOperator>
BoundBinaryOperator::Bind(SyntaxType syntaxType, Type leftOperandType,
                          Type rightOperandType) {
  for (std::shared_ptr<BoundBinaryOperator> op :
       BoundBinaryOperator::sOperators) {
    if (op->GetSyntaxType() == syntaxType &&
        op->LeftOperandType() == leftOperandType &&
        op->RightOperandType() == rightOperandType) {
      return op;
    }
  }
  return GetBindFailure();
}

SyntaxType BoundBinaryOperator::GetSyntaxType() { return mSyntaxType; }

BoundBinaryOperatorType BoundBinaryOperator::BoundType() { return mBoundType; }
Type BoundBinaryOperator::LeftOperandType() { return mLeftOperandType; }

Type BoundBinaryOperator::RightOperandType() { return mRightOperandType; }

Type BoundBinaryOperator::GetType() { return mEvalType; }

BoundBinaryExpressionNode::BoundBinaryExpressionNode(
    std::unique_ptr<BoundExpressionNode> left,
    const std::shared_ptr<BoundBinaryOperator> op,
    std::unique_ptr<BoundExpressionNode> right)
    : BoundExpressionNode(), mLeft(std::move(left)), mOperator(op),
      mRight(std::move(right)) {}

BoundNodeType BoundBinaryExpressionNode::NodeType() {
  return BoundNodeType::BinaryExpression;
}

Type BoundBinaryExpressionNode::GetType() { return mOperator->GetType(); }

BoundExpressionNode *BoundBinaryExpressionNode::Left() { return mLeft.get(); }

BoundExpressionNode *BoundBinaryExpressionNode::Right() { return mRight.get(); }

BoundBinaryOperatorType BoundBinaryExpressionNode::OperatorType() {
  return mOperator->BoundType();
}