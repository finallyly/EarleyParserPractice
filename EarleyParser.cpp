/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   FileName:EarleyParser.cpp
*   creator:yuliu1@microsoft.com
*   Time:11/02/2017
*   Description:
*   Notice: 
*   Updates:
*
================================================================*/
 
#include "EarleyParser.h"
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
// please add your code here!
std::vector<std::string> StringSplit(std::string sstr, const char* delim)
{
  std::vector<std::string> results;
  char *src = new char [sstr.length() + 1];
  strncpy(src,sstr.c_str(),sstr.length());
  src[sstr.length()] = 0;
  char *p = strtok(src,delim);
  if ( p!= NULL)
  {
    results.push_back(p);
  }
  while ( (p=strtok(NULL,delim)) != NULL )
  {
    results.push_back(p);
  }
  if (src != NULL )
  {
    delete [] src;
    src = NULL;
  }
  return results;
}

void EarleyParser::LoadVocabulary(const char* vocabFile)
{
  std::ifstream fin(vocabFile);
  std::string line = "";

  //load vocabulary
  while (std::getline(fin,line))
  {
    std::vector<std::string> lines = StringSplit(line,"\t");
    if (lines.size() != 2)
    {
      continue;
    }
    Vocab_.insert(std::make_pair(lines[0],lines[1]));
  }
  fin.close();

  //get terminals
  for (std::map<std::string, std::string>::iterator it = Vocab_.begin(); it != Vocab_.end(); it++)
  {
    Terminals_.insert(it->second);
  }
  /*
  for (std::set<std::string>::iterator it = Terminals_.begin(); it != Terminals_.end();it++)
  {
    std::cout<<*it<<std::endl;
  }
  */
}

void EarleyParser::LoadRules(const char* rulesFile)
{
  std::ifstream fin(rulesFile);
  std::string line = "";
  int id = 0;
  while(std::getline(fin,line))
  {
    std::vector<std::string> lines = StringSplit(line,":");
    if (lines.size() != 2)
    {
      continue;
    }
    Rules2Id_.insert(std::make_pair(line,id));
    Id2Rules_.insert(std::make_pair(id,line));
    id++;
    Nonterminals_.insert(lines[0]);
    std::vector<std::string> lines2 = StringSplit(lines[1],",");
    for ( std::vector<std::string>::iterator it = lines2.begin(); it != lines2.end(); it++)
    {
      if (!Terminals_.count(*it))
      {
        Nonterminals_.insert(*it);
      }
    }
  }
  fin.close();
  /*
  for (std::set<std::string>::iterator it = Nonterminals_.begin(); it != Nonterminals_.end();it++)
  {
    std::cout<<*it<<std::endl;
  }
  */
  /*
  for (std::map<std::string,int>::iterator it = Rules2Id_.begin(); it != Rules2Id_.end(); it++)
  {
    std::cout<<it->first<<"\t"<<it->second<<std::endl;
  }
  */
}

std::string EarleyParser::GetHead(std::string ruleText)
{
  std::vector<std::string> col = StringSplit(ruleText,":");
  return col[0]; 
}


std::vector<std::string> EarleyParser::GetRuleByHead(std::string head)
{
  std::vector<std::string> vecs;
  for ( std::map<std::string,int> ::iterator it = Rules2Id_.begin(); it != Rules2Id_.end();it++ )
  {
    std::vector<std::string> cols = StringSplit(it->first,":");
    if (cols[0] == head) 
    { 
      vecs.push_back(it->first); 
    } 
  } 
  return vecs;; 
}

//return the symbol after DOTT, if DOTT is the last element return empty string 
std::string EarleyParser::GetNextSymbol(std::string ruleText)
{
  std::string nextSymbol = "";
  std::vector<std::string> colRule = StringSplit(ruleText,":");
  std::vector<std::string> colBody = StringSplit(colRule[1],",");
  int pos = colBody.size() - 1;
  for ( int i = 0; i < colBody.size(); i++)
  {
    if (colBody[i] == "DOTT")
    {
      pos = i;
    }
  }
  if (pos < colBody.size() - 1)
  {
    nextSymbol = colBody[pos + 1];
  }
  return nextSymbol;
}

