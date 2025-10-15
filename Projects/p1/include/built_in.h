#ifndef INCLUDE_BUILT_IN_H_
#define INCLUDE_BUILT_IN_H_

/**
* Change the current working directory.
* @param args The arguments array where args[1] is the target directory.
*/
void built_in_cd(char **args);

/**
* Print the current working directory.
*/
void built_in_pwd(void);

#endif  // INCLUDE_BUILT_IN_H_
