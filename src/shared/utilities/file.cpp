#include "file.h"

int file_size(const char *filename)
{
      struct stat file_info;
      if (!stat (filename, &file_info))
          return file_info.st_size;
      return -1;
}

bool file_exists (const char * filename)
{
    struct stat file_info;
    if (stat (filename, &file_info ) == 0)
        return true;
    return false;
}

int is_dir(const char *path)
{
    struct stat stats;
    return stat(path, &stats) == 0 && S_ISDIR(stats.st_mode);
}

bool is_dir_empty(const char *dir)
{
    struct dirent *pdir;
    DIR *d;
    char *path;

    if (!dir)
        return true;

    path=(char *)malloc(sizeof(char)*1024);
    d=opendir(dir);

    while( (pdir=readdir(d)) )
    {
        if (strcmp(pdir->d_name,".") == 0)
            continue;
        if (strcmp(pdir->d_name,"..") == 0)
            continue;

        return false;
    }

    free(path);

    return true;
}

void add_slash(char *s)
{
    if (s && (strlen(s) > 0) && (s[strlen(s)-1] != '/'))
        strcat(s,"/");
}

void deltree(const char *dir)
{
    struct dirent *pdir;
    struct stat st;
    DIR *d;
    char *path;

    if (!dir)
        return;

    path =(char *)my_malloc(sizeof(char)*256);
    d = opendir(dir);

    while( (pdir=readdir(d)) )
    {
        if (strcmp(pdir->d_name,".") == 0)
            continue;
        if (strcmp(pdir->d_name,"..") == 0)
            continue;

        strcpy(path, dir);
        if (path[strlen(path)-1] != '/')
            strcat(path, "/");
        strcat(path, pdir->d_name);
        if (!(lstat(path, &st)>=0))
            continue;
        if (st.st_mode&S_IFDIR)
            deltree(path);
        else if (st.st_mode&S_IFREG)
            remove(path);
    }

    free(path);
    rmdir(dir);
    return;
}

char *list_files(const char *dir)
{
    struct dirent *pdir;
    struct stat st;
    DIR *d=opendir(dir);
    int size=2048;
    char *path=(char *)malloc(sizeof(char)*1024);
    char *buffer=(char *)malloc(sizeof(char)*size);
    if (!dir || !buffer)
        return 0;

    strcpy(buffer,"");

    while((pdir=readdir(d))!=NULL)
    {
        if (strcmp(pdir->d_name,".") == 0)
            continue;
        if (strcmp(pdir->d_name,"..") == 0)
            continue;

        strcpy(path, dir);
        if (path[strlen(path)-1] != '/')
            strcat(path, "/");
        strcat(path, pdir->d_name);
        if (!(lstat(path, &st)>=0))
            continue;

        if ( (strlen(pdir->d_name) + strlen(buffer) + 3) < size)
        {
            strcat(buffer,pdir->d_name);

            if (st.st_mode&S_IFDIR)
                strcat(buffer,"/");

            strcat(buffer," ");
        }
        else return buffer;
    }
    free(path);
    return buffer;
}

/*
void makeSubDir(char *dir)
{
    char *slash;
    char *pdir=dir;
    char segment[256];
    int pos,loops=0;

    slash=strchr(dir,'/');
    while (slash!=NULL)
    {
        pos=slash-dir+1;
        slash=strchr(slash+1,'/');
        if (pos>1)
            strncpy(segment,dir,pos-1);
        segment[pos-1]=0;
        mkdir(segment, 0755);

        pdir+=pos;
        loops++;
    }
}
*/

char *sha_digest(const char *filename)
{
    SHA_CTX context;
    unsigned char digest[SHA_DIGEST_LENGTH];
    char *digest_ret;
    int fd, nread, i;
    char buffer[1024], hex[3];

    if ((fd=open(filename, O_RDONLY)) < 0)
    {
        perrorf("open()",0);
        return 0;
    }

    SHA1_Init(&context);

    while ((nread=read(fd,buffer,1024)) > 0)
        SHA1_Update(&context, (unsigned char*)buffer, nread);

    close(fd);

    if (!SHA1_Final(digest, &context))
        fprintf(stderr,"* sha: could not compute message digest for %s\n", \
                filename);

    digest_ret = (char*) my_malloc(sizeof(char) * 2 * SHA_DIGEST_LENGTH + 1);
    memset(digest_ret,'\0', sizeof(char));

    for(i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        sprintf(hex, "%02x", digest[i]);
        strcat(digest_ret, hex);
    }

    return (char*) digest_ret;
}

void print_file_info(const char *filename,int size,const char *sha)
{
    if (!filename || !sha)
        return;

    printf("* filename: %s\n",filename);

    printf("* filesize: ");
    num2human(size);
    printf("B\n");

    printf("* sha1:     %s\n",sha);
}