bool EarleyParser::IsNonTerminal(std::string symbol)
{
  if (Nonterminals_.count(symbol))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool EarleyParser::IsTerminal(std::string symbol)
{
  if (Terminals_.count(symbol))
  {
    return true;
  }
  else
  {
    return false;
  }
}
//PREDICT 
bool EarleyParser::Predict(std::vector<ItemSet>&states,std::queue<Item>& itemQueue, int curStateId, Item item, std::string head,int itemId)
{
  bool returnFlag = false;
  std::vector<std::string> rulesVec = GetRuleByHead(head);
  int id = itemId;
  int prevItemId = item.id_;
  for (std::vector<std::string>::iterator it = rulesVec.begin(); it != rulesVec.end();it++)
  {
    std::vector<std::string> vecAll = StringSplit(*it,":");
    std::vector<std::string> vecBody = StringSplit(vecAll[1],",");
    std::string newRule = head + ":"+"DOTT";
    for (std::vector<std::string>:: iterator subit = vecBody.begin(); subit != vecBody.end(); subit++)
    {
      newRule += ",";
      newRule += *subit;
    }
    if ( states[curStateId].progressRuleSet_.count(newRule) == 0 )
    {
      Item item;
      item.ruleText_ = newRule;
      item.oriPos_ = curStateId;
      item.from_ = prevItemId;
      item.id_ = id++;
      states[curStateId].itemList_.push_back(item);
      states[curStateId].progressRuleSet_.insert(newRule);
      itemQueue.push(item);
      returnFlag = true;
    }
  }
  return returnFlag;
}

bool EarleyParser::Scan(std::vector<ItemSet>&states, std::queue<Item> &itemQueue,int curStateId, Item item, std::string termSymbol,int itemId)
{
  std::vector<std::string> vecAll = StringSplit(item.ruleText_,":");
  std::vector<std::string> vecBody = StringSplit(vecAll[1],",");
  std::string newRule = vecAll[0]+":";
  int count = 0;
  int pos = item.oriPos_;
  int prevItemId = item.id_;
  // tell if the current rule has corresponding termSymbol
  bool returnFlag = false;
  for (std::vector<std::string>:: iterator it = vecBody.begin(); it != vecBody.end(); it++)
  {
     if ( *it != "DOTT" )
     {
       if ( count != 0 )
       {
        newRule += ",";
       }
       newRule += *it;
       count++;
     }
     if ( *it == termSymbol )
     {
       newRule += ",";
       newRule += "DOTT";
       count++;
       returnFlag = true;
     }
  }
  if ( returnFlag &&states[curStateId].progressRuleSet_.count(newRule) == 0 )
  {
      Item item;
      item.ruleText_ = newRule;
      item.oriPos_ = pos;
      item.id_ = itemId++;
      item.from_ = prevItemId;
      states[curStateId+1].itemList_.push_back(item);
      states[curStateId+1].progressRuleSet_.insert(newRule);
  }
  return returnFlag;
}

bool EarleyParser::Complete(std::vector<ItemSet>&states,std::queue<Item>&itemQueue,int curStateId, Item item, int itemId)
{
    std::string ruleText = item.ruleText_;
    int prevStateId = item.oriPos_;
    std::vector<std::string> vecAll = StringSplit(ruleText,":");
    std::string head = vecAll[0];
    bool returnFlag = false;
    int id = itemId;
    for ( std::vector<Item>::iterator it = states[prevStateId].itemList_.begin(); it != states[prevStateId].itemList_.end(); it++ )
    {
      std::string nextSymbol = GetNextSymbol(it->ruleText_);
      //std::cout<<"traverse"<<it->ruleText_<< std::endl;
      if ( nextSymbol == head )
      {
          ruleText =  it->ruleText_;
          vecAll = StringSplit(ruleText,":");
          std::vector<std::string> vecBody = StringSplit(vecAll[1],",");
          std::string newRule = vecAll[0] + ":";
          int count = 0;
          for ( std::vector<std::string>::iterator subit = vecBody.begin(); subit != vecBody.end(); subit++ )
          {
              if ( *subit != "DOTT" ) 
              {
                if (count != 0)
                {
                  newRule += ",";
                }
                newRule += *subit;
                count++;
              }
              if ( *subit == head )
              {
                  newRule += ",";
                  newRule += "DOTT";
                  count++;
              }
          }
          if ( states[curStateId].progressRuleSet_.count(newRule) == 0 )
          { 
              Item item;
              item.ruleText_ = newRule;
              item.oriPos_ = it->oriPos_;
              item.from_ = it->from_;
              item.id_ = id++;
              states[curStateId].itemList_.push_back(item);
              states[curStateId].progressRuleSet_.insert(newRule);
              itemQueue.push(item);
              returnFlag = true;
          }
      }
       
    }
    return returnFlag;
}

void EarleyParser::TraverseItemSet(std::vector<ItemSet>&states, std::vector<std::string>&sentVec, int index)
{
    std::queue<Item> que;
    int count = 0;
    for ( std::vector<Item>::iterator it = states[index].itemList_.begin(); it != states[index].itemList_.end(); it++ )
    {
        que.push(*it);
    }
    while(!que.empty())
    {
      Item item = que.front();
      que.pop();
      std::string rule = item.ruleText_;
      std::string nextSymbol = GetNextSymbol(item.ruleText_);
      if (IsNonTerminal(nextSymbol))
      {
        count = states[index].itemList_.size(); 
        Predict(states,que,index,item,nextSymbol,count);
      }
      else if(IsTerminal(nextSymbol))
      {
         if (nextSymbol == sentVec[index])
         {
           count = states[index+1].itemList_.size(); 
           Scan(states,que,index,item,nextSymbol,count);
         }
      }
      else
      {
        count = states[index].itemList_.size(); 
        Complete(states,que,index,item,count);
      }
      count++;
    }
}


std::string EarleyParser::EarleyParse(std::vector<std::string> sentVec)
{
  std::vector<ItemSet> states;
  states.resize(sentVec.size());
  std::string rule="Start:DOTT,S";
  Item item;
  item.ruleText_ = rule;
  item.oriPos_ = 0;
  states[0].itemList_.push_back(item);
  states[0].progressRuleSet_.insert(rule);
  std::string result = "";

  for ( int i = 0; i < sentVec.size(); i++ )
  {
    TraverseItemSet(states,sentVec,i);
  }
  for (int myj=0; myj< states.size();myj++)
  {
    std::cout<<"now is the state"<<myj<<std::endl;
    for (int i = 0; i < states[myj].itemList_.size();i++)
    {
	    std::cout<<states[myj].itemList_[i].ruleText_<<"\t"<<states[myj].itemList_[i].oriPos_<<"\t"<<states[myj].itemList_[i].from_<<"\t"<<states[myj].itemList_[i].id_ << std::endl;
    }
  }
  std::string path="";
  std::vector<Item> results = GetFinishState(states[sentVec.size()-1].itemList_);
  if (results.size() == 0)
  {
    path = "bad";
  }
  else
  {
    path="good";
  }
  return path;
}

std::vector<Item>EarleyParser:: GetFinishState(std::vector<Item> itemList)
{
    std::vector<Item> results;
    for (std::vector<Item>::iterator it = itemList.begin(); it != itemList.end(); it++)
    {
      if (it->ruleText_=="Start:S,DOTT")
      {
        results.push_back(*it);
      }
    }
    return results;
}
