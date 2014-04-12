         Class Tree Builder Version 2.91
           (C) Sergey I. Yevtushenko.

1. Description.
 The main purpose of Class Tree Builder utility is displaying the
inheritance of C++ classes. Utility scans header files for declaration of
classes and builds the tree. Another useful features is displaing pairs
(file,line) where class declared and class methods with short descriptions
(like 'constructor', 'destructor', 'virtual', 'static', etc.)
 Utility is very fast: scanning OWL 2.0 (From Borland C++ for OS/2 Version 2.0)
needs as many as 6-8 seconds on my P-120.

2. Supported platforms.
 The first versions was written for DOS. Now support and maintenance of
DOS dropped (because of lack of any DOS compiler on my computer, and I hope
never be). Current version is OS/2 only. Future versions may also support
Linux. Windows means DOS ;)

3. Distribution
 Utility can be free distributed as long as this file and copyright notice
is a part of distribution package.

3. Limitations.
 Because of lack of built-in full C preprocessor mistakes are possible :(
 Also possible existance of number of bugs. Anyway if you find them
let me know. See addresses below.

4. Command line switches.
    CTB [-{iwsnma} [-Ipath{;path}] [-Sname] [-oname[.def]] [path\]filespec[.h]

-i      Ignore #include directives in processed files.
-w      Disable printing of warning messages
-s      Enables printing of pairs (file,line) for classes
-Ipath  Include files search path. Several paths can be separated by ';'
-Sname  Print only subtree which begins from class <name>
-oname  Redirect output into file <name>. If extension not specified CTB
        assumes .DEF
-c      Threat 'struct' and 'union' as 'class'
-a      Print all found declarations. If this option not specified CTB prints
        only classes with childrens.
-n      Desable print of tree of classes. Useful with -a and -m
-m      Print methods of classes.

5. Warrantyes.
 Class Tree Builder is distributed with no warranty whatever. The author and
any other contributors take no responsibility for the consequences of its use.

6. Author
 This utility designed and coded by Sergey I. Yevtushenko.
 Author can be reached
    E-mail: es@noos.kiev.ua
    FIDO: 2:463/114.69@fidonet

 Suggestions, comments, contributions and donations are welcome ;)
 No support for flames ;)

7. Future of CTB
 I hope that the version 3.0 will be true C++ Browser utility, with GUI and
 with true C++ preprocessor and parser (this can reduce the risk of
 mistake when parsing code). Maybe with built-in class editor.

8. History
   Note: there is lack of implementation history for versions prior to 2.5,
   because I simply forget it ;)

  V 2.91 vs 2.9

  1. Changes for more clean long file names support.
  1. English documentation.

  V 2.9 vs 2.8

  1. Fixed problems with -I switch.
  2. Fixed check of parenthese balance.

  V 2.8 vs 2.7

  1. Added printiong of class methods.
     Added command line switches -n, -m.
  2. Some minor fixes (compiler independence).

  V 2.7 vs 2.6

  1. Added sorting of class names
    (Used alghorithm of sigle-linked lists sorting from SNIPPETS.)

  V 2.6 vs V 2.5

  1. Fixed bug in parsing file name when processing include statements
  2. Changed parser algorithm
  3. Fixed bug in include paths parsing code
  4. Overloaded operator new

  V 2.5 vs previous versions

  1. Fixed bugs in command line parsing code.
  2. Changed purpose of switch -s
  3. Added printing of file name and line number where class is declared
  4. 'struct' and 'union' can be processed as 'class'.
  5. Fixed printing of base classes in case of multiple inheritance.

