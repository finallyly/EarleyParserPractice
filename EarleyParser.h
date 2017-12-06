/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   FileName:EarleyParser.h
*   creator:yuliu1@microsoft.com
*   Time:11/02/2017
*   Description:
*   Notice: 
*   Updates:
*
================================================================*/
#ifndef _EARLEYPARSER_H
#define _EARLEYPARSER_H
#include<map>
#include<set>
#include<string>
#include<vector>
#include<queue>
#include<utility>
struct Item
{
  std::string ruleText_;
  int oriPos_;
  int from_;
  int id_;
  // index of from state;
  Item()
  {
    ruleText_ = "";
    oriPos_ = 0;
    from_ = 0;
    id_ = 0;
  }
};
struct ItemSet
{
  std::vector<Item> itemList_;
  std::set<std::string> progressRuleSet_;
};
class EarleyParser
{

  public:
    void LoadVocabulary(const char*vocabFile);
    void LoadRules(const char *rulesFile);
    std::string GetHead(std::string ruleText);
    //bool IsRuleStartWith(std::string rule,std::string head);
    std::vector<std::string> GetRuleByHead(std::string head);
    std::string GetNextSymbol(std::string ruleText);
    bool IsNonTerminal(std::string symbol);
    bool IsTerminal(std::string symbol);
    bool Predict(std::vector<ItemSet>&states,std::queue<Item>& itemQueue,int curStateId,Item item,std::string head,int itemId);
    bool Scan(std::vector<ItemSet>&states,std::queue<Item>& itemQueue,int curStateId,Item item,std::string termSymbol,int itemId);
    bool Complete(std::vector<ItemSet>&states,std::queue<Item>& itemQueue,int curStateId, Item item, int itemId);
    std::string EarleyParse(std::vector<std::string> sentVec);
    void TraverseItemSet(std::vector<ItemSet>&states, std::vector<std::string>&sentVec, int index);
    std::vector<Item> GetFinishState(std::vector<Item> itemList);

    std::map<std::string, int> GetRules2Id()
    {
      return Rules2Id_;
    }

    std::map<int,std::string> GetId2Rules()
    {
      return Id2Rules_;
    }

    std::map<std::string,std::string>GetVocab()
    {
      return Vocab_;
    }

    std::set<std::string> GetTerminals()
    {
      return Terminals_;
    }

    std::set<std::string> GetNonterminals()
    {
      return Nonterminals_;
    }

  private:
    std::map<std::string,int> Rules2Id_;
    std::map<int, std::string> Id2Rules_;
    std::map<std::string, std::string> Vocab_;
    std::set<std::string> Terminals_;
    std::set<std::string> Nonterminals_;
};
#endif
