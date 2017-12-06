/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   FileName:UseEarleyParser.cpp
*   creator:yuliu1@microsoft.com
*   Time:11/03/2017
*   Description:
*   Notice: 
*   Updates:
*
================================================================*/
 
// please add your code here!
#include "EarleyParser.h"
#include<iostream>
int main(int argc, char **argv)
{
  std::vector<std::string> sent;
  sent.push_back("I");
  sent.push_back("saw");
  sent.push_back("a");
  sent.push_back("man");
  sent.push_back("in");
  sent.push_back("the");
  sent.push_back("apartment");
  std::vector<std::string> sentpos;
  sentpos.push_back("noun");
  sentpos.push_back("verb");
  sentpos.push_back("det");
  sentpos.push_back("noun");
  sentpos.push_back("prep");
  sentpos.push_back("det");
  sentpos.push_back("noun");
  sentpos.push_back("$");
  EarleyParser parser;
  parser.LoadVocabulary("Syntaticvocab.txt");
  parser.LoadRules("SyntaticRule.txt");
  std::cout << parser.EarleyParse(sentpos) << std::endl;
  
}
