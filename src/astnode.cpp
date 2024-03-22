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

#include "astnode.h"


ASTNode::ASTNode() {
}

ASTNode::ASTNode(ast_type type) {
  this->type = type;
}

ASTNode::ASTNode(ast_type type, Token* token) {
  this->type = type;
  this->token = token;
}

ASTNode::ASTNode(ast_type type, Token* token, std::list<ASTNode> children) {
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
void ASTNode::append(ASTNode child) {
  this->children.push_back(child);
}

// inserts child at index i
void ASTNode::insert(int i, ASTNode child) {
  auto it = this->children.begin();
  advance(it, i);
  this->children.insert(it, child);
}


// remove child at end
ASTNode ASTNode::pop() {
  ASTNode last = get(childcount());
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