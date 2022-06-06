//
//  Query.hpp
//  Container2
//
//  Created by 孙浩 on 5/1/22.
//

#ifndef Query_hpp
#define Query_hpp
#include <iostream>
#include <fstream>

#include <list>
#include <map>
#include <deque>
#include <set>
#include <vector>
#include <iterator>

#include <ctype.h>
#include <string>
#include <locale>
#include <algorithm>
#include <stddef.h>
#include <utility>

using namespace std;

typedef pair<short,short> location;
typedef vector<location> loc;
typedef vector<string> text;
typedef pair< text*,loc* > text_loc;

class NameQuery;

class Query{
    friend class NameQuery;
public:
    Query(const Query &rhs): _loc(rhs._loc),_lparen(rhs._lparen), _rparen(rhs._rparen), _solution(0){}
    Query& operator=( const Query &rhs );
    static void filter_elements(string felems){filt_elems = felems;}
    
    void query_text();
    void display_map_text();
    void display_text_locations();
    
    void build_text_map(){
        retrieve_text();
        separate_words();
        filter_text();
        suffix_text();
        strip_caps();
        build_word_map();
    }
    
    virtual Query* clone() = 0;
    virtual void eval() = 0;
    void display () const;
    // 读访问函数
    const set<short> *solution();
    const vector<location> *locations() const { return &_loc; }
    static const vector<string> * text_file(){ return _text_file; }
    
    virtual ostream& print(ostream &os = cout) const = 0;
    
    // 设置 _lparen 和 _rparen
    void lparen( short lp ) { _lparen = lp; }
    void rparen( short rp ) { _rparen = rp; }

    // 获取 _lparen 和 _rparen 的值
    short lparen() { return _lparen; }
    short rparen() { return _rparen; }

    // 打印左右括号
    void print_lparen( short cnt, ostream& os ) const{}
    void print_rparen( short cnt, ostream& os ) const{}
protected:
    Query();
    set<short> *_vec2set( const vector<location>* );
    static vector<string> * _text_file;
    
    set<short> *_solution;
    vector<location> _loc;
    
    // 拥有左右括号的数目
    short _lparen;
    short _rparen;
private:
    explicit Query(const vector<location>&);
    
    void retrieve_text();
    void separate_words();
    void filter_text();
    void strip_caps();
    void suffix_text();
    void suffix_s( string& );
    void build_word_map();
    
    vector<string> *lines_of_text;
    text_loc *text_locations;
    map< string,loc*> *word_map;
    static string filt_elems;
    
};

inline Query::Query():_solution(0){}
inline Query::Query(const vector<location> &loc):_solution(0),_loc(loc){}

inline const set<short>* Query::solution(){
    return _solution ? _solution : _solution = _vec2set( &_loc );
}

typedef vector<location> loc;
class NameQuery : public Query {
public:
    explicit NameQuery(const string&);
    virtual NameQuery * clone() { return new NameQuery(*this); }
    
    NameQuery (const string&, vector<location>*);
    virtual void eval() {}
    // 读访问函数
    string name() const { return _name; }
    static const map<string,loc*> *word_map() { return _word_map; }
    
    virtual ostream& print( ostream &os ) const;
protected:
    string _name;
    static map<string,loc*> *_word_map;
    bool _present;
};

inline NameQuery::NameQuery( const string &name )
// Query::Query() 被隐式调用
: _name( name ) {}

inline NameQuery::NameQuery( const string &name, vector<location> *ploc )
: _name( name ), Query( *ploc ) {}

class NotQuery : public Query {
public:
    NotQuery(Query* op=0);
    NotQuery(const NotQuery &rhs):Query(rhs){_op = rhs._op->clone();}
    // 另外一种语法: 显式的 virtual 关键词
    // 改写 Query::eval()
    virtual void eval();

    // 读访问函数
    const Query *op() const { return _op; }
    static const vector<location>*all_locs(){ return _all_locs; }
    virtual ostream& print (ostream & os) const;
protected:
    Query *_op;
//    op是干什么的？
    static const vector< location > *_all_locs;
};

inline NotQuery::NotQuery( Query *op ) : _op( op ) {}


// Binary 二元操作
class BinaryQuery : public Query {
public:
    BinaryQuery(Query *lop, Query * rop, string oper)
        : _lop(lop),_rop(rop), _oper(oper){}
    ~BinaryQuery(){delete _lop; delete _rop;}
    virtual ostream& print (ostream& = cout) const = 0;
    
    const Query *lop() { return _lop; }
    const Query *rop() { return _rop; }

protected:
    inline BinaryQuery( Query *lop, Query *rop ) : _lop( lop ), _rop( rop ) {}
    Query *_lop;
    Query *_rop;
    string _oper;
};

class OrQuery : public BinaryQuery {
public:
    OrQuery(Query *lop = 0, Query *rop = 0):BinaryQuery(lop,rop){}
    virtual void eval();
    virtual ostream& print (ostream &os) const;

};


class AndQuery : public BinaryQuery {
public:
    AndQuery(Query *lop = 0, Query *rop = 0):BinaryQuery(lop,rop){}
    virtual void eval();
    
    static void max_col( const vector< int > *pcol )
        { if ( !_max_col ) _max_col = pcol; }
    virtual ostream& print (ostream &os) const;
    

protected:
    static const vector< int > *_max_col;
};


class less_than_pair{
public:
    bool operator()(location loc1, location loc2){
        return ((loc1.first < loc2.first) || ((loc1.first == loc2.first) && (loc1.second <loc2.second)));
    }
};
#endif /* Query_hpp */
