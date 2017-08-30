#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
enum Type {
    Boolean,
    Null,
    Undefined,
    Number,
    String,
    Symbol,
    Object
};

enum Scoping {
    Global,
    Var,
    Let,
    Const
};

enum Operator {
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Modulus,
    Increment,
    Decrement,
    Assignment,
    CompoundAddition,
    CompoundSubtraction,
    CompoundMultiplication,
    CompoundDivision,
    CompoundModulus,
    Equality,
    TypedEquality,
    Inequality,
    TypedInequality,
    GreaterThan,
    LessThan,
    GreaterThanOrEqualTo,
    LessThanOrEqualTo,
    Ternary,
    LogicalAnd,
    LogicalOr,
    LogicalNot,
    And,
    Or,
    Not,
    Xor,
    LeftShift,
    RightShift,
    Typeof,
    Delete,
    In,
    InstanceOf,
    None
};

class Expression {
public:
    Expression *RightHand;
    Expression *LeftHand;
    Operator Op;
};
class Variable : public Expression {
public:
    Type T;
    size_t Size;
    void* Ptr;
    string Identifier;
    Scoping Scope;
    string *Index;
};

class Function : public Expression {
public:
    string Name;
    vector<Expression*> Expressions;
    vector<Variable> Arguments;
};

class FunctionCall : public Expression {
public:
    string name;
    vector<Variable> Arguments;
};

class Conditional : public Expression {
    Expression *Eval;
    vector<Expression> Expressions;
};

enum Token {
    Operator = -1,
    ScopeDecl = -2,
    Name = -3,
    FunctionDecl = -4,
    EndOfFile = -6,
    StringValue = -7,
    NumberValue = -8
};

class Parser {
public:
    Parser(fstream &f) {
        File = std::move(f);
    }

    int NextToken() {
        while(isspace(LastCharacter)){
            LastCharacter = File.get();
        }
        if(isalpha(LastCharacter)){
            LastLexeme = LastCharacter;
            while(isalnum(LastCharacter = File.get()) || LastCharacter == '_') {
                LastLexeme += LastCharacter;
            }
            if((LastLexeme == "let") || (LastLexeme == "var") || (LastLexeme == "const")){
                LastToken = ScopeDecl;
                return LastToken;
            }
            if(LastLexeme == "function"){
                LastToken = FunctionDecl;
                return LastToken;
            }
            LastToken = Name;
            return LastToken;
        }
        if(LastCharacter == '+') {
            LastCharacter = File.get();
            if(LastCharacter == '+') {
                LastOperator = Increment;
                LastToken = Operator;
                return LastToken;
            }
            LastOperator = Addition;
            LastToken = Operator;
            return LastToken;
        }

        if(LastCharacter == '-') {
            LastCharacter = File.get();
            if(LastCharacter == '-') {
                LastOperator = Decrement;
                LastToken = Operator;
                return LastToken;
            }
            LastOperator = Subtraction;
            LastToken = Operator;
            return LastToken;
        }

        if(LastCharacter == '=') {
            LastCharacter = File.get();
            if(LastCharacter == '=') {
                LastCharacter = File.get();
                if(LastCharacter == '=') {
                    LastOperator = TypedEquality;
                    LastToken = Operator;
                    return LastToken;
                }
                LastOperator = Equality;
                LastToken = Operator;
                return LastToken;
            }
            LastOperator = Assignment;
            LastToken = Operator;
            return LastToken;
        }
        int CurrentCharacter = LastCharacter;
        LastCharacter = File.get();
        if(LastCharacter == EOF) {
            LastToken = EndOfFile;
            return EndOfFile;
        }
        LastToken = CurrentCharacter;
        return CurrentCharacter;

    }

    Expression* ParseFunctionDecl() {
        Function *f = new Function;
        int t = NextToken();
        if(t != Name) {
            cout << "Error while parsing function definition!" << endl;
            exit(-1);
        }
        f->Name = LastLexeme;
        t = NextToken();
        if(t != '(') {
            cout << "Error while parsing function definition!" << endl;
            exit(-1);
        }
        while(t != ')'){
            t = NextToken();
            if(t != Name) {
                cout << "Error while parsing function definition!" << endl;
                exit(-1);
            }
            Variable v;
            v.Identifier = LastLexeme;
            v.Scope = Var;
            v.T = Undefined;
            f->Arguments.push_back(v);
            t = NextToken();
            if((t == ',') || (t == ')')) {
                continue;
            } else {
                cout << "Error while parsing function definition!" << endl;
                exit(-1);
            }
        }
        t = NextToken();
        if(t != '{') {
            cout << "Error while parsing function definition!" << endl;
            exit(-1);
        }
        while(t != '}'){
            t = NextToken();
            f->Expressions.push_back(ParseExpression());
        }
    }
    Expression* ParseNameExpression() {
        int t;
        Expression *e = new Expression;
        t = NextToken();
        if(t == Operator) {
            Variable *v = new Variable;
            v->Identifier = LastLexeme;
            if(LastOperator == Assignment) {
                e->RightHand = v;
                e->LeftHand = ParseExpression();
                e->Op = Assignment;
            }
        } else if(t == ';') {
            Variable *v = new Variable;
            v->Identifier = LastLexeme;
            e->RightHand = v;
            e->LeftHand = NULL;
            e->Op = None;
            return e;
        } else if(t == '(') {
            FunctionCall *call = new FunctionCall;
            call->name = LastLexeme;
            while(t != ')'){
                t = NextToken();
                if(t != Name) {
                    cout << "Error while parsing function call!" << endl;
                    exit(-1);
                }
                Variable v;
                v.Identifier = LastLexeme;
                v.Scope = Var;
                v.T = Undefined;
                call->Arguments.push_back(v);
                t = NextToken();
                if((t == ',') || (t == ')')) {
                    continue;
                } else {
                    cout << "Error while parsing function call!" << endl;
                    exit(-1);
                }
            }
        }
    }

    Expression* ParseVariable() {
        int t;
        Expression *e = new Expression;
        Variable *v = new Variable;
        if(LastLexeme == "let") {
            v->Scope = Let;
        } else if(LastLexeme == "var") {
            v->Scope = Var;
        } else if(LastLexeme == "const") {
            v->Scope = Const;
        }
        t = NextToken();
        if(t != Name) {
            cout << "Error while parsing Variable!" << endl;
            exit(-1);
        }
        v->Identifier = LastLexeme;
        e->RightHand = v;
        e->LeftHand = NULL;
        e->Op = None;
        t = NextToken();
        if(t == ';'){
            return e;
        }
        else if(t == Operator) {
            if(LastOperator == Assignment) {
                e->LeftHand = ParseExpression();
                return e;
            }
        }
    }

    Expression* ParseExpression() {
            switch(LastToken) {
                case ScopeDecl:
                    return ParseVariable();
                case FunctionDecl:
                    return ParseFunctionDecl();
                case Name:
                    return ParseNameExpression();
                case '}':
                    return NULL;
                default:
                    return NULL;
             }
    }

    void Parse() {
        while(true) {
            int t = NextToken();
            if(t == EndOfFile) {
                exit(-1);
            }
            Expressions.push_back(ParseExpression());
        }
    }

private:
    int LastOperator;
    int LastToken;
    string LastLexeme;
    int LastCharacter = ' ';
    vector<Expression*> Expressions;
    fstream File;
};

int main(int argc, char *argv[])
{
    fstream f;
    if(argc > 1) {
        f.open(argv[1]);
        Parser p(f);
        p.Parse();
    }
    return 0;
}
