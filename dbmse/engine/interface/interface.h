// Toy DBMS, v0.4
// George Chernishev (c) 2016-2017, chernishev<at>google mail
// A task framework for undergraduate students at Saint-Petersburg Academic University, DBMS development course
// More details regarding the course can be found here: www.math.spbu.ru/user/chernishev/
// CHANGELOG:
// 0.4: no chance for efficiency competition, so, this year I reoriented task towards ideas:
//      1) tried to remove C code, now we are using lots of std:: stuff
//      2) implemented support for multiple attributes in the DBMS
//      3) code clean-up and restructurization
// 0.3: added:
//      1) support for restricting physical join node size
//      2) support for deduplication node, LUniqueNode
//      3) print methods for Predicate and BaseTable
//      updated:
//      1) new format for data files: third line is the sort status now
//      2) added projection code
//      3) contract contains print methods for physical and logical nodes
// 0.2: first public release

#ifndef INTERFACE_H
#define INTERFACE_H
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "basics.h"

/* Logical nodes (query) */

class LAbstractNode{
  public:
    LAbstractNode(LAbstractNode* left, LAbstractNode* right);
    virtual ~LAbstractNode();
    LAbstractNode* GetLeft();
    LAbstractNode* GetRight();
    // schema-related info
    std::vector<std::vector<std::string>> fieldNames;
    std::vector<ValueType> fieldTypes;
    std::vector<COLUMN_SORT> fieldOrders;
  protected:
    LAbstractNode* left;
    LAbstractNode* rigth;
};

class LCrossProductNode : public LAbstractNode{
  public:
    LCrossProductNode(LAbstractNode* left, LAbstractNode* right);
    ~LCrossProductNode();
};

class LJoinNode : public LAbstractNode{
  public:
    // offsets are defined as "TableName.AttributeName" so, ensure there is no duplicates
    LJoinNode(LAbstractNode* left, LAbstractNode* right, std::string offset1, std::string offset2, int memorylimit);
    ~LJoinNode();
    // attributes to perform equi-join on
    std::string offset1, offset2;
    // maximum number of records permitted to present inside physical node
    int memorylimit;
};

class LProjectNode : public LAbstractNode{
  public:
    // offsets to keep
    LProjectNode(LAbstractNode* child, std::vector<std::string> tokeep);
    ~LProjectNode();
    // offsets are defined as "TableName.AttributeName" so, ensure there is no duplicates
    std::vector<std::string> offsets;
};

class LSelectNode : public LAbstractNode{
  public:
    LSelectNode(BaseTable& table, std::vector<Predicate> predicates);
    // returns a reference to BaseTable
    BaseTable& GetBaseTable();
    // returns end status and next predicate (if exists)
    std::tuple<int, Predicate> GetNextPredicate();
    // resets predicate iterator
    void ResetIterator();
    ~LSelectNode();
  private:
    int iteratorpos;
    std::vector<Predicate> predicates;
    BaseTable table;
};

class LUniqueNode : public LAbstractNode{
  public:
    LUniqueNode(LAbstractNode* child);
    ~LUniqueNode();
};

// Physical node interface (result), should be used for automatic testing

class PResultNode{
  public:
    PResultNode(PResultNode* left, PResultNode* right, LAbstractNode* p);
    virtual ~PResultNode();
    // returns number of attributes
    virtual int GetAttrNum() = 0;
    // prints tree
    virtual void Print(int indent) = 0;
    // used to get attribute info
    LAbstractNode* prototype;
    // returns error status and data, if possible
    virtual std::tuple<ErrCode, std::vector<Value>> GetRecord();
  protected:
    PResultNode* left;
    PResultNode* right;
    std::vector<std::vector<Value>> data;
    int pos;
};

#endif // INTERFACE_H
