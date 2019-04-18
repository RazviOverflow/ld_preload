/* RazviOverflow
 This file must be compiled with the following command:
	gcc -shared -Wall -Wextra -fPIC ALLinONE.c -o ALLinONE.so -std=c99 -ldl
 You can then execute the vulnerable code with:
 	LD_PRELOAD=$PWD/libTTThwart.so ./vulnerable tryout

Iint lstat64 (const char *__restrict __file
*/
#define _GNU_SOURCE

#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


// https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/fcntl.h
//#define O_RDONLY  00000000

static int (*old_xstat)(int ver, const char *path, struct stat *buf) = NULL;
static int (*old_lxstat)(int ver, const char *path, struct stat *buf) = NULL;
static int (*old_xstat64)(int ver, const char *path, struct stat64 *buf) = NULL;
static int (*old_open)(const char *path, int flags) = NULL; 

//TODO malloc family functions error checking
//TODO find
//TODO implement logger and replace printf family with corresponding log level


//#################### file_objects_info.c #######################
typedef struct{
  char *path;
  ino_t inode;
} file_object_info;

typedef struct{
    file_object_info * list;
    size_t used;
    size_t size;
} file_objects_info;

//########## GLOBAL VARIABLES ########################
file_objects_info g_array;
//####################################################

/* 

// This function is highly probable to be deleted since array initialiation is
// only needed when inserting elements for the first time, so there is no need
// for such a dedicated function. Checking and initialization can be carried
// out in Insert function. 

void check_and_initialize_array(){
    printf("Check and initialize has been called\n");
    if(g_array.size == 0){
        Initialize(&g_array, 2);
    }
}
*/

void check_dlsym_error(){
  char * error = dlerror();
  if(error != NULL){
    printf("There were errors while retrieving the original function from the dynamic linker/loader.\nDlsym error: %s\n", error);
    exit(EXIT_FAILURE);
}
}


/*
    Initializes the given array with the given size, allocating
    the corresponding memory.
*/
void initialize_array(file_objects_info *array, size_t size){
    printf("Initialize has been called for array: %X and size: %d\n", &(*array), size);
    array->list = (file_object_info *) calloc(size, sizeof(file_object_info)); 
    if(!array->list){
        printf("Error allocating memory for array in Initialize process.\n");
        exit(EXIT_FAILURE);
    }
    array->used = 0;
    array->size = size;
    //Elements of array are contiguous
    //memset(&array->list[array->used], 0, sizeof(file_object_info) * initialSize);
}

/*
file_object_info Createfile_object_info(char * path, ino_t inode){
    file_object_info file_object_info;
    file_object_info.path = path;
    file_object_info.inode = inode;
    return file_object_info;
}
*/

/*
    Inserts into the given array the given path and inode.  
    Before inserting elements into the given array, the array 
    must be initialized.
    If there is not enough room in the array to insert a new
    file_object_info element, the size of the array gets doubled.
    After the element is inserted, "used" member of the given
    array is postincremented.
*/
void insert_in_array(file_objects_info *array, const char *path, ino_t inode){
    printf("Insert has been called for array: %X with path: %s and inode: %d\n",&(*array), path, inode);
    
    // If array has not been yet initialized, initialize it. 
    if(array->size == 0){
        initialize_array(&g_array, 2);
    }

    // If number of elements (used) in the array equals its size, it means
    // the array requires more room. It's size gets doubled
    if(array->used == array->size){
        printf("Size of array %X is about to get doubled.\n", &(*array));
        array->size *= 2;
        file_object_info *aux = (file_object_info *)realloc(array->list,
            array->size * sizeof(file_object_info));

        // It is never a good idea to do something like:
        // array->list = realloc... because if realloc fails you lose the
        // reference to your original data and realloc does not free() so
        // there'll be an implicit memory leak.
        if(!aux){
            printf("Error trying to realloc size for array in Insert process.\n");
            exit(EXIT_FAILURE);
        } else {
            array->list = aux;
        }

        //Initializing new elements of realocated array
        memset(&array->list[array->used], 0, sizeof(file_object_info) * (array->size - array->used));


    }

    //file_object_info file_object_info = Createfile_object_info(path, inode);

    //array->list[array->used].path = (const char *)malloc(strlen(path)+1);
    //strcpy(array->list[array->used].path, path);

    array->list[array->used].path = strdup(path);
    array->list[array->used].inode = inode;



    array->used++;



}

/*
    Frees the memory used by the given array. This function
    is ment to be called at the end of the program.
*/
void free_array(file_objects_info *array){

    for(int i = 0; i < array->used; i++){
        free(array->list[i].path);
        array->list[i].path = NULL;
    }

    free(array->list);
    array->list = NULL;

    array->used = 0;
    array->size = 0;

}

