/*
 * Copyright (c) 2022 F. Lotfi All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#pragma once
#include "SyntaxNode.h"

class ExpressionNode : public SyntaxNode {
public:
  ExpressionNode(SyntaxType type);
  virtual ~ExpressionNode() {}
};