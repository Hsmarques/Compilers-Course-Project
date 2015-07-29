# Pascal-Compiler-Project
An attempt to create a Pascal Compiler for the Compilers Course in Informatics Engineering's Bachelor’s degree in Universidade de Coimbra

It was developed through a semester and it was divided in 4 parts:

 1st - Lexical analysis, using the lex tool we implemented code that analysed inputed code and detected any lexical errors. 
 
 2nd - Syntactic analysis, using the yacc tool and the previous part with lex we constructed a grammar for this language and implemented the construction of the syntax tree and detection of any syntatic errors.;
 
 3th - Semantic analysis, using the previous parts we implemented the detection of any semantic errors as well as the lexical and syntatic ones;
 
 4th - Code generation, using c language we implemented code that outputs LLVM on it's intermediate representation.
 