/*
    Find the index of the given path in the given array. If array's
    size is not bigger than 0, it means the array has not yet been 
    initialized, so there is no way the element could be found. 
*/
int find_index_in_array(file_objects_info *array, const char *path){
    int returnValue = -1;
    if(array->size > 0){
        for(int i = 0; i < array->used; i++){
            if(!strcmp(array->list[i].path, path)){
                returnValue = i;
                break;
            }
        }
        return returnValue;
    } else {
        return returnValue;
    }
}

/*
   Retrieve the file_object_info element at the given index in the
   given array. 
*/
file_object_info get_from_array_at_index(file_objects_info *array, int index){
    return array->list[index];
}

/*
    Checks properties of the given parameters, this is, the given path and 
    inode. Checking properties in this context means checking if a 
    file_object_info with the same path already exists in the array. If it
    doesn't, insert it and return true, othwerwise (if it does) compare 
    the given inode and the inode of the file_object_info. If they're equal
    return true, otherwise return false (TOCTTOU detected). 
*/
void check_parameters_properties(const char *path, ino_t inode, const char *caller_function_name){

    int index = find_index_in_array(&g_array, path);
    if(index < 0){
        insert_in_array(&g_array, path, inode);
    } else {
        file_object_info aux = get_from_array_at_index(&g_array,index);
        if(aux.inode != inode){
            printf("WARNIN! TOCTTOU DETECTED!. Inode of %s has changed since it was previously invoked. Threat detected when invoking %s function.", caller_function_name, path);
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

    }
    printf("######\n");
}

//#######################################################################

/*
    The correct way to test for an error is to call dlerror() 
    to clear any old error conditions, then call dlsym(), and 
    then call dlerror() again, saving its return value into a
    variable, and check whether this saved value is not NULL.
    https://linux.die.net/man/3/dlsym
*/
void* dlsym_wrapper(char *original_function){

    dlerror();

    void *function_handler;
    
    function_handler = dlsym(RTLD_NEXT, original_function);

    check_dlsym_error();

    return function_handler;
}
/*
    The open wrapper ensures old_open is initialized and is used
    by other inner functions in order to avoid open() recursivity
    and overhead.
*/
int open_wrapper(const char *path, int flags, ...){
    printf("Array size is: %d and used is:%d\n", (int) g_array.size, g_array.used);
    if ( old_open == NULL ) {
        old_open = dlsym_wrapper("open");
    }
    return old_open(path, flags);

}

/*
    Retrieves the corresponding inode of a give path while performing errors
    checking.
*/
ino_t get_inode(const char *path){
    int fd, ret;
    ino_t inode;
    printf("User invoked get_inode for %s\n", path);
    // Parenthesis are needed because of operator precedence.
    // https://en.cppreference.com/w/c/language/operator_precedence
    if((fd = open_wrapper(path, O_RDONLY)) < 0){
        printf("Errors occured while trying to access %s.\nAborting.", path);
        perror("Error1 is: ");
        fflush(stdout);
        //exit(EXIT_FAILURE);
    } else {
        printf("Created fileDescriptor is: %d\n", fd);
        struct stat file_stat;
        if((ret = fstat(fd, &file_stat)) < 0 ){
            printf("Errors occured while trying to stat %d file descriptor.\nAborting.", fd);
            perror("Error2 is: ");
            close(fd);
            //exit(EXIT_FAILURE);
        } else {
            inode = file_stat.st_ino;
            //After opening a FD, it must be closed
            close(fd);
        }
    }
    printf("User invoked get_inode for %s and it's %d\n", path, inode);
    return inode;
}

int __xstat(int ver, const char *path, struct stat *buf)
{

 printf("User invoked %s on: %s\n", __func__, path);

 ino_t inode = get_inode(path);
 check_parameters_properties(path, inode, __func__);

 if ( old_xstat == NULL ) {
    old_xstat = dlsym_wrapper(__func__);
}

  //printf("xstat64 %s\n",path);
return old_xstat(ver, path, buf);
} 

int __lxstat(int ver, const char *path, struct stat *buf)
{
    printf("User invoked %s on: %s\n", __func__, path);

    ino_t inode = get_inode(path);

    check_parameters_properties(path, inode, __func__);
    
    if ( old_lxstat == NULL ) {
        old_lxstat = dlsym_wrapper(__func__);
    }

    return old_lxstat(ver,path, buf);
}


int __xstat64(int ver, const char *path, struct stat64 *buf)
{

    printf("User invoked %s on: %s\n", __func__, path);

    ino_t inode = get_inode(path);
    check_parameters_properties(path, inode, __func__);

    if ( old_xstat64 == NULL ) {
        old_xstat64 = dlsym_wrapper(__func__);
    }

  //printf("xstat64 %s\n",path);
    return old_xstat64(ver, path, buf);
}

int open(const char *path, int flags, ...)
{

  printf("User invoked %s on: %s\n", __func__, path);
  
  ino_t inode = get_inode(path);

  check_parameters_properties(path, inode, __func__);
  return open_wrapper(path, flags); 
}

//#########################
/*
int
creat64(const char *pathname, mode_t mode)
{
    intercept("creat64", 2);
    set_errno();
    return -1;
}

int
creat(const char *pathname, mode_t mode)
{
    intercept("creat", 2);
    set_errno();
    return -1;
}

int
close(int fd)
{
    intercept("close", 2);
    set_errno();
    return -1;
}

int
open64(const char *pathname, int flags, ...)
{
    intercept("open64", 2);
    set_errno();
    return -1;
}



ssize_t
read(int fd, void *buf, size_t count)
{
    intercept("read", 2);
    set_errno();
    return -1;
}

ssize_t
readv(int fd, const struct iovec *vector, int count)
{
    intercept("readv", 2);
    set_errno();
    return -1;
}

ssize_t
pread(int fd, void *buf, size_t count, unsigned long offset)
{
    intercept("pread", 2);
    set_errno();
    return -1;
}

ssize_t
pread64(int fd, void *buf, size_t count, uint64_t offset)
{
    intercept("pread64", 2);
    set_errno();
    return -1;
}

ssize_t
write(int fd, const void *buf, size_t count)
{
    intercept("write", 2);
    set_errno();
    return -1;
}

ssize_t
writev(int fd, const struct iovec *vector, int count)
{
    intercept("writev", 2);
    set_errno();
    return -1;
}

ssize_t
pwrite(int fd, const void *buf, size_t count, unsigned long offset)
{
    intercept("pwrite", 2);
    set_errno();
    return -1;
}

ssize_t
pwrite64(int fd, const void *buf, size_t count, uint64_t offset)
{
    intercept("pwrite64", 2);
    set_errno();
    return -1;
}

off_t
lseek(int fildes, unsigned long offset, int whence)
{
    intercept("lseek", 2);
    set_errno();
    return -1;
}

off_t
lseek64(int fildes, uint64_t offset, int whence)
{
    intercept("lseek64", 2);
    set_errno();
    return -1;
}

int
dup(int fd)
{
    intercept("dup", 2);
    set_errno();
    return -1;
}

int
dup2(int oldfd, int newfd)
{
    intercept("dup2", 2);
    set_errno();
    return -1;
}

int
mkdir(const char *pathname, mode_t mode)
{
    intercept("mkdir", 2);
    set_errno();
    return -1;
}

int
rmdir(const char *pathname)
{
    intercept("rmdir", 2);
    set_errno();
    return -1;
}

int
chmod(const char *pathname, mode_t mode)
{
    intercept("chmod", 2);
    set_errno();
    return -1;
}

int
chown(const char *pathname, uid_t owner, gid_t group)
{
    intercept("chown", 2);
    set_errno();
    return -1;
}

int
fchmod(int fd, mode_t mode)
{
    intercept("fchmod", 2);
    set_errno();
    return -1;
}

int
fchown(int fd, uid_t uid, gid_t gid)
{
    intercept("fchown", 2);
    set_errno();
    return -1;
}

int
fsync(int fd)
{
    intercept("fsync", 2);
    set_errno();
    return -1;
}

int
ftruncate(int fd, off_t length)
{
    intercept("ftruncate", 1);
    set_errno();
    return -1;
}

int
ftruncate64(int fd, off_t length)
{
    intercept("ftruncate64", 1);
    set_errno();
    return -1;
}

int
link(const char *oldpath, const char *newname)
{
    intercept("link", 2);
    set_errno();
    return -1;
}

int
rename(const char *oldpath, const char *newpath)
{
    intercept("rename", 2);
    set_errno();
    return -1;
}

int
utimes(const char *path, const struct timeval times[2])
{
    intercept("utimes", 2);
    set_errno();
    return -1;
}

int
futimes(int fd, const struct timeval times[2])
{
    intercept("futimes", 2);
    set_errno();
    return -1;
}

int
utime(const char *path, const struct utimbuf *buf)
{
    intercept("utime", 2);
    set_errno();
    return -1;
}

int
mknod(const char *path, mode_t mode, dev_t dev)
{
    intercept("mknod", 2);
    set_errno();
    return -1;
}

int
__xmknod(int ver, const char *path, mode_t mode, dev_t *dev)
{
    intercept("__xmknod", 2);
    set_errno();
    return -1;
}

int
mkfifo(const char *path, mode_t mode)
{
    intercept("mkfifo", 2);
    set_errno();
    return -1;
}

int
unlink(const char *path)
{
    intercept("unlink", 2);
    set_errno();
    return -1;
}

int
symlink(const char *oldpath, const char *newpath)
{
    intercept("symlink", 2);
    set_errno();
    return -1;
}

int
readlink(const char *path, char *buf, size_t bufsize)
{
    intercept("readlink", 1);
    set_errno();
    return -1;
}

char *
realpath(const char *path, char *resolved)
{
    intercept("realpath", 1);
    set_errno();
    return NULL;
}

DIR *
opendir(const char *path)
{
    intercept("opendir", 2);
    set_errno();
    return NULL;
}

struct dirent *
readdir(DIR *dir)
{
    intercept("readdir\t", 2);
    set_errno();
    return NULL;
}

struct dirent *
readdir64(DIR *dir)
{
    intercept("readdir64", 2);
    set_errno();
    return NULL;
}

int
readdir_r(DIR *dir, struct dirent *entry, struct dirent **result)
{
    intercept("readdir_r", 1);
    set_errno();
    return -1;
}

int
readdir64_r(DIR *dir, struct dirent *entry, struct dirent **result)
{
    intercept("readdir64_r", 1);
    set_errno();
    return -1;
}

int
closedir(DIR *dh)
{
    intercept("closedir", 1);
    set_errno();
    return -1;
}

int
stat(const char *path, struct stat *buf)
{
    intercept("stat", 2);
    set_errno();
    return -1;
}

int
stat64(const char *path, struct stat *buf)
{
    intercept("stat64", 2);
    set_errno();
    return -1;
}

int
__fxstat(int ver, int fd, struct stat *buf)
{
    intercept("__fxstat\t", 2);
    set_errno();
    return -1;
}

int
__fxstat64(int ver, int fd, struct stat *buf)
{
    intercept("__fxstat64", 2);
    set_errno();
    return -1;
}


int
fstat(int fd, struct stat *buf)
{
    intercept("fstat", 2);
    set_errno();
    return -1;
}

int
fstat64(int fd, struct stat *buf)
{
    intercept("fstat64", 2);
    set_errno();
    return -1;
}


int
__lxstat64(int ver, const char *path, struct stat *buf)
{
    intercept("__lxstat64", 2);
    set_errno();
    return -1;
}

int
lstat(const char *path, struct stat *buf)
{
    intercept("lstat", 2);
    set_errno();
    return -1;
}

int
lstat64(const char *path, struct stat *buf)
{
    printf("USER INVOKED LSTAT64 ON: %s !!!!! ", path);
    intercept("lstat64", 2);
    set_errno();
    return -1;
}

int
statfs(const char *path, struct statfs *buf)
{
    intercept("statfs", 2);
    set_errno();
    return -1;
}

int
statfs64(const char *path, struct statfs *buf)
{
    intercept("statfs64", 2);
    set_errno();
    return -1;
}

int
statvfs(const char *path, struct statvfs *buf)
{
    intercept("statvfs\t", 2);
    set_errno();
    return -1;
}

int
statvfs64(const char *path, struct statvfs *buf)
{
    intercept("statvfs64", 2);
    set_errno();
    return -1;
}

ssize_t
getxattr(const char *path, const char *name, void *value, size_t size)
{
    intercept("getxattr", 1);
    set_errno();
    return -1;
}

ssize_t
lgetxattr(const char *path, const char *name, void *value, size_t size)
{
    intercept("lgetxattr", 1);
    set_errno();
    return -1;
}

int
remove(const char *path)
{
    intercept("remove", 2);
    set_errno();
    return -1;
}

int
lchown(const char *path, uid_t owner, gid_t group)
{
    intercept("lchown", 2);
    set_errno();
    return -1;
}

void
rewinddir(DIR *dirp)
{
    intercept("rewinddir", 1);
    set_errno();
    return;
}

void
seekdir(DIR *dirp, off_t offset)
{
    intercept("seekdir", 2);
    set_errno();
    return;
}

off_t
telldir(DIR *dirp)
{
    intercept("telldir", 2);
    set_errno();
    return -1;
}

ssize_t
sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    intercept("sendfile\t", 1);
    set_errno();
    return -1;
}

ssize_t
sendfile64(int out_fd, int in_fd, off_t *offset, size_t count)
{
    intercept("sendfile64", 1);
    set_errno();
    return -1;
}

int
fcntl(int fd, int cmd, ...)
{
    intercept("fcntl", 2);
    set_errno();
    return -1;
}*/
