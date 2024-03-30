//
//  main.cpp
//  PL_proj1
//
//  Created by Ruby on 2023/3/6.
//

# include <iostream>
# include <string>
# include <vector>
# include <cmath>
# include <sstream>
# include <stdio.h>
# include <string.h>
# include <cstdlib>
# include <cctype>

using namespace std;

struct PToken {
  string atom ;
  int column ;
  int line ;
  bool parentheses ;
  int parenthesesNUM;
};

struct Node {
  Node *left;
  Node *right;
  PToken leftPItem;
  PToken rightPItem;
};

typedef Node* Nodeptr;

struct Token {
  string atom ;
  int column ;
  int line ;
  vector<Token> tree;
  Node* nodeptr;
  // vector<Token> rightTree;
};

Nodeptr ghead = NULL;
Nodeptr gnow = NULL;
bool gwelcom = false;
vector<Token> gData;
vector<int> gSyntaxVec;
Token gtokenItem;
int gNextSyntaxNUM = 0;
int gP_NUM = 0;
bool gErrorEOF = false;
//bool gTest = true; // true false
bool gTest = false; // true false

bool IsFloat( string token ) { // FLOAT : '3.25', '.25', '+.25', '-.25', '+3.'
  //  cout <<token <<endl;
  size_t found1 = 0, found2 = 0 ;
  string newString = "" ;
  found1 = token.find_first_of( "." ) ;
  found2 = token.find_last_of( "." );

  if ( found1 != found2 ) return false;
  if ( ( token[0] == '+' || token[0] == '-' ) && token[1] == '.'
       && token.length() == 2 ) return false;

  if ( strstr( token.c_str(), "." ) != NULL ) { // 有.
    if ( token[0] == '+' || token[0] == '-' ) { // 第一個字元是+,-
      if ( token[0] == '-' ) newString += '-'; // 如果是負的 負號也要加上去
      if ( token[1] == '.' ) newString += '0';
      for ( int i = 1 ; i < token.length() ; i++ ) {

        newString += token[i];
        if ( ( token[i] < '0' || token[i] > '9' ) && token[i] != '.' )  return false; // 不是數字且也不是.
      } // for
      // cout << newString <<endl;
      gtokenItem.atom = newString;
      return true;
    } // if

    for ( int i = 0 ; i < token.length() ; i++ ) {
      // cout << token[i] <<endl;
      if ( ( token[i] < '0' || token[i] > '9' ) && token[i] != '.' )  return false; // 不是數字且也不是.

    } // for

    return true;
  } // if

  return false;

} // IsFloat()

bool IsInt( string token ) { // 3, +3 ,-3
  string newString = "" ;
  if ( token[0] == '+' || token[0] == '-' ) { // 第一個字元是+,-
    if ( token[0] == '-' ) newString += '-';

    for ( int i = 1 ; i < token.length() ; i++ ) { // 除了第一個字元後都是否都是數字
      newString += token[i];
      if ( token[i] < '0' || token[i] > '9' )  return false ; // 有不是數字的字元 return false
    } // for

    gtokenItem.atom = newString;
    return true;
  } // if
  else {
    for ( int i = 0 ; i < token.length() ; i++ ) {
      if ( token[i] < '0' || token[i] > '9' )  return false ; // 有不是數字的字元 return false


    } // for

    return true;
  } // else

  return false;

} // IsInt()

float Rounding( float num, int index )
{ // 四捨五入 取到 小數點第 X 位
  bool isNegative = false; // whether is negative number or not

  if ( num < 0 ) // if this number is negative, then convert to positive number
  {
    isNegative = true;
    num = -num;
  } // if

  if ( index >= 0 )
  {
    int multiplier;
    multiplier = pow( 10, index );
    num = ( int ) ( num * multiplier + 0.5 ) / ( multiplier * 1.0 );
  } // if

  if ( isNegative ) // if this number is negative, then convert to negative number
  {
    num = -num;
  } // if

  return num;
} // Rounding()



