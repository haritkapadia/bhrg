#ifndef CLASS_UTIL_HPP
#define CLASS_UTIL_HPP

// https://stackoverflow.com/questions/5256313/c-c-macro-string-concatenation
#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)

#define GetDecl(TYPE, A) TYPE A();
#define SetDecl(TYPE, A) void A(TYPE A);
#define GetSetDecl(TYPE, A) GetDecl(TYPE, A); SetDecl(TYPE, A);

#define Get(CLASS, TYPE, A) TYPE CLASS::A() {return PPCAT(_, A);}
#define Set(CLASS, TYPE, A) void CLASS::A(TYPE A) {PPCAT(_, A) = A;}
#define GetSet(CLASS, TYPE, A) Get(CLASS, TYPE, A); Set(CLASS, TYPE, A);


#endif
