/*
 * Copyright (c) 2022 F. Lotfi All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#pragma once
#include "Error/Record.h"
#include "SyntaxToken.h"
#include <memory>
#include <string>
#include <vector>

class Lexer {
private:
  std::string mText;
  std::string mIdentifier;
  int32_t mPosition;
  Value mValue;
  Records mRecords;
  void ReadToken(SyntaxType &type);
  void ParseNumber(SyntaxType &type);
  void ParseLetters(SyntaxType &type);
  void ReadIdentifierOrKeyword(SyntaxType &type);

public:
  Lexer(std::string text);
  char CurrentToken();
  std::unique_ptr<SyntaxToken> NextToken();
  const Records &Errors() const { return mRecords; }
};