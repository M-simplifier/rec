add(a i32, b i32) i32 {
	return a + b;
}

main() i32 {
	return 0;
}

program := definition*
definition := ident args type body
args := "(" (arg ("," arg)*)? ")"
arg := ident type
type := "i32"
body := "{" stmt* "}"
stmt := "return" expr ";"
expr := unit (("+"|"-"|"*"|"/"|"%") expr)?
unit := num
      | ident
      | fc
fc := ident params
params := "(" (expr (, expr)*)? ")"

ND := Program | Definition | Arg | Type | Body | Stmt | Expr

NDProgram {
	definitions *NDDefinition
}

NDDefinition {
	ident *NDExprIdent
	args *NDArg
	type *NDType
	body *NDBody
	next *NDDefinition
}

NDArg {
	ident *NDExprIdent
	type *NDType
	next *NDArg
}

NDType {
}

NDBody {
	stmt *NDStmt
}

NDStmt {
	expr *NDExpr
	next *NDStmt
}

NDExpr {next *NDExpr} := Binary | Ident | Num | Fc

NDExprBinary {
	lhs *NDExpr
	rhs *NDExpr
	ope string
}

NDExprIdent {
	ident string
}

NDExprNum {
	num int
}

NDExprFc{
	ident *NDExprIdent
	params *NDExpr
}

TK := Keyword | Ident | Num | Punct | TKEof

TKKeyword {
	keyword string
}

TKIdent {
	ident string
}

TKNum {
	num int
}

TKPunct {
	punct string
}

TKEof {}

------------AST to C

--Program

definition

definition

--Definition

type ident(arg, arg) {
	body
}

--Arg

type ident

--Type

int

--Body

stmt ;
stmt ;

--Stmt

return expr

--ExprBinary

lhs op rhs

--ExprNum

num

--ExprIdent

ident

--ExprFc

ident (expr, epxr)

