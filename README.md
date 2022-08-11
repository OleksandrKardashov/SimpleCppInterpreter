# SimpleCppInterpreter
An interpreter for the custom programming language in the form of a console application, written in C++

This console application allows you to execute the code of a simple programming language (hereinafter MyLan), according to the given grammar rules. The application gives the user the opportunity, for educational purposes, to see each stage of the interpreter, consisting of several main blocks:
- Lexical analyzer
- Syntactic analyzer
- Reverse Polish notation (RPN) building block
- RPN execution block

![image](https://user-images.githubusercontent.com/51093494/184239126-ada625d9-594b-4095-9c09-d6394b129ee1.png)

The interpreter performs lexical analysis using the **transition diagram method**.
Syntactical analysis performs using a **bottom-up parsing method based on precedence relationships**.
Reverse Polish Notation is built using **Dijkstra's algorithm**.


## Language overview
The MyLan language includes the following features:
- The data type for performing mathematical operations with floating point numbers, similar to the *float* data type. It is also allowed to use numbers in integer form.
- Performing basic mathematical operations - addition, subtraction, multiplication, division.
- Performing comparison operations (>, <, >=, <=, !=, ==), but without the possibility of using the logical operators AND (&&), OR (||).
- Initialization of variables and the ability to write data into variables.
- Execution of a cyclic operatot **for**. The syntax of the cyclic operator is as follows:
  ```
  for <idn>=<exp> by <exp> to <exp> do
  <operator list>
  rof
  ```
- Execution of a conditional choice operator **if** with this syntax:
  ```
  if <relation> then
  <operator list>
  fi
  ```
- Execution of the unconditional transition operator goto:
  ```
  goto <@label>
  ```
  
  
## GRAMMAR
Programming language functionality are defined by the grammar based on Extended Backus-Naur Form (EBNF). Below are all the grammar rules of the MyLan language:
```
Program := "program" "id" "begin" ListOper_1 "end".

ListOper_1 := ListOper.
ListOper := Oper "¶".
ListOper := Oper "¶" ListOper.

Oper := Declaration.
Oper := "lab".
Oper := CondTransition.
Oper := Cycle.
Oper := Initialization.
Oper := Input.
Oper := Output.
Oper := IncondTransition.
Oper := Relation.
Oper := Expression.

Declaration := "type" "id".
CondTransition := "if" Relation "then" ListOper_1 "fi".
Cycle := "for" Initialization "by" Expression "to" Expression "do" ListOper_1 "rof".
Initialization := "id" "=" Expression.
Input := "cin" ">>" "id".
Output := "cout" "<<" Expression.
IncondTransition := "goto" "lab".
Relation := Expression "comparisonSign" Expression.

Expression := Term.
Term := Term_1.
Term := Term "+" Term_1.
Term := Term "-" Term_1.
Term_1 := Mult.
Mult := Mult_1.
Mult := Mult "*" Mult_1.
Mult := Mult "/" Mult_1.
Mult_1 := "id".
Mult_1 := "fixConst".
Mult_1 := InPar.
InPar := "(" Expression ")".
```


## USING
The interpreter is designed as a console application, which, in addition to executing the program code of the MyLan language, makes it possible to manually specify the necessary input files for parsing and track each step in the process of performing lexical and syntactic analysis, as well as performing program code operations.
All of these options are available using *keys* when running a console application. For each key is defined the full and abbreviated form:
- **--codePath/-cp** – specifies the path to the file with the input program code of the MyLan language
- **--grammarPath/-gp** – specifies the path to the MyLan language grammar file
- **--info/-i** – display the process of parsing and executing the program code of the MyLan language
- **--help/-h** – displays a quick guide to using the application

There is no strictly defined name for each file; for the program to work, it is enough that the contents of the files indicated after the keys are correct. Also, the order in which multiple keys are used can be arbitrary:
```
SimpleCppInterpreter.exe -gp “D:\SomeFolder\GrammarText.txt” -codePath “D:\MyCode.txt” -i
SimpleCppInterpreter.exe -cp “D:\ SomeFolder\MyCode.txt” –h
```

The use of keys when starting the application is optional. Without a manually specified path to a necessary file, the application requires each file to be in the appropriate folder, in the same directory with the application's executable file SimpleCppInterpreter.exe:
- File with program code of MyLan language: ./ProgramCode/DemoCode.txt
- MyLan language grammar file: ./Resources/Grammar.txt
- File with input table of MyLan language lexemes: ./Resources/Lexem.txt

  <img src="https://user-images.githubusercontent.com/51093494/184240243-f0133d42-faa8-4189-a1e1-56a27bca7b3d.png" width="250" height="125">

When the program is executed, two more folders are created - OutLAnalyzer and OutRPN, in which the output files generated after the lexical analyzer and the Reverse Polish Notation creation process are placed.


## OUTPUT OF THE APPLICATION
The results of the application execution will be demonstrated using the default input data as an example.
The executable program code of the MyLan language:

<img src="https://user-images.githubusercontent.com/51093494/184240349-9d5145fe-41a2-4177-ad4f-01d148c9b780.png" width="450" height="250">

If you use the key --info/-i when starting the application, then the following data will be displayed one by one:
- Grammar of the MyLan language, which the application received as input:
  ![image](https://user-images.githubusercontent.com/51093494/184240424-964fd379-1030-4abf-8331-eff806711a80.png)

- Next, precedence relations EQUAL, FIRST+, LAST+ are displayed, which created on the basis of parsing the input grammar:
  ![image](https://user-images.githubusercontent.com/51093494/184240474-69dca811-fe21-4c47-863f-029e29e49853.png)
  ![image](https://user-images.githubusercontent.com/51093494/184240500-06b45076-a5e1-4d7f-afad-8720c15c3f4b.png)
  ![image](https://user-images.githubusercontent.com/51093494/184240519-0ac45188-0fdd-4e5b-8c0b-cc447926778e.png)

- Table of relations between each syntax construct of the MyLan language (=, >, <):
  ![image](https://user-images.githubusercontent.com/51093494/184240597-c6d61a18-2522-4249-bc2c-e9bdfee93c04.png)

- Program code in the form of Reverse Polish Notation:
  ![image](https://user-images.githubusercontent.com/51093494/184240654-683bc8b8-579e-4c56-bf9a-30e63985c063.png)

- After that, there is a process of executing the program code that was submitted as input:
  ![image](https://user-images.githubusercontent.com/51093494/184240705-436fb5b1-06a2-4597-ace5-cd2e6ef0cf8b.png)
