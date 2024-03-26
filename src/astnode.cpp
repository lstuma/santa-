#include <iomanip>
#include <iostream>
#include <cassert>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string_view>
#include <sstream>
#include <list>
#include <array>
#include <stack>

#include "color.h"
#include "astnode.h"

ASTNode::ASTNode() {
}

ASTNode::ASTNode(ast_type type) {
  this->type = type;
}

ASTNode::ASTNode(ast_type type, Token token) {
  this->type = type;
  this->token = token;
}

ASTNode::ASTNode(ast_type type, Token token, ASTNode* parent) {
  this->type = type;
  this->token = token;
  this->parent = parent;
}

ASTNode::ASTNode(ast_type type, Token token, std::list<ASTNode> children) {
  this->type = type;
  this->token = token;
  this->children = children;
}

// get child at position i
ASTNode ASTNode::get(int i) {
  auto it = this->children.begin();
  advance(it, i);
  return *it;
}

int ASTNode::childcount() {
  return this->children.size();
}

ASTNode& ASTNode::operator[](std::size_t i) {
  auto it = this->children.begin();
  advance(it, i);
  return *it;
}
    
// add child
ASTNode ASTNode::append(ASTNode child) {
  child.parent = this;
  this->children.push_back(child);
  return child;
}

// inserts child at index i
ASTNode ASTNode::insert(int i, ASTNode child) {
  child.parent = this;
  auto it = this->children.begin();
  advance(it, i);
  this->children.insert(it, child);
  return child;
}


// remove child at end
ASTNode ASTNode::pop() {
  ASTNode last = get(childcount()-1);
  this->children.pop_back();
  return last;
}

// remove child at index i
ASTNode ASTNode::pop(int i) {
  auto it = this->children.begin();
  advance(it, i);
  ASTNode elem = this->get(i);
  this->children.erase(it);
  return elem;
}

std::string ASTNode::to_string() {
  return to_string(0);
}

std::string ASTNode::to_string(int depth) {
  std::string s = "";
  for(int i = 0; i < depth-1; i++) s += "│  ";
  if(depth>0) s += "├─ ";
  s += Color::bhblue + std::to_string(this->type) + Color::reset;
  if(this->type==ast_terminal) s += " " + this->token.to_string();
  s += "\n";
  for(ASTNode child: this->children) s+=child.to_string(depth+1);
  return s;
}