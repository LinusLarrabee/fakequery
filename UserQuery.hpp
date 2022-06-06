//
//  UserQuery.hpp
//  Query
//
//  Created by 孙浩 on 5/27/22.
//

#ifndef UserQuery_hpp
#define UserQuery_hpp

#include <string>
#include <vector>
#include <map>
#include <stack>
using namespace std;
typedef pair<short,short> location;
typedef vector<location> loc;

#include "Query.hpp"
class UserQuery{
public:
    UserQuery( vector< string> *pquery = 0 )
    : _query( pquery ), _eval( 0 ), _paren( 0 ) {}
    
    Query *eval_query(); // 建立层次结构
    void query( vector< string > *pq );
    void displayQuery();
    
    static void word_map( map<string,loc*> *pwm ) {
        if ( !_word_map ) _word_map = pwm; }
private:
    enum QueryType { WORD = 1, AND, OR, NOT, RPAREN, LPAREN };
    QueryType evalQueryString( const string &query );
    void evalWord( const string &query );
    void evalAnd();
    void evalOr();
    void evalNot();
    void evalRParen();
    bool integrity_check();
    
    int _paren;
    Query *_eval;
    vector<string> *_query;
    
    stack<Query*,vector<Query*> > _query_stack;
    stack<Query*,vector<Query*> > _current_op;
    
    static short _lparenOn, _rparenOn;
    static map<string,loc*> *_word_map;
};
//
//inline void UserQuery:: evalAnd() {
//    Query *pop = _query_stack.top(); _query_stack.pop();
//    AndQuery *pq = new AndQuery( pop );
////    Allocating an object of abstract class type 'AndQuery'
////    一般是该类继承的抽象类中，有未实现的抽象函数
//    if ( _lparenOn ) { pq->lparen( _lparenOn ); _lparenOn = 0; }
//    if ( _rparenOn ) { pq->rparen( _rparenOn ); _rparenOn = 0; }
//    _current_op.push( pq );
//
//}
//
//inline void UserQuery:: evalOr() {
//    Query *pop = _query_stack.top();
//    _query_stack.pop();
//    OrQuery *pq = new OrQuery( pop );
//    
//    if ( _lparenOn ) { pq->lparen( _lparenOn ); _lparenOn = 0; }
//    if ( _rparenOn ) { pq->rparen( _rparenOn ); _rparenOn = 0; }
//    _current_op.push( pq );
//
//}
//
//inline void UserQuery:: evalNot() {
//    NotQuery *pq = new NotQuery;
//    if ( _lparenOn ){
//        pq->lparen( _lparenOn ); _lparenOn = 0; }
//    if ( _rparenOn ) {
//        pq->rparen( _rparenOn ); _rparenOn = 0; }
//    _current_op.push( pq );
//}
//
//inline void UserQuery:: evalRParen() {
//    if ( _paren < _current_op.size() ) {
//        Query *poperand = _query_stack.top();
//        _query_stack.pop();
//        Query *pop = _current_op.top();
//        _current_op.pop();
//        pop->add_op( poperand );
//        _query_stack.push( pop );
//    }
//}
//
//inline void UserQuery::evalWord( const string &query ) {
//    NameQuery *pq;
//    loc *ploc;
//    if ( ! _word_map->count( query ))
//        pq = new NameQuery( query );
//    else {
//        ploc = ( *_word_map )[ query ];
//        pq = new NameQuery( query, *ploc );
//    }
//    if ( _current_op.size() <= _paren )
//        _query_stack.push( pq );
//    else {
//        Query *pop = _current_op.top();
//        _current_op.pop();
//        pop->add_op( pq );
//        _query_stack.push( pop );
//    }
//}

#endif /* UserQuery_hpp */
