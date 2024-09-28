#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 声明 fmtname 函数
char *fmtname(char *path);

void find(char *path, char *name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue; // 跳过空项
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                // 忽略 . 和 ..
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;

                // 递归查找子目录
                find(buf, name);

                // 检查当前文件名，无论是文件还是目录，都可以输出且只在这个case输出
                if (strcmp(fmtname(buf), name) == 0) {
                    printf("%s\n", buf); // 打印匹配的文件或目录路径
                }
            }
            break;
    }
    close(fd);
}

char *fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;

    // 从路径中提取文件名
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++; // 跳过 '/'

    // 处理文件名长度
    if (strlen(p) >= DIRSIZ) return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = '\0';
    return buf; // 返回格式化后的文件名
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <name>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
