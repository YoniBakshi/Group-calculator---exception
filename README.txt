				~~~ Group Calculator ~~~
==============================================================================
Description : 
This program is a Group Calculator. 
Overall, there are 5 operations available : Union, Intersection, Difference, Product, Composite
Program will start and print the list of 3 basic operations on 2 groups which are :  Union, Intersection, Difference.
User can add operation to list of use the calculator groups :
- Use the calculator by writing the command "eval ..." (number of wanted operation) , 
enter a serie of numbers as group when the first number is the size of current receiving group and so on.
Print the input splitted to groups, operations of each between them and final result. 
- Add operations to the list by writing the wanted operations and two listed operations from the list. 
- Read commands from a text file. all parameters of a command needs to be written in the same row. (Notice also while using eval/resize , all parameters in one row).
			* fulfilled reading another text file from a text file.*
- Resize - Change the limited quantity of operation's list. 

Available commands are :
1. eval (num)	         - Calculate group according to entered num which represent the index from list.
2. uni (num1)(num2)        - AddUnion operation with 2 wanted indexes according to current operation's list
3. inter (num1)(num2 )     - Add Intersection operation with 2 wanted indexes according to current operation's list
4. diff (num1)(num2)       - Add Difference operation with 2 wanted indexes according to current operation's list
5. prod (num1)(num2)     - Add Product operation with 2 wanted indexes according to current operation's list
6. comp (num1)(num2)    - Add Composite operation with 2 wanted indexes according to current operation's list
7. read (File's path)         - Read commands from text file. All needed parameters needs to be written in the same row. 
8. resize                         - Choose a new limit for operation's 
9. del (num)	         - Delete specific operation from current operation's list. (Cannot delete 3 basic operations). 
10. help                          - Print all available commands with explanation for each.
11. exit                           - Finish & Exit program.
  
==============================================================================  

Files list :
Header Files :
BinaryOperation.h
Comp.h
Difference.h
Identity.h
Intersection.h
NameGenerator.h
NonCompBinaryOperation.h
Operation.h
Product.h
Set.h
SetCalculator.h
Union.h

.cpp Files :
main.cpp             		
BinaryOperation.cpp  
Comp.cpp  
Difference.cpp
Identity.cpp
Intersection.cpp
NameGenerator.cpp
NonCompBinaryOperation.cpp
Operation.cpp
Product.cpp
Set.cpp
SetCalculator.cpp
Union.cpp

==============================================================================

Extra Notes :
* Operation "read" is fulfilled - reading another text file from a text file. *

If command is too long - program will apply the valid command and ignore the rest.
If command is too short - program will throw an exception.

The thrown exception will be according to the first found error.

==============================================================================

  Full-Names : Yehonatan Bakshi & Hila Saadon
