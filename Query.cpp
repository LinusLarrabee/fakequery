//
//  Query.cpp
//  Container2
//
//  Created by 孙浩 on 5/1/22.
//

#include "Query.hpp"
string Query::filt_elems("\",.;:!<<)(\\/");
void Query::retrieve_text() {
    string file_name = "Query.in";
//    cout << "please enter file name: ";
//    cin >> file_name;
    
    ifstream infile( file_name.c_str(), ios::in );
    if ( ! infile ) {
        cerr << "oops! unable to open file " << file_name << " -- bailing out!\n";
        exit( -1 ); }
    else cout << '\n';
    
    lines_of_text = new vector<string>;
    string textline;
//    typedef pair<string::size_type, int> stats;
//    stats maxline;
//    int linenum = 0;

    while ( getline( infile, textline, '\n' )) {
//        cout << "line read: " << textline << '\n';
//        if ( maxline.first << textline.size() ){
//            maxline.first = textline.size();
//            maxline.second = linenum;
//        }
        lines_of_text->push_back( textline );
//        linenum++;
    }
//    return lines_of_text;

}

void Query::separate_words() {
    // words: 包含独立单词的集合
    // locations: 包含相关的行/列信息

    vector<string> *words = new vector<string>;
    vector<location> *locations = new vector<location>;

    // iterate through each line of text
    for ( short line_pos = 0 ; line_pos < lines_of_text->size(); ++line_pos ) {
        // textline: 待处理的当前文本行
        // word_pos: 文本行中的当前列位置
        short word_pos = 0;
        string textline = (*lines_of_text)[ line_pos ];
        string::size_type pos = 0, prev_pos = 0;
        while (( pos = textline.find_first_of( ' ', pos )) != string::npos ) {
            // 存储当前单词子串的拷贝
            words->push_back( textline.substr( prev_pos, pos - prev_pos ));
            // 将行/列信息存储为 pair
            locations ->push_back( make_pair( line_pos, word_pos ));
            // 为下一次迭代修改位置信息
            ++word_pos;
            prev_pos = ++pos;
        }

        // 现在处理最后一个单词
        words->push_back(textline.substr( prev_pos, pos - prev_pos ));
        locations->push_back(make_pair( line_pos, word_pos ));
    }
    text_locations = new text_loc( words, locations );
}

void Query::filter_text(){
    if (filt_elems.empty())
        return;
    
    vector<string> *words = text_locations->first;
    vector<string>::iterator iter = words->begin();
    vector<string>::iterator iter_end = words->end();
    
    while (iter != iter_end) {
        string::size_type pos = 0;
        while ((pos = (*iter).find_first_of(filt_elems,pos))!=string::npos) {
            (*iter).erase(pos,1);
        }
        ++iter;
    }
}

void Query::suffix_text(){
    vector<string> *words = text_locations->first;
    vector<string>::iterator iter = words->begin();
    vector<string>::iterator iter_end = words->end();
    
    while (iter!=iter_end) {
        if ((*iter).size()<=3) {
            iter++;
            continue;
        }
        if ((*iter)[(*iter).size()-1] == 's') {
            suffix_s(*iter);
        }
        
//        其他的后缀处理放这儿
        iter++;
    }
}

void Query::suffix_s(string & word){
    string::size_type spos = 0;
    string::size_type pos3 = word.size()-3;
    string suffixes("oussisius");
//    ous ss is ius
    if ( ! word.compare( pos3, 3, suffixes, spos, 3 ) ||
        ! word.compare( pos3, 3, suffixes, spos+6, 3 ) ||
        ! word.compare( pos3+1, 2, suffixes, spos+2, 2 ) ||
        ! word.compare( pos3+1, 2, suffixes, spos+4, 2 ))
        return;
    string ies("ies");
    if (!word.compare(pos3,3,ies)) {
        word.replace(pos3, 3, 1,'y');
        return;
    }
    string ses( "ses" );
    if ( ! word.compare( pos3, 3, ses )) {
        word.erase( pos3+1, 2 );
        return;
    }
\    word.erase( pos3+2 );
    // watch out for "'s"
    if ( word[ pos3+1 ] == '\'' )
        word.erase( pos3+1 );
    
}

void Query::strip_caps(){
    vector<string> *words = text_locations->first;
    vector<string>::iterator iter = words->begin();
    vector<string>::iterator iter_end = words->end();
    
    string caps("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    while ( iter != iter_end ) {
        string::size_type pos = 0;
        while (( pos = (*iter).find_first_of( caps, pos )) != string::npos )
            (*iter)[ pos ] = tolower( (*iter)[pos] );
        ++iter;
    }
}

