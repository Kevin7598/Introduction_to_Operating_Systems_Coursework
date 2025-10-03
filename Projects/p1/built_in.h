#ifndef BUILTIN_H
#define BUILTIN_H

/**
* Change the current working directory.
* @param args The arguments array where args[1] is the target directory.
*/
void built_in_cd(char **args);

/**
* Print the current working directory.
*/
void built_in_pwd(void);

#endif
