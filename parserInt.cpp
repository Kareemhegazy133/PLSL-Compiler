/* Implementation of PLSL Interpreter
* parseInt.cpp
*/

#include <vector>
#include <string>
#include "parseInt.h"
#include "val.cpp"
#include "Lex.cpp"

using namespace std;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line, vector<string> &IdList);


//Prog ::= PROGRAM IDENT; DeclBlock ProgBody
bool Prog(istream& in, int& line)
{
	bool f1, f2;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			
			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() == SEMICOL) {
				f1 = DeclBlock(in, line); 
			
				if(f1) {
					f2 = ProgBody(in, line);
					if(!f2)
					{
						ParseError(line, "Incorrect Program Body.");
						return false;
					}
					
					return true;//Successful Parsing is completed
				}
				else
				{
					ParseError(line, "Incorrect Declaration Section.");
					return false;
				}
			}
			else
			{
				//Parser::PushBackToken(tok);
				ParseError(line-1, "Missing Semicolon.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}

//ProgBody ::= BEGIN {Stmt;} END
bool ProgBody(istream& in, int& line) {
	LexItem t = Parser::GetNextToken(in, line);
	//cout << t.GetLexeme() + " here in ProgBody" << endl;

	if(t != BEGIN){
		ParseError(line, "Missing BEGIN, Given Token is " + t.GetLexeme());
		return false;
	}

	bool stmt = Stmt(in, line);
	
	while(stmt){
		t = Parser::GetNextToken(in, line);
		if(t != SEMICOL){
			// Potential Bassel Bug
			line -= 1;
			ParseError(line, "Missing semicolon in Statement.");
			return false;
		}

		if(t == SEMICOL){
			t = Parser::GetNextToken(in, line);

			if(t == END){
				Parser::PushBackToken(t);
				return true;
			}
			else{
				Parser::PushBackToken(t);
			}
		}

		stmt = Stmt(in, line);

		if(!stmt){
			ParseError(line, "Syntactic error in Program Body.");
			return false;
		}
	}
	
	t = Parser::GetNextToken(in, line);

	if(t != END){
		ParseError(line, "Missing END, Given Token is " + t.GetLexeme());
		return false;
	}
	return stmt;
}

//DeclBlock ::= VAR {DeclStmt;}
bool DeclBlock(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	//cout << "in Decl" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	if(t == VAR)
	{
		status = DeclStmt(in, line);
		
		while(status)
		{
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				line--;
				ParseError(line, "Missing semicolon in Declaration Statement.");
				return false;
			}
			status = DeclStmt(in, line);
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok == BEGIN )
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else 
		{
			ParseError(line, "Syntactic error in Declaration Block.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Declaration Block.");
		return false;
	}
	
}//end of DeclBlock function

//DeclStmt ::= IDENT {, IDENT} : (INTEGER | REAL | STRING)
bool DeclStmt(istream& in, int& line)
{
	vector<string> tempVars;
	LexItem t;
	bool status = IdentList(in, line, tempVars);
	
	if (!status)
	{
		ParseError(line, "Incorrect variable in Declaration Statement.");
		return status;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t == COLON)
	{
		t = Parser::GetNextToken(in, line);
		if(t == INTEGER || t == REAL || t == STRING)
		{
			//cout << t.GetLexeme() << " token is " << t.GetToken() << endl;
			for(auto it = begin(tempVars); it != end(tempVars); it++){
				//cout << "The key added is " << *it << " with token " << t.GetToken() << endl;
				SymTable[*it] = t.GetToken();
			}
			return true;
		}
		else
		{
			ParseError(line, "Incorrect Declaration Type.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(t);
		return false;
	}
	
}//End of DeclStmt

//IdList:= IDENT {,IDENT}
bool IdentList(istream& in, int& line, vector<string> &IdList) {
	bool status = false;
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{
		//set IDENT lexeme to the type tok value
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
			IdList.push_back(identstr);
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = IdentList(in, line, IdList);
	}
	else if(tok == COLON)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return status;
	
}//End of IdentList

//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt
bool Stmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case WRITELN:
		status = WriteLnStmt(in, line);
		
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		
		break;
			
	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
}//End of Stmt

//WriteStmt:= WRITELN (ExprList)
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	//cout << t.GetLexeme() << " in WriteLn" << endl;
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);
	//cout << "Back in writeln" << endl;
	
	if( !ex ) {
		ParseError(line, "Missing expression after WriteLn");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	
	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	
	cout << endl;
    
    delete ValQue;
	return ex;
}//End of WriteLnStmt

//IfStmt ::= IF ( LogicExpr ) THEN Stmt [ELSE Stmt]
bool IfStmt(istream& in, int& line) {
	//cout << "Starting if stmt" << endl;
	bool ex=false, status ; 
	LexItem t;
	Value retVal;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	//cout << "retVal in IfStmt is " << retVal << endl;
	ex = LogicExpr(in, line, retVal);
	//cout << "retVal in IfStmt2 is " << retVal << endl;

	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
    
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != THEN)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}
	if(retVal.GetBool()){
		
		status = Stmt(in, line);
		//cout << "Executed" << endl;
		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}
	}
	else{
		while(t != ELSE && t != SEMICOL){
			t = Parser::GetNextToken(in, line);
			if(t == ELSE || t == SEMICOL){
				Parser::PushBackToken(t);
			}
		}
	}
	
	t = Parser::GetNextToken(in, line);
	if( t == ELSE ) {
		if(!retVal.GetBool()){
			status = Stmt(in, line);
			if(!status)
			{
				ParseError(line, "Missing Statement for If-Stmt Else-Part");
				return false;
			}
		}
		else{
			while(t != SEMICOL){
				t = Parser::GetNextToken(in, line);
				//cout << t.GetLexeme() << " x" << endl;
				if(t == SEMICOL){
					Parser::PushBackToken(t);
				}
			}
		}
		return true;
	}

	Parser::PushBackToken(t);
	return true;
}//End of IfStmt function