int GetTokenType( string token ) {
  int tmp = 0  ;
  //  float float_num ;
  double float_num ;
  stringstream s ;
  string addZero = "";
  char str[20];
  if ( token == "." ) return 0 ;
  else if ( token == "(" ) return 1 ;
  else if ( token == ")" ) return 2 ;
  else if ( token == "\'" ) return 3 ; // the single-quote character (')
  else if ( token == "\"" ) return 4 ; // the double-quote character (")
  else if ( token == ";" ) return 5 ;
  else if ( IsFloat( token ) ) { // 是float
    token =  gtokenItem.atom ;
    //    cout << "token " << token << endl;
    tmp = token.length() - token.find( "." ) ;
    for ( int i = 0 ; i < token.length() ; i++ ) {
      str[i] = token[i];
    } // for
    //    float_num = stof(token); // string to float
    float_num = atof( str ); // string to float
    if ( token[0] == '.' ) { // 若.前無數字則要補0
      token = "0" + token;
    } // if

    if ( tmp > 4 ) { // ex: 13.56789 大於小數點後三位 取到小數點後三位 四捨五入
      float_num = Rounding( float_num, 3 ) ;
      s << float_num;
      token = s.str();
    } // if
    else if ( tmp < 4 ) { // ex: 13.56 小於小數點後三位 補0
      for ( int i = 0 ; i < 4-tmp ; i ++ ) {
        addZero += '0';
      } // for

      token += addZero;
    } // else if

    gtokenItem.atom = token ; // 更改atom
    return 6 ; // float
  } // else if
  else if ( IsInt( token ) ) { // 是數字
    // cout << "(INT) " << token << " " ;
    return 7 ;
  } // else if
  else if ( token == "nil" || token == "#f" ) {
    gtokenItem.atom = "nil" ;
    return 9 ;
  } // else if
  else if ( token == "t" || token == "#t" ) {
    gtokenItem.atom = "#t" ;
    return 10 ;
  } // else if
  else if ( token == "()" ) {
    gtokenItem.atom = "nil" ;
    return 11 ;
  } // else if
  else {
    gtokenItem.atom = token ;
    return 0;
  } // else
} // GetTokenType()

bool CheckSyntax( int SyntaxNum ) {
  if ( SyntaxNum == 6 || SyntaxNum == 7 || SyntaxNum == 9 ||
       SyntaxNum == 10 || SyntaxNum == 0 ) { // 如果目前是atom 要將gNextsyntaxNUM 設為下一個可以的號碼

  } // if

  if ( SyntaxNum == 1  ) {
    // 如果目前是左括號 要將gNextsyntaxNUM 設為下一個可以的號碼: <S-exp>,
    gSyntaxVec.push_back( 1 );
    gNextSyntaxNUM = 3;
  } // if

  return true;
} // CheckSyntax()


bool gFirst = false;
void PrettyPrint( Nodeptr walk ) {
  bool haveP = false;
  //  cout << "gP_num " << gP_NUM <<endl;
  if ( walk == NULL ) {
    return;
  } // if

  if ( walk->leftPItem.atom != "" ) {
    if ( walk->leftPItem.parenthesesNUM == 1 ) { // 印leftPItem的左括號
      for ( int i = 0 ; i < gP_NUM  && gP_NUM == 1 ; i++ ) {
        haveP = true ;
        cout << "  ";
      } // for

      cout << "( ";
      gP_NUM ++;
    } // if

    if ( !haveP ) {
      for ( int i = 0 ; i < gP_NUM  && gFirst ; i++ ) {
        cout << "  ";
      } // for
    } // if

    cout << walk->leftPItem.atom << endl;
    gFirst = true;
    //    cout << walk->LeftPItem.atom << "     " <<walk->LeftPItem.parenthesesNUM<<endl;
    //    if ( gTest ) cout << "gP_NUM$ " << gP_NUM << endl;

    if ( walk->leftPItem.parenthesesNUM == 2  &&  gP_NUM != 1 ) { // 印leftPItem的右括號 &&  gP_NUM != 1
      for ( int i = 0 ; i < gP_NUM ; i++ ) {
        cout << "  ";
      } // for

      cout << ")\n";
      gP_NUM --;
    } // if

  } // if

  if ( walk->rightPItem.atom != "" && walk->rightPItem.atom != "nil" &&  walk->rightPItem.atom != "()" ) {
    //    cout << gP_NUM << endl;
    if ( walk->rightPItem.parenthesesNUM == 1 ) {
      cout << "( ";
    } // if

    //    cout << "gP_NUM " << gP_NUM << endl;
    for ( int i = 0 ; i < gP_NUM ; i++ ) {
      cout << "  ";
    } // for

    cout << ".\n" ;
    for ( int i = 0 ; i < gP_NUM ; i++ ) {
      cout << "  ";
    } // for

    cout << walk->rightPItem.atom << endl;
    //    cout << walk->RightPItem.atom << "    " <<walk->RightPItem.parenthesesNUM <<endl;
    if ( gTest ) cout << "gP_NUM# " << gP_NUM << endl;
    if ( walk->rightPItem.parenthesesNUM == 2 &&  gP_NUM != 1 ) { // &&  gP_NUM != 1
      gP_NUM -- ;
      for ( int i = 0 ; i < gP_NUM ; i++ ) {
        cout << "  ";
      } // for

      cout << ")\n";
    } // if
  } // if

  PrettyPrint( walk->left );
  PrettyPrint( walk->right );

} // PrettyPrint()


