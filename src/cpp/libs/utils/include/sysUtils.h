#ifndef __SYS_UTILS_H__
#define __SYS_UTILS_H__
#include <string>
#include <vector>
namespace utils{

// close a file descriptor handling EINTR
int eclose(int fd,bool throwExcept=true);

// dup a file descriptor throwing execption if fail
int edup(int fd);

// set file descriptor to non-blocking
void setFdNonblock(int fd);

// fork/exec an executable and setup an in-memory pipe between the two, returns child pid
// (child will read/write from/to stdin/stdout)
// (file - executable file, args - argv[0] ...)
int spawnPipeChild(std::string const&file,std::vector<std::string>args,int&fdRead,int&fdWrite,bool dieWhenParentDies);
}
#endif