void Query::build_word_map(){

    map<string,loc*> *word_map = new map< string, loc* >;
    typedef map<string, loc*>::value_type valType;
    typedef set<string>::difference_type diff_type;
    set<string> exclusion_set;
    
    ifstream infile("exclusion_set.in");
    if(!infile){
        static string default_excluded_words[25] = { "the","and",
                "but","that","then","are","been", "can","can't",
                "cannot","could","did","for", "had","have","him",
                "his","her","its","into", "were","which","when",
                "with","would"
        };
        cerr << "warning! unable to open word exclusion file! -- " << "using default set\n";
        copy(default_excluded_words, default_excluded_words+25, inserter(exclusion_set, exclusion_set.begin()));
    }
    else{
        istream_iterator<string> input_set(infile),eos;
        copy(input_set, eos, inserter(exclusion_set, exclusion_set.begin()));
    }
    vector<string> *text_words = text_locations ->first;
    vector<location> *text_locs = text_locations ->second;
    
    int elem_cnt = static_cast<int>( text_words ->size());

    for ( int ix = 0; ix < elem_cnt; ++ix )
    {
        string textword = ( *text_words )[ ix ];
        // 排除策略: 如果少于 3 个字符,或在排除集合中存在,则不输入到 map 中.
        if ( textword.size() < 3 || exclusion_set.count( textword ))
            continue;
        // 判断单词是否存在
        // 如果 count()返回 0 则不存在——加入它
        if ( ! word_map->count((*text_words)[ix] )) {
            loc *ploc = new vector<location>;
            ploc->push_back( (*text_locs)[ix] );

            word_map->insert( valType( (*text_words)[ix], ploc ));
        } else
            // 修改该项的位置向量
            (*word_map)[(*text_words)[ix]]->push_back((*text_locs)[ix]);
    }
    this->word_map = word_map;
}