//Var ::= IDENT
bool Var(istream& in, int& line, LexItem & idtok)
{
	//called only from the AssignStmt function
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	idtok = tok;

	if (tok == IDENT){
		identstr = tok.GetLexeme();
		
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}

		
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssignStmt ::= Var := Expr
bool AssignStmt(istream& in, int& line) {
	Value retVal;
	bool varstatus = false, status = false;
	LexItem token, t;
	
	varstatus = Var( in, line, token);
	
	if (varstatus){
		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){
			status = Expr(in, line, retVal);
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
            // Do the compatible assignment expressions checking and then call the following line
			//cout<< "token is : " << token.GetLexeme() << endl;
			//cout << retVal << " and in map it is " << SymTable[token.GetLexeme()] << endl;

			// VINT is 0 == INTEGER is 2, VREAL is 1 == REAL is 6
			if((SymTable[token.GetLexeme()] == 2 || SymTable[token.GetLexeme()] == 6) && !retVal.IsString()){
				if(!retVal.IsInt() && !retVal.IsReal()){
					ParseError(line, "Illegal Assignment Operation");
					return false;
				}

				// If var should be an Int but retVal is Real
				if(SymTable[token.GetLexeme()] == 2 && retVal.IsReal()){
					float temp = retVal.GetReal();
					retVal.SetType(VINT);
					retVal.SetInt(temp);
				}

				// If var should be a Real but retVal is Int
				if(SymTable[token.GetLexeme()] == 6 && retVal.IsInt()){
					int temp = retVal.GetInt();
					retVal.SetType(VREAL);
					retVal.SetReal(temp);
				}
			}
			// VSTRING is 2 == STRING is 7
			else if(SymTable[token.GetLexeme()] == 7 && !retVal.IsString()){
				ParseError(line, "Illegal Assignment Operation");
				return false;
			}
			

			//cout << token.GetLexeme() << endl;
			if(retVal.IsString() && (SymTable[token.GetLexeme()] == SymTable[retVal.GetString()]) && (TempsResults.find(retVal.GetString()) != TempsResults.end())){
				Value varVal = Value(TempsResults[retVal.GetString()]);
				TempsResults[token.GetLexeme()] = varVal;
			}
			else if(retVal.IsString() && (SymTable[token.GetLexeme()] == 2 && (TempsResults.find(retVal.GetString()) != TempsResults.end()))){
				int temp = TempsResults[retVal.GetString()].GetReal();
				Value varVal = Value(temp);
				TempsResults[token.GetLexeme()] = varVal;
			}
			else if(retVal.IsString() && (SymTable[token.GetLexeme()] == 6 && (TempsResults.find(retVal.GetString()) != TempsResults.end()))){
				float temp = TempsResults[retVal.GetString()].GetInt();
				Value varVal = Value(temp);
				TempsResults[token.GetLexeme()] = varVal;
			}
			else if(SymTable[token.GetLexeme()] == 6 && !retVal.IsReal()){
				ParseError(line, "Illegal Assignment Operation");
				return false;
			}
			else if(SymTable[token.GetLexeme()] == 2 && !retVal.IsInt()){
				ParseError(line, "Illegal Assignment Operation");
				return false;
			}
			else{
				TempsResults[token.GetLexeme()] = retVal;
			} 
			
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	Value retVal;
	bool status = false;
	
	status = Expr(in, line, retVal);
	//cout << "Back in ExprList1" << endl;
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	//cout << retVal << endl;
    ValQue -> push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	//cout << tok.GetLexeme() << " in ExprList" << endl;
	
	while (tok == COMMA) {
		//cout << "Calling Expr in ExprList again" << endl;
		status = Expr(in, line, retVal);
		//cout << "Back in ExprList2" << endl;
        
        if(!status){
            ParseError(line, "Missing Expression");
            return false;
        }
       	//cout << retVal << endl;
		if(retVal.IsString() && (!TempsResults[retVal.GetString()].IsErr())){
			//cout << "here?" << endl;
			ValQue -> push(TempsResults[retVal.GetString()]);
		}
		else{
			//cout << "orhere?" << endl;
			ValQue -> push(retVal);
		}
        
        tok = Parser::GetNextToken(in, line);
	}
	//cout << tok.GetLexeme() << " in ExprList Final" << endl;
	//cout << "retVal in ExprList Final is " << retVal << endl;
	Parser::PushBackToken(tok);
	return true;
	
}

//Expr:= Term {(+|-) Term}
bool Expr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	//cout << "in Expr" << endl;
	bool t1 = Term(in, line, val1);
	//cout << "Back in Expr1" << endl;
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	
	//cout << "retVal in Expr 1 is " << retVal << endl;

	tok = Parser::GetNextToken(in, line);
	//cout << tok.GetLexeme() << " in Expr" << endl;

	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	//Evaluate: evaluate the expression for addition or subtraction
	while ( tok == PLUS || tok == MINUS ) 
	{
		t1 = Term(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		if(tok == PLUS)
		{
			//cout << val2.GetType() << endl;
			if(retVal.IsString() || val2.IsString()){
				if(TempsResults.find(retVal.GetString()) != TempsResults.end()){
					Value temp = TempsResults[retVal.GetString()];
					retVal = temp;
				}

				if(TempsResults.find(val2.GetString()) != TempsResults.end()){
					Value temp = TempsResults[val2.GetString()];
					val2 = temp;
				}
				
			}
			retVal = retVal + val2;

			//cout << retVal << " in Expr +" << endl;

			if(retVal.IsErr())
			{
				ParseError(line, "Illegal addition operation.");
				//cout << "(" << tok.GetLexeme() << ")" << endl;		
				return false;
			}
		}
		else if(tok == MINUS)
		{
			retVal = retVal - val2;

			//cout << retVal << " in Expr -" << endl;

			if(retVal.IsErr())
			{
				ParseError(line, "Illegal subtraction operation.");
				return false;
			}
		}
			
		tok = Parser::GetNextToken(in, line);
		//cout << tok.GetLexeme() << " in Expr2" << endl;
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
		
	}
	//cout << tok.GetLexeme() << " in Expr Final" << endl;
	//cout << "retVal in Expr Final is " << retVal << endl;
	Parser::PushBackToken(tok);
	return true;
}//end of Expr

//Term:= SFactor {(*|/) SFactor}
bool Term(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	bool t1 = SFactor(in, line, val1);
	//cout << "Back in Term1" << endl;
	LexItem tok;

	//cout << "val1 in Term is " << val1 << endl;

	if( !t1 ) {
		return false;
	}
	retVal = val1;

	//cout << "retVal in Term is " << retVal << endl;
	
	tok	= Parser::GetNextToken(in, line);
	//cout << tok.GetLexeme() << " in Term" << endl;
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		t1 = SFactor(in, line, val2);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		if(tok == MULT)
		{
			//cout << "retVal in Term Mult is " << retVal << " and val2 is " << val2.GetType() << endl;
			if(val2.IsString()){
				retVal = TempsResults[retVal.GetString()] * TempsResults[val2.GetString()];
			}
			else{
				retVal = TempsResults[retVal.GetString()] * val2;
			}
			
			//cout << "Crossed" << endl;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal multiplication operation.");
				//cout << "(" << tok.GetLexeme() << ")" << endl;		
				return false;
			}
		}
		else if(tok == DIV)
		{
			retVal = TempsResults[retVal.GetString()] / TempsResults[val2.GetString()];
			if(retVal.IsErr())
			{
				ParseError(line, "Run-Time Error-Illegal Division by Zero");
				return false;
			}
		}

		tok	= Parser::GetNextToken(in, line);
		//cout << tok.GetLexeme() << " in Term2" << endl;
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	//cout << tok.GetLexeme() << " in Term Final" << endl;
	//cout << "retVal in Term Final is " << retVal << endl;
	Parser::PushBackToken(tok);
	return true;
}

//SFactor ::= [(+ | -)] Factor
bool SFactor(istream& in, int& line, Value & retVal){
	LexItem t = Parser::GetNextToken(in, line);
	//cout << t.GetLexeme() << " in SFactor" << endl;
	bool status;
	int sign = 0;
	
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);
		
	status = Factor(in, line, sign, retVal);
	//cout << "Back in SFactor" << endl;
    //cout << "retVal in SFactor is " << retVal << endl;

    if(!status){
        ParseError(line, "Undefined Variable");
        return false;
    }
    
    if(sign == 1 || sign == -1){
		if(retVal.IsString() && (TempsResults.find(retVal.GetString()) != TempsResults.end())){
			Value temp = TempsResults[retVal.GetString()];
			retVal = temp;
		}
        if(!retVal.IsInt() && ! retVal.IsReal()){
            ParseError(line, "Illegal Operand Type for Sign Operator");
            return false;
        }
        
        if(retVal.IsInt()){
            retVal.SetInt(retVal.GetInt() * sign);
        }
        
        if(retVal.IsReal()){
            retVal.SetReal(retVal.GetReal() * sign);
        }
        
    }
    return status;
}

