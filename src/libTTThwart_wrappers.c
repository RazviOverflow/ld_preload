#define _GNU_SOURCE

#include <dlfcn.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "libTTThwart_wrappers.h"
#include "libTTThwart_internals.h"
#include "libTTThwart_hooked_functions.h"


/// ########## Wrappers ##########

/*
    The correct way to test for an error is to call dlerror() 
    to clear any old error conditions, then call dlsym(), and 
    then call dlerror() again, saving its return value into a
    variable, and check whether this saved value is not NULL.
    https://linux.die.net/man/3/dlsym
*/
void* dlsym_wrapper(const char *original_function){

	dlerror();

	void *function_handler;

	function_handler = dlsym(RTLD_NEXT, original_function);

	check_dlsym_error();

	return function_handler;
}

/*
    The open wrapper guarantees, insures original_open is initialized.
    It's used by other inner functions in order to avoid open() recursivity
    and overhead. In adittion, it deals with ellipsis (variable 
    arguments) since open is a variadic function.
*/
int open_wrapper(const char *path, int flags, va_list variable_arguments){

	if ( original_open == NULL ) {
		original_open = dlsym_wrapper("open");
	}

	if(variable_arguments){
		va_list aux_list;
		va_copy(aux_list, variable_arguments);

		mode_t mode = va_arg(aux_list, mode_t);

		va_end(aux_list);

		return original_open(path, flags, mode);

	} else {
		return original_open(path, flags);
	}
	
}

int open64_wrapper(const char *path, int flags, va_list variable_arguments){

	if ( original_open64 == NULL ) {
		original_open64 = dlsym_wrapper("open64");
	}

	if(variable_arguments){
		va_list aux_list;
		va_copy(aux_list, variable_arguments);

		mode_t mode = va_arg(aux_list, mode_t);

		va_end(aux_list);

		return original_open64(path, flags, mode);

	} else {
		return original_open64(path, flags);
	}
	
}

/*
	Openat wrapper is exactly the same as open wrapper but for
	 openat.
*/
int openat_wrapper(int dirfd, const char *path, int flags, va_list variable_arguments){

	if(original_openat == NULL){
		original_openat = dlsym_wrapper("openat");
	}

	if(variable_arguments){
		va_list aux_list;
		va_copy(aux_list, variable_arguments);

		mode_t mode = va_arg(variable_arguments, mode_t);

		va_end(aux_list);

		return original_openat(dirfd, path, flags, mode);

	} else {
		return original_openat(dirfd, path, flags);
	}

}

/*
	Same as open_wrapper.
*/
int chdir_wrapper(const char *path){
	
	if(original_chdir == NULL){
		original_chdir = dlsym_wrapper("chdir");
	}

	return original_chdir(path);
}


/*
	Wrapper for all execlX functions family. This wrapper treats the variable
	arguments and calls the corresponding execlX function according to:
	[function argument value] : [execlX function]
	0 : execl https://code.woboq.org/userspace/glibc/posix/execl.c.html
	1 : execlp https://code.woboq.org/userspace/glibc/posix/execlp.c.html
	2 : execle https://code.woboq.org/userspace/glibc/posix/execle.c.html

	Additional info: https://code.woboq.org/userspace/glibc/posix/execl.c.html
*/
int execlX_wrapper(int function, const char *pathname, const char *arg, va_list variable_arguments){
	int execlX_result = -1;

	if(function == 0 || function == 1 ){
		va_list aux_list;
		va_copy(aux_list, variable_arguments);
		int number_of_arguments = get_number_of_variable_arguments_char_pointer_type(aux_list);
		if(number_of_arguments == -1){
			fprintf(stderr,"Error when retrieveng variable arguments. Aborting\n. Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// Reset aux_list and start from the very beginning when using va_arg
		va_end(aux_list);
		va_copy(aux_list, variable_arguments);

		char *argv[number_of_arguments + 1];
		argv[0] = (char *) arg;
		ptrdiff_t i;
		for(i = 1; i<= number_of_arguments; i++){
			argv[i] = va_arg(aux_list, char *);
		}

		va_end(aux_list);
		switch(function){
			case 0:
				execlX_result = execve_wrapper(pathname, argv, __environ);
				break;
			case 1:
				execlX_result = execvpe_wrapper(pathname, argv, __environ);
				break;
		}

	} else if(function == 2){

		va_list aux_list;
		va_copy(aux_list, variable_arguments);
		int number_of_arguments = get_number_of_variable_arguments_char_pointer_type(aux_list);
		if(number_of_arguments == -1){
			fprintf(stderr,"Error when retrieveng variable arguments. Aborting\n. Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);		
		}

		// Reset aux_list and start from the very beginning when using va_arg
		va_end(aux_list);
		va_copy(aux_list, variable_arguments);

		char *argv[number_of_arguments + 1];
		char **envp;
		argv[0] = (char *) arg;
		ptrdiff_t i;
		for(i = 1; i<= number_of_arguments; i++){
			argv[i] = va_arg(aux_list, char *);
		}
		envp = va_arg(variable_arguments, char **);
		va_end(aux_list);

		execlX_result = execve_wrapper(pathname, argv, envp);
	} 

	return execlX_result;
}

int execv_wrapper(const char *pathname, char *const argv[]){

	return execve_wrapper(pathname, argv, __environ);
}

int execvp_wrapper(const char *file, char *const argv[]){

	return execvpe_wrapper(file, argv, __environ);
}

int execve_wrapper(const char *pathname, char *const argv[], char *const envp[]){
	if ( original_execve == NULL ) {
		original_execve = dlsym_wrapper("execve");
	}

	return original_execve(pathname, argv, envp);
}

int execvpe_wrapper(const char *file, char *const argv[], char *const envp[]){
	if ( original_execvpe == NULL ) {
		original_execvpe = dlsym_wrapper("execvpe");
	}

	return original_execvpe(file, argv, envp);
}

FILE* fopen_wrapper(const char *path, const char *mode){

	if(original_fopen == NULL){
		original_fopen = dlsym_wrapper("fopen");
	}

	return original_fopen(path, mode);
}

int mkdir_wrapper(const char* pathname, mode_t mode){

	printf("Se ha invocado mkdir wrapper con :%s\n", pathname);

	if(original_mkdir == NULL){
		original_mkdir = dlsym_wrapper("mkdir");
	}

	return original_mkdir(pathname, mode);

}
/// ########## Wrappers ##########