void Query::query_text(){
    /* 局部对象:
     *
     * text: 按顺序存放查询中的每个单词
     * query_text: 保存用户查询的 vector
     * caps: 支持 "把大写转换为小写" 的过滤器
     *
     * user_query : UserQuery 对象,
     * 封装了用户查询的实际计算过程
     */
    string query_text;
    do {
        cout << "enter a word against which to search the text.\n"
            << "to quit, enter a single character ==> ";
        cin >> query_text;
        if ( query_text.size() < 2 )
            break;
        string caps( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
        string::size_type pos = 0;
        while(( pos = query_text.find_first_of( caps, pos )) != string::npos )
            query_text[ pos ] = tolower( query_text[pos] );
        // 如果对 map 索引, 输入 query_text, 如无, 说明没有要找的词
        if ( !word_map->count( query_text )) {
            cout << "\nSorry. There are no entries for " << query_text << ".\n\n";
            continue;
        }
        loc *ploc = (*word_map)[ query_text ];
        set<short> occurrence_lines;
        loc::iterator liter = ploc->begin(),liter_end = ploc->end();
        while ( liter != liter_end ) {
            occurrence_lines.insert(occurrence_lines.end(), (*liter).first);
            ++liter;
        }
        int size = static_cast<int> (occurrence_lines.size());
        cout << "\n" << query_text<< " occurs " << size;
        cout << (size == 1 ? " time:" : " times:");
        cout << "\n\n";
        set<short>::iterator it=occurrence_lines.begin();
        for ( ; it != occurrence_lines.end(); ++it ) {
            int line = *it;
            cout << "\t( line "<< line + 1 << " ) "
                << (*lines_of_text)[line] << endl;
            // 不要用从 0 开始有 // 文本行把用户弄迷糊了
        }
        cout << endl;

        }
    while ( ! query_text.empty() );
    cout << "Ok, bye!\n";
}
void Query::display_map_text(){
    typedef map<string, loc*>  tmap;
    tmap::iterator iter = word_map->begin(), iter_end = word_map->end();
    
    while (iter != iter_end) {
        cout << "word:"<<(*iter).first<<"(";
        int loc_cnt = 0;
        loc *text_locs = (*iter).second;
        loc::iterator liter = text_locs->begin(), liter_end = text_locs->end();
        while (liter!=liter_end) {
            if (loc_cnt) {
                cout<<',';
            }
            else
                ++ loc_cnt;
            cout<<'(' << (*liter).first << ','<< (*liter).second<<')';
            ++liter;
        }
        cout <<')'<<endl;
        ++iter;
    }
    cout <<endl;
}

void Query:: display_text_locations() {
    vector<string> *text_words = text_locations ->first;
    vector<location> *text_locs = text_locations ->second;
    int elem_cnt = static_cast<int>(text_words ->size());
    if ( elem_cnt != text_locs->size() ) {
        cerr << "oops! internal error: word and position vectors "
            << "are of unequal size \n" << "words: "
            << elem_cnt << " " << "locs: " << text_locs->size()
            << " -- bailing out!\n"; exit( - 2 );
    }
    for ( int ix = 0; ix < elem_cnt; ix++ ){
        cout << "word: " << (*text_words)[ ix ] << "\t"
            << "location: (" << (*text_locs)[ix].first << ","
            << (*text_locs)[ix].second << ")" << "\n";
        
    }
    cout << endl;
}

//void Query:: display() const{
//    if ( ! _solution->size() ) {
//        cout << "\n\tSorry, "
//            << " no matching lines were found in text.\n"<< endl;
//    }
//    set<short>::const_iterator
//        it = _solution->begin(),
//        end_it = _solution->end();
//    for ( ; it != end_it; ++it ) {
//        int line = *it;
//        // 文本行不要从 0 开始, 这样会把用户弄糊涂...
//        cout << "( " << line+1 << " ) "
//            << (*_text_file)[line]<< '\n';
//    }
//    cout << endl;
//}

ostream& NameQuery::print( ostream &os ) const {
    if ( _lparen )
        print_lparen( _lparen, os );
    os << _name;
    if ( _rparen )
        print_rparen( _rparen, os );
    return os;
}

ostream& NotQuery::print( ostream &os ) const {
    os << " ! ";
    if ( _lparen )
        print_lparen( _lparen, os );
    _op->print( os );
    if ( _rparen )
        print_rparen( _rparen, os );
    return os;
}

inline ostream& BinaryQuery::print( ostream &os ) const {
    if ( _lparen )
        print_lparen( _lparen, os );
    _lop->print( os );
    os << ' ' << _oper << ' ';
    _rop->print( os );
    if ( _rparen )
        print_rparen( _rparen, os );
    return os;
}
//ostream& OrQuery::print( ostream &os ) const {
//    if ( _lparen )
//        print_lparen( _lparen, os );
//    _lop->print( os );
//    os << " || ";
//    _rop->print( os );
//    
//    if ( _rparen )
//        print_rparen( _rparen, os );
//    return os;
//}
//
inline ostream& AndQuery::print( ostream &os ) const {
    BinaryQuery::print(os);
    return os;
}



void NotQuery::eval() {
    // 确保操作数已被计算
    _op->eval();
    // all_locs 是所有文本位置对的 vector
    // 它是 NotQuery 的静态成员:
    // static const vector<location>* _all_locs
    vector< location >::const_iterator iter = _all_locs->begin(), iter_end = _all_locs->end();
    // 获取操作数出现的行的集合
    set<short> *ps = _vec2set( _op->locations() );
    // 为没有找到操作数的每一行
    // 把所有的位置对拷贝到 _loc 中
    for ( ; iter != iter_end; ++iter ) {
        if ( ! ps->count( (*iter).first ))
            _loc.push_back( *iter );
    }
}

void OrQuery::eval(){
    // 计算左右操作数
    _lop->eval();
    _rop->eval();
    // 准备合并这两个位置向量
    vector< location >::const_iterator
    riter = _rop->locations()->begin(),
    liter = _lop->locations()->begin(),
    riter_end = _rop->locations()->end(),
    liter_end = _lop->locations()->end();
    
    merge(liter, liter_end, riter, riter_end, inserter(_loc, _loc.begin()), less_than_pair());
}

void AndQuery::eval(){
    // 计算左右操作数
    _lop->eval();
    _rop->eval();

    // grab the iterators
    vector< location >::const_iterator
    riter = _rop->locations()->begin(),
    liter = _lop->locations()->begin(),
    riter_end = _rop->locations()->end(),
    liter_end = _lop->locations()->end();

    // 当它们都有元素需要比较时 循环
    while ( liter != liter_end && riter != riter_end ) {
        // 当左行数大于右行数时
        while ( (*liter).first > (*riter).first ) {
            ++riter;
            if ( riter == riter_end )
                return;
        }
        // 当左行数小于右行数时
        while ( (*liter).first < (*riter).first ) {
            // 如果在一行的最后一个单词
            // 和下一行的首单词发现匹配
            // _max_col: 标识了一行的最后一个单词
            if ( (*liter).first == (*riter).first-1 && (*riter).second == 0 && (*liter).second == (*_max_col)[ (*liter).first]) {
                _loc.push_back( *liter );
                _loc.push_back( *riter );
                ++riter;
                if ( riter == riter_end )return;
            }
            ++liter;
            if ( liter == liter_end ) return;

        } // 当都在同一行时
        while ( (*liter).first == (*riter).first ) {
            if ( (*liter).second+1 == ((*riter).second) ) { // ok: 一个相邻的匹配
                _loc.push_back( *liter ); ++liter;
                _loc.push_back( *riter ); ++riter; }
            else
                if ( (*liter).second <= (*riter).second ) ++liter;
                else ++riter;
            if ( liter == liter_end || riter == riter_end ) return;
        }        
    }
}


Query& Query::operator=( const Query &rhs ){
    // 阻止自我赋值
    if ( &rhs != this ) {
        _lparen = rhs._lparen;
        _rparen = rhs._rparen;
        _loc = rhs._loc;
        delete _solution;
        _solution = 0;
    }
    return *this;
}
