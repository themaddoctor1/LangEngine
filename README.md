# LangEngine

A C library for creating programming languages.

## About

LangEngine is a collection of C code designed to allow a user to develop their own programming languages. Although the language is friendly for interpreted language development, it can also be used to design a compiler. The naming conventions and general layout is heavily inspired by EOPL (Essentials of Programming Languages, 3rd edition), which is the textbook I had available when I took our programming languages course. The library offers a framework for parsing BNF grammars into expressions, as well as evaluationg said expressions.

## Getting Started

This section covers the setup of the source, as well as using it to build a language. It is recommended that the user have some familiarity with the design of programming languages beforehand.

### Installation

To install the source, simply download it.

### Configuration

The source comes with a makefile for building the language, as well as the tests for the base program. The following make options are available:

```
# Builds object files and an executable.
make lang

# Removes object files built during compilation.
make clean

# Removes object files and the executable.
make fclean

# Rebuilds the libraries. Equivalent to 'make fclean all'
make re
```
The makefile also includes a variable ```LANG``` for naming the language. The variable will define the name of the executable, and is used to determine where to search for the language source.

### Development

Any source code for the language should be placed in a folder named `lang_files`, where `lang` is the value of the `LANG` variable. The code should include implementations of several functions that are required for interpreting and evaluating given programs. These functions include:

```
// Evaluates an expression given an environment
ExpVal value_of(Exp, Env);

// Creates the default environment.
Env init_env();

// Given an environment and a variable name, lookup the value of the variable.
ExpVal apply_env(Env, char*) ;

// Disposes of an environment
void dispose_env(Env);

// Disposes of a variable.
void dispose_exp(Exp);

// Delete an expressed value
void dispose_expval(ExpVal);

// Clone an expressed value
ExpVal clone_expval(ExpVal);

/*
 * Creates a BNF grammar for parsing. Implementation is not necessary if
 * the provided parsing is not used. Called by parse() the first time
 * it is executed.
 */
BnfGrammar generateGrammar();
```
These funtions are used internally by the source to parse and interpret given commands. However, designing these functions will generally require other functions to be written, such as functions that generate expressed values of a particular type, or environment types that utilize specific value formats. Versions of the source code will be packaged with a collection of sample languages that can be studied to understand how a language could be implemented.

#### Parsing

The program comes with a BNF parser that given a string and a BNF grammar in the form of a `BnfGrammar` will generate an expression in the form of an `Exp` that represents the statement, if such an `Exp` exists.

To implement a BNF grammar, a set of variables must be constructed. These are represented by the `BnfVariable` structure, which requires a statement to match, a filter for handling the parsing result, and a destructor for disposing of the filtered result should it need to be destroyed. The `BnfVariable` constructor function is as follows:
```
/**
 * name     - The numerical id of the variable. In an n-variable grammar, 0 <= name < n.
 * value    - The value of the variable that strings will be tested against.
 * filter   - A function that handles the result of parsing a sequence.
 * disposer - A function that can safely dispose of anything created by filter.
 *
 * precondition: value MUST represent a union statement.
 */
BnfVariable bnfVariable(int name, BnfStatement value, BnfFilter filter, void (*disposer)(void*));
```
There are several `BnfStatement` types that can be constructed. Although a `BnfVariable` must take a `bnfUnion`, there are various formats that can be used:

```
// Represents a literal that will be searched for during parsing.
BnfStatement bnfLit(char *lit);

// Represents a number. When parsed, provides an integer.
BnfStatement bnfNum();

// Represents an identifier. Identifiers are currently represented
// by characters in A-Z and a-z.
BnfStatement bnfId();

// Represents a BnfVariable that will be evaluated and then filtered.
BnfStatement bnfVar(int name);

// Represents a union of n BnfSequences.
BnfStatement bnfUnion(int n, ...);

// Represents a sequence of n BNF items.
BnfStatement bnfSeq(int n, ...);

// Alternate constructor for sequences.
BnfStatement bnfSeq(BnfStatement*);

// Represents an arbitrary number of values matching a sequence.
// Can be separated by an identifier if desired.
BnfStatement bnfArb(BnfStatement statement, BnfStatement delimiter);

// Used to "null-terminate" a statement. Only necessary for the variable with id 0.
BnfStatement bnfTrn();
```
By combining these definitions, it is possible to define an abstract syntax for a language. Note that it is not necessary to use this functionality, but may be helpful for formalizing the structure of the language.

#### Evaluation

The parser uses three main structure types to evaluate programs. These are:

* `Exp`
    * Short for 'expression'.
    * Used to store the structure of a program.
* `Env`
    * Short for 'environment'.
    * Used to represent the state of a program based on previous execution.
* `ExpVal`
    * Short for 'expressed value'
    * Used to represent the result of evaluating an `Exp` (expression).

The implementation of these types allows the evaluator to determine the value of a program.

## Samples

This repository includes sample code that demonstrates usage of the language engine. Any languages that are created will be described here.

### SimpleLang

SimpleLang is an interpreted language that is heavily inspired by the let lang from EOPL. It has all of the features of said language, but with slightly different syntax. The grammar is as follows:

| Name | Syntax | Description |
| --- | --- | --- |
| Difference | `- <expression> <expression>` | Subtracts one expression from another. |
| If-Else | `'if' <expression> 'then' <expression> 'else' <expression>` | Computes conditional statement. |
| Let | `'let' <identifier> 'be' <expression> 'in' <expression>` | Adds new variable, then computes expression with it. |
| Number | `<number>` | Evaluates to an integer. |
| Product | ` * <expression> <expression>` | Multiplies two expressions. |
| Sum | `+ <expression> <expression>` | Adds two expressions. |
| Variable | `<identifier>` | Evaluates to the value of a variable. |

### PemdasLang

PemdasLang is an interpreted language that is specifically designed to provide a working example of an order of operation. The grammar allows for the use of addition, subtraction, multiplication, division, and parentheses.

## Future

In the future, I intend to add more features to the compiler in order to make it easier to develop more complex languages. This includes, but is not limited to:

* Type checking
* More parsing features
