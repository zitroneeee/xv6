// Lab Xv6 and Unix utilities
// find.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
void
find(char *dir, char *file)
{   
    // 声明 文件名缓冲区 和 指针
    char buf[512], *p;
    // 声明文件描述符 fd
    int fd;
    // 声明与文件相关的结构体
    struct dirent de;
    struct stat st;
    if ((fd = open(dir, 0)) < 0)
    {
        // 报错，提示无法打开此路径
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }
    if (fstat(fd, &st) < 0)
    {
        // 出错则报错
        fprintf(2, "find: cannot stat %s\n", dir);
        // 关闭文件描述符 fd
        close(fd);
        return;
    }
    if (st.type != T_DIR)
    {
        // 报类型不是目录错误
        fprintf(2, "find: %s is not a directory\n", dir);
        // 关闭文件描述符 fd
        close(fd);
        return;
    }
    if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2, "find: directory too long\n");
        close(fd);
        return;
    }
    strcpy(buf, dir);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if(de.inum == 0)
            continue;
        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        if (st.type == T_DIR)
        {
            find(buf, file);
        }
        else if (st.type == T_FILE && !strcmp(de.name, file))
        {
            printf("%s\n", buf);
        } 
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "usage: find dirName fileName\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