//LogicExpr ::= Expr (= | > | <) Expr
bool LogicExpr(istream& in, int& line, Value & retVal)
{
    Value val1, val2;
	bool t1 = Expr(in, line, val1);
	//cout << "val1 in LogicExpr is " << val1 << endl;
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	
	if ( tok == GTHAN  || tok == EQUAL  || tok == LTHAN)
	{
		t1 = Expr(in, line, val2);
		//cout << "val2 in LogicExpr is " << val2 << endl;
		if( !t1 ) 
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
		     
        if(tok == GTHAN){
			Value tempValue = TempsResults[val1.GetString()];

			//cout << tempValue << " " << val2 << endl;
			//cout << "The expression returned " << (tempValue > val2) << endl;
            retVal.SetType(VBOOL);
			retVal.SetBool((tempValue > val2).GetBool());
			
			//cout << retVal << " Final Logic" << endl;
        }
        if(tok == LTHAN){
            retVal = TempsResults[val1.GetString()] < val2;
        }
        if(tok == EQUAL){
            retVal = TempsResults[val1.GetString()] == val2;
        }
        if(retVal.IsErr()){
            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
            return false;
        }
		return true;
	}
	Parser::PushBackToken(tok);
	return true;
}

//Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal) {
	LexItem tok = Parser::GetNextToken(in, line);
	
	//cout << "retVal in Factor is " << retVal << endl;
	//cout << tok.GetLexeme() << " in Factor" << endl;

	if( tok == IDENT ) {
		string lexeme = tok.GetLexeme();
		//cout << lexeme << endl;
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable " + lexeme);
			return false;	
		}
        
        // Check if the var is assigned, so check if its in tempsResults and return error/false if its not
		
		if (TempsResults.find(lexeme) == TempsResults.end()){
			ParseError(line, "Using Undefined Variable " + lexeme);
			return false;	
		}

		retVal.SetType(VSTRING);
		retVal.SetString(tok.GetLexeme());
		return true;
	}
	else if( tok == ICONST ) {
		retVal.SetType(VINT);
		retVal.SetInt(stoi(tok.GetLexeme()));
		//cout << "retVal in Factor2 is " << retVal << endl;
		return true;
	}
	else if( tok == SCONST ) {
		retVal.SetType(VSTRING);
		retVal.SetString(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) {
        retVal.SetType(VREAL);
		retVal.SetReal(stof(tok.GetLexeme()));
		//cout << "retVal in Factor3 is " << retVal << endl;
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	//cout << tok.GetLexeme() << " in Factor2" << endl;

	return false;
}