void Init() {
  ghead = NULL;
  gnow = NULL;
  gP_NUM = 0;
  gFirst = false;
  gtokenItem.atom = "" ;
  //  preNode = NULL;
} // Init()

void InitPtoken( Nodeptr node ) {
  node->leftPItem.atom = "" ;
  node->leftPItem.parentheses = false ;
  node->leftPItem.parenthesesNUM = 0 ;
  node->rightPItem.atom = "" ;
  node->rightPItem.parentheses = false ;
  node->rightPItem.parenthesesNUM = 0 ;
} // InitPtoken()

void VectorPrint() {
  //  cout << "\n\nvectorPrint:\n";
  if ( !gwelcom ) {
    cout << "\nWelcome to OurScheme!" << endl;
    gwelcom = true;
  } // if

  //  cout << "\n&& " << gData.size() ;

  for ( int i = 0 ; i < gData.size() ; i++ ) {
    if ( gData.size() >= 1 ) cout << "\n> " ;
    cout << gData[i].atom ;
    if ( i != gData.size()-1  ) cout << endl;

  } // for

  if ( gData.size() >= 1 ) cout << endl;
  gData.clear();
  Init();
} // VectorPrint()

string IntTOString( int inputNUM ) {
  stringstream ss;
  string str;
  ss << inputNUM ;
  str = ss.str();
  return ss.str();
} // IntTOString()

