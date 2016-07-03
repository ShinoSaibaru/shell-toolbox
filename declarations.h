#ifndef DECLARATIONS_H
#define DECLARATIONS_H

extern char **filenames;
extern char **descriptions;
extern int files_count = 0;

extern int handle_arguments(int argc, char *argv[]);
extern void usage();
extern int load_scripts();
extern int execute(int argc, char *argv[]);

#endif // DECLARATIONS_H