void ReadSExp() {
  Nodeptr preNode = NULL;
  char inputChar = '\0', prechar = '\0';
  bool dotWithSpace = false, rightP = false;
  string atom = "", dqText = "", errorString = "" ;
  int dqNum = 0;
  int column = 1, line = 1, loc = 0 ;
  int parentheses = 0, whileNUM = 0  ;
  bool isParentheses = false, dot = false, dot_NP = false ;
  bool doubleQuote = false, pWithoutDot = false, firstatom = false;
  bool nil = false, newNode = false;
  int pNUM = 0, atomNum = 0 ;
  bool enter = false;
  bool preP = false, onlyNIL = false;
  bool float_end = false, before_dQuote = false, error = false;

  while ( whileNUM == 0 && !cin.eof() ) { // cin.peek() != '\0'
    enter = false;
    if ( !isspace( inputChar ) ) {
      if ( inputChar == '(' ) preP = true; // 用在(exit)
      prechar = inputChar;
      //      cout << "@@ prechar: " << prechar <<endl;
    } // if

    // if ( gTest ) cout << "before_dQuote " << before_dQuote << endl;
    if ( !before_dQuote ) {
      inputChar = '\0';
      cin.get( inputChar );
      if ( cin.eof() ) {
        gErrorEOF = true;
        whileNUM = 1 ;
      } // if
      // "與其他字連再一起的時 ex:4"ggg" 先暫停讀入 處理原本“字元
    } // if

    if ( gTest ) cout << ": " << inputChar << endl;
    // if ( gTest ) cout << "onlyNIL: " << onlyNIL << endl;
    if ( inputChar == '('  ) {
      pNUM = 1;
      if ( gTest ) cout << "@@ PNUM = 1 @@" << endl;
    } // if
    else if ( inputChar == ')' ) {
      pNUM = 2;
    } // if
    //    else pNUM = 0;
    if ( gTest ) cout << "@@ line @@ "  << line << endl;
    loc ++ ; // 目前位置
    if ( ( inputChar == '\"' && isParentheses ) ||  before_dQuote ) { // ( " ab " )
      //  if ( gTest ) cout << "ATOM! " << atom << endl;
      if ( gTest ) cout << "In doublequote " << endl;
      if ( before_dQuote ) {
        atom = "";
        atom += prechar;
        if ( gTest ) cout << "ATOM! " << atom << endl;
        while ( cin.peek() != '\"' ) {
          cin.get( inputChar );
          if ( gTest ) cout << "inputChar~ " << inputChar << endl;
          atom += inputChar ;
        } // while

        cin.get( inputChar );
        atom += inputChar ;
        if ( gTest ) cout << "ATOM! " << atom << endl;
        if ( gTest ) cout << "inputChar! " << inputChar << endl;
        before_dQuote = false;
      } // if

      else if ( atom != "" ) { // "前還有資料尚未處理完
        before_dQuote = true;
        prechar = '\"';
      } // else if

      else if ( atom == "" ) {
        atom += inputChar; // 加入1st “
        while ( cin.peek() != '\"' ) {
          cin.get( inputChar );
          if ( gTest ) cout << "inputChar~ " << inputChar << endl;
          atom += inputChar ;
        } // while

        cin.get( inputChar ); // 加入2nd “
        atom += inputChar ;
      } // else if
    } // if 串列中有" （且“中有空格）

    if ( inputChar == '.' && atom != ""  ) { // 遇到.表示要處理浮點數
      if ( gTest ) cout << "## float ## " << endl;
      if ( isspace( cin.peek() ) || cin.peek() == '(' ||
           cin.peek() == ')' ||  cin.peek() == '\"' ) {
        float_end = true; // float_end = true
        atom += inputChar;
      } // if

      while ( !isspace( cin.peek() ) &&  cin.peek() != '(' &&
              cin.peek() != ')' &&  cin.peek() != '\"' ) {
        // ex:2.5 將點後面的東西都讀入直到遇到（）或 ＷＳ
        atom += inputChar;
        cin.get( inputChar ); // 讀入所有空白或tab
        loc ++ ; // 更改目前位置
      } // while

      if ( gTest ) cout << "atom " << atom << endl;
      if ( gTest ) cout << "inputChar " << inputChar << endl;
    } // if

    if ( doubleQuote && !isParentheses ) { // 遇到雙引號" 且不是在串列中
      if ( gTest ) cout << "inputChar\": " << inputChar << endl;
      if ( inputChar == '\n' ) {
        doubleQuote = false;
        if ( dqNum != 2 ) { // 表示雙引號數量錯誤
          // cout << line << " " << loc-1 << endl;
          cout << "\n> ERROR (no closing quote) : END-OF-LINE encountered at Line "
               << line << " Column " << loc-1  << endl;
        } // if
        else cout << dqText ;
        dqNum = 0;
        dqText = "" ;
      } // if
      else if ( inputChar == '\\' ) { // 處理"\n"
        //        cout << "###\\###" << endl;
        if ( cin.peek() == 'n' ) {
          cin.get( inputChar ); // 讀入n
          loc ++;
          dqText += "\n" ; // 換行
          enter = true;
        } // if
        else if ( cin.peek() == '\"' || cin.peek() == '\'' || cin.peek() == '\t' ||
                  cin.peek() == '\\' ) {
          cin.get( inputChar ); // 處理\\, \', \t, \"
          // column ++;
        } // else if

      } // else if
      else if ( inputChar == '\"' && !isParentheses ) { // 遇到第二個雙引號的狀況
        dqText += inputChar ; // 換行
        column ++;
        dqNum ++;
        while ( inputChar != '\n' ) { // 還未到結尾
          if ( cin.peek() == ' ' || cin.peek() == '\t' || cin.peek() == '\n' ) {
            cin.get( inputChar );
            loc ++;
            if ( inputChar == '\n' ) {
              cout << dqText << endl;
              doubleQuote = false;
              line = 1;
            } // if
          } // if
        } // while
      } // else if

      if ( !enter ) dqText += inputChar ; // cout << inputChar;
      //      cout << inputChar; //
    } // if

    else if ( inputChar == '\"' && !isParentheses  ) { // "(1 . 2  .3)" 先進
      if ( !gwelcom ) {
        cout << "\nWelcome to OurScheme!" << endl;
        gwelcom = true;
      } // if

      dqText = "";
      dqText += "\n> " ;
      dqText += inputChar; // 存入“
      doubleQuote = true;
      dqNum ++;
    } // else if 原封不動的印出

    else if ( inputChar == '(' || inputChar == ')'  || isspace( inputChar )
              || float_end || inputChar == '\"'  ) {
      // 遇到 WS, (, ), ', ", ; 開始處理atom字串
      if ( gTest ) cout << "inputChar: " << inputChar << endl;
      if ( nil ) nil = false;
      if ( inputChar == ')' ) {
        parentheses -= 1;
        if ( parentheses < 0 ) {
          gtokenItem.atom = "ERROR (unexpected token) : atom or '(' expected when token at Line "
                            + IntTOString( line ) + " Column " + IntTOString( loc ) + " is >>)<<" ;
          //            gtokenItem.atom = atom ;
          GetTokenType( gtokenItem.atom );
          gData.push_back( gtokenItem );
          // 跳掉此行
          while ( inputChar != '\n' && !cin.eof() ) {
            cin.get( inputChar );
            if ( cin.eof() ) {
              gErrorEOF = true;
              whileNUM = 1 ;
            } // if
          } // while

          parentheses = 0;
        } // if

        if ( gTest ) cout << "prechar " << prechar << endl;
        if ( prechar == '(' ) { // nil 的情況
          if ( gTest ) cout << parentheses << endl;
          atom += prechar ; // atom = (
          atom += inputChar ; // atom = ()
          nil = true;
          if ( cin.peek() == '\n' || ( parentheses == 0 && gData.size() == 0 ) ) {
            if ( gTest ) cout << " @onlyNIL@ " << endl;
            isParentheses = false;
            onlyNIL = true;
            gtokenItem.atom = atom ;
            GetTokenType( gtokenItem.atom );
            gData.push_back( gtokenItem );
            loc = 0;
            line = 1;
          } // if
        } // if
        else {
          rightP = true;
          if ( parentheses == 1 && atom == ""  ) {
            preNode = ghead;
            // cout << "gnow = head;" << endl;
          } // if
        } // else

      } // if

      if ( inputChar == '(' ) { // 遇到(要創建節點
        if ( atom != "" && !isParentheses ) { // 5(exit)
          gtokenItem.atom = atom ;
          GetTokenType( gtokenItem.atom );
          gData.push_back( gtokenItem );
        } // if

        parentheses ++ ; // 記錄括號數量
        isParentheses = true;
        if ( gTest ) cout << "dot" << dot << endl;
        if ( !dot ) {
          pWithoutDot = true;
        } // if

        // cout << "pWithoutDot " << pWithoutDot << endl;
        if ( cin.peek() == ')' ) { // 判斷()nil 的情況
          if ( gTest ) cout << " nil " << endl;
          parentheses -= 1;
          nil = true;
          if ( dot ) {
            dot_NP = true;
            dot = false;
          } // if

          atom += inputChar ; // atom = (
          cin.get( inputChar );
          atom += inputChar ; // atom = ()
          loc ++ ;
          if ( cin.peek() == '\n' || ( parentheses == 0 && gData.size() == 0 ) ) { // && gData.size() == 0
            if ( gTest ) cout << " @onlyNIL@ " << endl;
            isParentheses = false;
            onlyNIL = true;
            gtokenItem.atom = atom ;
            GetTokenType( gtokenItem.atom );
            gData.push_back( gtokenItem );
            loc = 0;
            line = 1;
          } // if
          else if (  atom == "()"  && !isParentheses ) {
            // cout << " SAVE () " << endl;
            isParentheses = false;
            //            gtokenItem.atom = atom ;
            //            GetTokenType( gtokenItem.atom );
            //            gData.push_back( gtokenItem );
            loc = 0;
            line = 1;
          } // else if


        } // if( cin.peek() == ')' )
        else { // 不是nil 就創立節點

          if ( ghead == NULL ) { // 遇到'('時 且 head為null 就創建新節點
            gnow = new Node;
            ghead = gnow ; // head指向最頂部
            preNode = gnow;
            gnow->left = NULL;
            gnow->right = NULL;
            InitPtoken( gnow );
            if ( gTest ) cout << "NEW_HEAD" << endl;
          } // if
          else {
            if ( gnow->leftPItem.atom != "" ) { // 如果遇到左括號但是左邊的atom有存東西 就要創建新右節點
              gnow->right = new Node;
              gnow = gnow->right; // ???????
              preNode = gnow;
              gnow->left = NULL; // 將right邊節點指向的左節點設為空
              gnow->right = NULL; // 將right邊節點指向的右節點設為空
              if ( gTest ) cout << " %% NEW_Right" << endl;
              InitPtoken( gnow );
              newNode = false;
            } // if

            else {
              if ( gnow->left == NULL && !dot ) { // 左邊為空 '.'後面接'('：要建立右邊的新節點
                gnow->left = new Node; // 創建左邊節點
                preNode = gnow;
                // if ( preNode == ghead ) cout << "$$" << endl;
                gnow = gnow->left; // ???????
                gnow->left = NULL; // 將左邊節點指向的左節點設為空
                gnow->right = NULL; // 將左邊節點指向的右節點設為空
                InitPtoken( gnow );
                if ( gTest ) cout << "NEW_LEFT" << endl;
              }  // if( gnow->left == NULL )

              if ( gnow->left != NULL && dot  ) { // dot : '.'後面接'('：要建立右邊的新節點
                gnow->right = new Node;
                gnow = gnow->right; // ???????
                preNode = gnow;
                gnow->left = NULL; // 將right邊節點指向的左節點設為空
                gnow->right = NULL; // 將right邊節點指向的右節點設為空
                InitPtoken( gnow );
                if ( gTest ) cout << "NEW_Right" << endl;
                newNode = false;
              } // if
            } // else

          } // else
        } // else
      } // if

      if ( inputChar == ' ' ||  inputChar == '\t' ) { // 遇到空白或tab
        while ( cin.peek() == ' ' ||  cin.peek() == '\t' ) { // 如果下一個字元也是空白或tab就讀入跳過 column++
          cin.get( inputChar ); // 讀入所有空白或tab
          loc ++ ; // 更改目前位置
        } // while
      } // else if

      //      if ( gTest ) cout << "@atom " << atom << endl;
      //      if ( gTest ) cout << "@onlyNIL " << onlyNIL << endl;
      if ( atom != ""  && !onlyNIL && ! ( nil && parentheses == 0 )  ) { // 字串非空字串

        atomNum ++;
        if ( gTest ) cout << "atomNum: " << atomNum << endl;
        if ( gTest ) cout << "IN atom NOT NULL: " << atom << endl;
        if ( atomNum == 1  ) { // 在第一個atom
          if ( gTest ) cout << "#parentheses: " << parentheses << endl;
          gP_NUM = parentheses;
          firstatom = true; // 表示為第一個atom
        } // if
        else firstatom = false;


        if ( atom == "exit" && preP ) {
          if ( inputChar == ')' ) whileNUM = 1;
          else {
            while ( cin.peek() == ' ' || cin.peek() == '\t' ||  cin.peek() == '\n' ) {
              cin.get( inputChar ); // 讀入所有空白或tab
            } // while

            cin.get( inputChar );
            if ( inputChar == ')' ) whileNUM = 1;
          } // else
        } // if
        else preP = false;
          //        if ( !firstAtom ) {
          //          // cout << "parentheses: " << parentheses << endl;
          //          gP_NUM = parentheses;
          //          firstAtom = true; // 尚未初始化
          //        } // if

        column = loc ; // column 改為下token的首字位置
        // cout << "atom: " << atom << endl;
        if ( atom == "'" ) {
          atom = "quote";
        } // if

        if ( gTest ) cout << "atom: " << atom << " inputchar: " << inputChar << endl;
        //        if ( gTest ) cout << "pNUM: " << pNUM << endl;
        if ( gTest ) cout << "isParentheses: " << isParentheses << endl;
        if ( isParentheses  ) { // 代表有"(" : 為串列，要建樹
          //          if ( gTest ) cout << "為串列，要建樹 " << endl;
          gtokenItem.atom = atom ;
          GetTokenType( gtokenItem.atom );
          atom = gtokenItem.atom ;
          //          if ( gTest ) cout << atom << endl;
          if ( dot_NP ) {
            gnow->rightPItem.atom = gtokenItem.atom;
            if ( gTest ) cout << "pNUM: " << pNUM << endl;
            if ( pNUM != 0 && !dot && !firstatom && !nil ) {
              // cout << "prechar " << prechar << endl;
              gnow->rightPItem.parenthesesNUM = pNUM;
              if ( gTest ) cout << "SAVE PNUM: " << pNUM << endl;
              if ( gTest ) cout << "!! PNUM -> 0 !!" << pNUM << endl;

            } // if

            pNUM = 0;
            // dot_NP = false;
            if ( gTest ) cout << "## SaveRight: " << gnow->rightPItem.atom << endl;
            if ( newNode ) {
              newNode = false;
              gnow->right = NULL;
              if ( gTest ) cout << "new node but dot & gnow->right = NULL" << endl;
            } // if

            dot_NP = false;
          } // if
          else {
            if ( newNode ) {
              if ( gnow->right != NULL ) gnow = gnow->right;
              newNode = false;
              if ( gTest ) cout << "Point To gnow->right" << endl;
              //              dot = false;
            } // if

            if ( dot_NP ) { // '.'後面NO'(' : 不用建立新節點 存入右token
              gnow->rightPItem.atom = gtokenItem.atom; // 存入right邊字串
              // if ( pWithoutDot && !nil && !firstatom ) gnow->rightPItem.parenthesesNUM = pNUM;
              if ( gTest ) cout << "pNUM: " << pNUM << endl;
              if ( pNUM != 0 && !dot && !firstatom && !nil ) {
                // cout << "prechar " << prechar << endl;
                gnow->rightPItem.parenthesesNUM = pNUM;
                if ( gTest ) cout << "SAVE PNUM: " << pNUM << endl;
              } // if

              pNUM = 0;

              if ( gTest ) cout << "## SaveRight: " << gnow->rightPItem.atom << endl;
              dot_NP = false;
            } // if
            else {
              if ( gnow != NULL ) gnow->leftPItem.atom = gtokenItem.atom; // 存入左邊字串
              // if ( pWithoutDot && !nil && !firstatom  ) gnow->leftPItem.parenthesesNUM = pNUM;
              if ( gTest ) cout << "pNUM: " << pNUM << endl;
              if ( pNUM != 0 && !dot  && !firstatom && !nil ) {
                gnow->leftPItem.parenthesesNUM = pNUM;
                if ( gTest ) cout << "SAVE PNUM: " << pNUM << endl;

              } // if

              pNUM = 0;
              if ( gTest ) cout << "## SaveLEFT: " << gnow->leftPItem.atom << endl;
            } // else
          } // else

          // cout << "inputChar: " <<  inputChar << endl;
          if ( ( cin.peek() != ')' && inputChar != ')' ) || ( nil && cin.peek() != ')' ) ) {
            // 下一個字元或現在這個字元不是 ')' 表示到結尾不用在建立新右節點
            //            cout << inputChar <<endl;
            newNode = true;
            gnow->right = new Node;
            gnow->right->left = NULL;
            gnow->right->right = NULL;
            //            gnow = gnow->right ;
            InitPtoken( gnow->right );
            if ( gTest ) cout << "## NEW_Right ##" << endl;
          } // if

          if ( parentheses == 1  ) {
            preNode = ghead;
            // cout << "^^ preNode = head; ^^ " << endl;
          } // if

          if ( gTest ) cout << "dot = false" << endl;
          pWithoutDot = false;
          dot = false;
        } // if ( isParentheses )
        else { // 代表有沒有"(" : 存入vector
          loc = 1; // 因為遇到空白才分割 所以已經讀入一個空白ㄌ
          if ( gTest ) cout << "~~VECTOR~~" << endl;
          gtokenItem.atom = atom ;
          GetTokenType( gtokenItem.atom );
          gData.push_back( gtokenItem );
          line = 1;
        } // else

        atom = "" ;
        float_end = false;
        //        before_dQuote = false;
      } // if

      if ( inputChar == ')' && whileNUM != 1 ) {
        if ( gTest ) cout << "Encounter ')' && whileNUM != 1 "  << endl;

        if ( gTest ) cout << "onlyNIL " << onlyNIL << endl;
        if ( parentheses == 0 && isParentheses ) {
          if ( gTest ) cout << "parentheses == 0: " << gP_NUM << endl;
          atomNum = 0 ;
          if ( cin.peek() != '\0' ) {
            // cout << "prettyprint: \n\n";
            if ( !gwelcom ) {
              cout << "Welcome to OurScheme!" << endl;
              gwelcom = true;
            } // if

            if ( error ) {
              cout << "\n> " << errorString << endl;
              error = false;
              errorString = "";
            } // if
            else {
              if ( !nil ) cout << "\n> ( " ;
              PrettyPrint( ghead );
              if ( !nil ) cout << ")\n" ;
            } // else


          } // if

          line = 1;
          Init();
          nil = false ;
          newNode = false ;
          isParentheses = false;
          dot = false;
          dot_NP = false ;
          preNode = NULL;
          atom = "" ;
          //          parentheses = 0 ;
        } // if(parentheses == 0 )
      } // if(inputChar == ')')

      if ( inputChar == '\n' ) { // 換行 line ++ ; column 歸零
        //          cout << " \n line ++ : " << line;
        line++ ;
        if ( gTest ) cout << "## line:" << line << endl;
        column = 0 ;
        loc = 1 ;
        onlyNIL = false;
        if ( gTest ) cout << "CountP:" << parentheses << endl;
        if ( isParentheses && parentheses == 0 ) {
          // cout << "prettyprint: \n";
          cout << "\n> ( " ;
          PrettyPrint( ghead ) ;
          cout << ")\n" ;
          atomNum = 0;
          line = 1;
        } // if
        else if ( !isParentheses ) {
          if ( parentheses < 0  ) {
            // cout << "error: gdata.size " << gData.size()  << endl;
          } // if

          atomNum = 0;
          if ( gData.size() > 0 ) {
            VectorPrint();
            line = 1;
          } // if

        } // else if
        //        Init();
        atom = "" ;
      } // if

    } // if 遇到 WS, (, ), ', ", ; 開始處理atom字串

    else if ( inputChar == '.' && atom == "" ) { // 遇到.要在同一層
      if ( parentheses == 0 && atom == "" && isspace( cin.peek() )  ) {
        // 只有點且前後都是空白     !@  .  ( a . b ) 要是error 且.後面的東西都不要
        gtokenItem.atom = "ERROR (unexpected token) : atom or '(' expected when token at Line "
                          + IntTOString( line ) + " Column " + IntTOString( loc ) + " is >>.<<";
        GetTokenType( gtokenItem.atom );
        gData.push_back( gtokenItem );
        // cout << "ERROR" << " loc: " << loc << endl;
        if ( gTest ) cout << "dot and space" << endl;
        while ( cin.peek() != '\n' ) { // .後面的東西直到換行都不要
          cin.get( inputChar );
        } // while
      } // if
      else {
        if ( gTest ) cout << "ATOM " << atom << endl;
        if ( cin.peek() != ' ' ||  cin.peek() != '\t' || cin.peek() != '\n' ) dotWithSpace = false;
        if ( atom == "" ) { // 若.前面為空字串，表示不是浮點數
          while ( cin.peek() == ' ' ||  cin.peek() == '\t' || cin.peek() == '\n' ) {
            // 如果下一個字元也是空白或tab就讀入跳過 loc++
            cin.get( inputChar ); // 讀入所有空白或tab
            // cout << "dot_With_Space" << endl;
            dotWithSpace = true;
            loc ++ ; // 更改目前位置
            if ( inputChar == '\n' ) {
              loc = 1;
              line++ ;
            } // if
          } // while
        } // if

        if ( cin.peek() == ')' ) { // .後面為右括號的情況
          if ( gTest ) cout << "ERROR " << "line " << line << " loc " << loc  << endl;
          error = true;
          errorString = "ERROR (unexpected token) : atom or '(' expected when token at Line "
                        + IntTOString( line ) + " Column " + IntTOString( loc ) + " is >>)<<" ;
        } // if

        if ( cin.peek() == ';' ) { // .後面為分號的情況
          while ( cin.peek() != '\n' ) {
            cin.get( inputChar );
          } // while

          // line++;
          while ( cin.peek() == ' ' ||  cin.peek() == '\t' || cin.peek() == '\n' ) { // 在讀掉分號換行後的空白
            // 如果下一個字元也是空白或tab就讀入跳過 column++
            cin.get( inputChar ); // 讀入所有空白或tab
            // cout << "dot_With_Space" << endl;
            dotWithSpace = true;
            loc ++ ; // 更改目前位置
            if ( inputChar == '\n' ) line++;
          } // while

        } // if .後面為分號的情況

        // cout << "NIL-> " << nil << endl;
        if ( cin.peek() == '(' && atom == "" ) { // '.'後面接'('：要建立右邊的新節點
          dot = true ;
          if ( gTest ) cout << "!!dot " << dot << endl;
          if ( !rightP ) { // 表示.前面有一個串列還沒結束 所以preNode = gnow
            preNode = gnow;
            rightP = false;
          } // if
          else {
            if ( preNode->leftPItem.atom != "" && gnow != ghead ) { // 如果preNode的左邊已經有存字 就指向右下新的節點
              gnow = preNode->right ;
              // cout << " gnow = preNode->right ;" << endl;
            } // if
            else {
              gnow = preNode ; // 表示.前面有一個串列“已經”結束 所以preNode = gnow
              // cout << "gnow = preNode " << endl;
            } // else
          } // else
        } // if '.'後面接'('：要建立右邊的新節點
        else { // '.'後面NO'(' : 不用建立新節點 存入右token;
          if ( dotWithSpace ) {
            dot_NP = true;
            dotWithSpace = false;
          } // if
          else {
            atom += inputChar ;
            //          cout << "@@ @@" << atom << endl;
          } // else
        } // else
      } // else

    } // else if
    else if ( inputChar == ';' ) { // 分號後都不要
      while ( cin.peek() != '\n' ) {
        cin.get( inputChar );
        //        cout << inputChar << " $ " << endl ;
      } // while

      //      line++; 不用！
    } // else if
    else {
      atom += inputChar ; // 不是 WS, (, ), ', ", ; 才加入atom字串
    } // else

  } // while


} // ReadSExp()


int main( void ) { // int argc, const char * argv[]
  // insert code here...
  char test;
  char testNum ;
  string text ;
  if ( cin.peek() == '(' ) {

  } // if

  cin.get( testNum );
  // cin.get( testNum ); // \n
  // ReadSExp();
  if ( testNum == '1' ||  testNum == '2' ) {
    cin.get( testNum ); // \n
    ReadSExp();
  } // if
  else if ( testNum == '3' ) {
    cout << "RRRR" << endl;
  } // else if


  if ( gErrorEOF ) {
    if ( gData.size() > 0 ) {
      VectorPrint();
    } // if

    cout << "\n> ERROR (no more input) : END-OF-FILE encountered" << endl;
    gErrorEOF = false;
    cout << "Thanks for using OurScheme!" << endl;
  } // if
  else {
    if ( gData.size() > 0 ) {
      VectorPrint();
    } // if

    cout << "\n> \nThanks for using OurScheme!" << endl;
  } // else
  //  vectorPrint();
  //  prettyPrint();
  //  print();
  return 0;
} // main()


