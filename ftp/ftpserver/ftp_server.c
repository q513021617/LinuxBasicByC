#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <stdbool.h>
#define TRUE 1
#define dataLen 1024

typedef struct {
    char **str;     //the PChar of string array
    size_t num;     //the number of string
}IString;
typedef struct data{
    char name[20]; //存储用户名
    char pass[20];  //存储密码
}user_data;
user_data user[20] = {0}; //能够保存20个用户的结构体数组


int LISTEN_PORT=3499;
char currentDirPath[200];
char currentDirName[30];
char* userlist[30];
char help[]="get    get a file from server\n\
put    upload a file to server\n\
pwd    display    the current directory of server\n\
ls    display the files in the current directory of server\n\
cd    change the directory of server\n\
mkdir    create the directory in the current directory of server\n\
rmdir    remove the directory in the current directory of server\n\
lpwd    display    the current directory of local\n\
dir    display the files in the current directory of local\n\
lmkdir    create the directory in the current directory of local\n\
lrmdir    remove the directory in the current directory of local\n\
login    login system\n\
register    register user\n\
?    display the whole command which equals 'help'\n\
quit    return\n";

char user_pass_empty[]="username or password is null\n";
char user_pass_error[]="username or password is error,please check your username and password\n";


int Split(char *src, char *delim, IString* istr);
int CreateDir(const char *sPathName);
char *getDirName(char *dirPathName);
void cmd_pwd(int sock);
void cmd_dir(int sock);
void cmd_login(int sock,char *userName);

void cmd_cd(int sock,char *dirName);
void cmd_cdback(int sock);
void cmd_help(int sock);
void cmd_mkdir(int sock,char *dirName);
void cmd_rmdir(int sock,char *dirName);
void cmd_get(int sock,char*fileName);
void cmd_put(int sock,char *fileName);
int addUser(char* username);
void executeCMD(const char *cmd, char *result);
void getUidByusername(char* res,int *uid,int* gid);
void cmd_count_current(int sock);
void cmd_count_all(int sock);
void cmd_list(int sock);
void cmd_killuser(int sock);

int userlistsize=0;
int onlineUser=0;
char user_name[20],user_password[20];

bool cmd_register(int sock,char* name,char* pass);
bool login(int sock,char* name,char* pass);
int main(int argc,char *argv[])
{
    int is_null=argc;
    printf("%d\n",argc);

   int sock,sockmsg, length, lengthmsg;
   char client_cmd[10] , cmd_arg[20];
   struct sockaddr_in server;
   struct sockaddr_in servermsg;
   int datasock, msgsock;     
   pid_t child;
 
  // int datasock;   //data socket
   int rval;
   
   char* ip_addr;
   if(is_null>2){
        LISTEN_PORT=atoi(argv[2]);
        ip_addr=argv[1];
        
        printf("ip:%s port:%d \n",ip_addr,LISTEN_PORT);
   }
   
   if(is_null<=2){
        LISTEN_PORT=atoi(argv[1]);
        ip_addr="0.0.0.0";
        printf("ip:%s port:%d \n",ip_addr,LISTEN_PORT);
   }

   

   sock=socket(AF_INET,SOCK_STREAM,0);//创建流式套接字
   sockmsg=socket(AF_INET,SOCK_STREAM,0);
   if (sock<0||sockmsg<0)
   {
     printf("opening stream socket");
     exit(1);
   }
 
   server.sin_family=AF_INET;//AF_INET针对internet,另一种形式AF_UNIX只能够用于单一的Unix系统进程通信（主机通信协议）
   server.sin_addr.s_addr=INADDR_ANY;
   if(strlen(ip_addr)>0){
       server.sin_addr.s_addr=inet_addr(ip_addr);
   }
   //server.sin_addr.s_addr=INADDR_ANY;//表示可以和任何主机通信
   server.sin_port=htons(LISTEN_PORT);//htons将短整形数据转换位网络字节顺序

 
   servermsg.sin_family=AF_INET;
   servermsg.sin_addr.s_addr=INADDR_ANY;
   if(strlen(ip_addr)>0){
       servermsg.sin_addr.s_addr=inet_addr(ip_addr);
   }
//    servermsg.sin_addr.s_addr=INADDR_ANY;
   servermsg.sin_port=htons(LISTEN_PORT+1);

   if (bind(sock,(struct sockaddr *)&server,sizeof server)<0||bind(sockmsg,(struct sockaddr *)&servermsg,sizeof servermsg)<0)//将本地端口与socket返回的文件描述符绑定在一起
   {
     printf("binding stream socket");
     exit(1);
   }
   
   length=sizeof server;
   lengthmsg=sizeof servermsg;
   if (getsockname(sock,(struct sockaddr *)&server,&length)<0||getsockname(sockmsg,(struct sockaddr *)&servermsg,&lengthmsg)<0)//用于得套接口的名字
   {
     printf("getting socket name");
     exit(1);
   }
 
   printf("Socket port # %d  %d\n",ntohs(server.sin_port),ntohs(servermsg.sin_port));

   memset(currentDirPath,0,sizeof(currentDirPath));//将currentDirpath置零
   getcwd(currentDirPath,sizeof(currentDirPath));//得到当前路径，保存在currentDirPath中
 
   listen(sock,2); //监听，允许最大监听数位2，将绑定的套接字变成监听套接字
   listen(sockmsg,2);
   do  
   {
      datasock = accept(sock,(struct sockaddr*)0,(int*)0);//接受请求
      msgsock = accept(sockmsg,(struct sockaddr*)0,(int*)0);
      if (datasock == -1||msgsock==-1)
        printf("accept");
      else
      {
        if((child=fork())==-1)//创建子进程
        {
            printf("Fork Error!\n");
        }
        //The child process
        if(child==0)
        {
            printf("connection accepted! new client comming\n");
            onlineUser++;
    loop:
             printf("当前服务器在线人数 : %d 人\n",onlineUser);

            memset(client_cmd,0,sizeof(client_cmd));
            rval=0;
            rval=read(msgsock,client_cmd,sizeof(client_cmd));//从msgsock中读取数据到client_cmd
             printf("client cmd: %s\n",client_cmd);
            if(rval<0)
            {
                printf("reading command failed\n");
            }
            else if(rval==0)
            {
                printf("connection closed.\n");
                close(datasock);//关闭套接字
                close(msgsock);
            }
            else
            {
                if(strcmp(client_cmd,"login")==0){
                    printf("cmmand login\n");
                    memset(user_name,0,sizeof(user_name));
                    read(msgsock,user_name,sizeof(user_name));
                    memset(user_password,0,sizeof(user_password));
                    read(msgsock,user_password,sizeof(user_password));
                    printf("user_name %s\n",user_name);
                    login(datasock,user_name,user_password);
                    printf("done\n\n");
                    goto loop;
                }
                if(strcmp(client_cmd,"pwd")==0)
                {
                    printf("cmmand pwd\n");
                    cmd_pwd(datasock);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"ls")==0)
                {
                    printf("command ls\n");
                    cmd_dir(datasock);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"count")==0)
                {
                    printf("command count ");
                    memset(cmd_arg,0,sizeof(cmd_arg));
                    read(msgsock,cmd_arg,sizeof(cmd_arg));
                     printf("%s",cmd_arg);
                     if(strcmp(cmd_arg,"current")==0)
                        cmd_count_current(datasock);
                    if(strcmp(cmd_arg,"all")==0)
                        cmd_count_all(datasock);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"cd")==0)
                {
                    printf("command cd\n");
                    //read the argument
                    memset(cmd_arg,0,sizeof(cmd_arg));
                    read(msgsock,cmd_arg,sizeof(cmd_arg));
                    cmd_cd(datasock,cmd_arg);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"register")==0)
                {
                    printf("command cd\n");
                    char username[20],password[20];
                    //read the argument
                    memset(username,0,sizeof(username));
                    read(msgsock,username,sizeof(username));
                    memset(password,0,sizeof(password));
                    read(msgsock,password,sizeof(password));
                    cmd_register(datasock,username,password);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"mkdir")==0)
                {
                    printf("command mkdir\n");
                    memset(cmd_arg,0,sizeof(cmd_arg));
                    read(msgsock,cmd_arg,sizeof(cmd_arg));
                    cmd_mkdir(datasock,cmd_arg);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"rmdir")==0)
                {
                    printf("command mkdir\n");
                    memset(cmd_arg,0,sizeof(cmd_arg));
                    read(msgsock,cmd_arg,sizeof(cmd_arg));
                    cmd_rmdir(datasock,cmd_arg);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"cd..")==0)
                {
                    printf("command cd..\n");
                    cmd_cdback(datasock);
                    printf("done\n\n");
                    goto loop;
                }else if (strcmp(client_cmd,"list")==0)
                {
                     printf("user list\n");
                    cmd_list(datasock);
                    printf("done\n\n");
                    goto loop;
                }
                else if (strcmp(client_cmd,"kill")==0)
                {
                    memset(cmd_arg,0,sizeof(cmd_arg));
                    read(msgsock,cmd_arg,sizeof(cmd_arg));
                    cmd_killuser(datasock);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"get")==0)
                {
                    printf("command get\n");
 
                    memset(cmd_arg,0,sizeof(cmd_arg));
                    read(msgsock,cmd_arg,sizeof(cmd_arg));
                    cmd_get(datasock,cmd_arg);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"put")==0)
                {
                    printf("command put\n");
 
                    memset(cmd_arg,0,sizeof(cmd_arg));
                    read(msgsock,cmd_arg,sizeof(cmd_arg));
                    cmd_put(datasock,cmd_arg);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"?")==0)
                {
                    printf("command ?\n");

                    cmd_help(datasock);
                    printf("done\n\n");
                    goto loop;
                }
                else if(strcmp(client_cmd,"quit")==0)
                {
                    printf("quit\n");
                    goto endchild;
                }
                else  
                {
                    printf("bad request!\n");
                    goto loop;
                }
            }
endchild:
            onlineUser--;
            printf("connection closed.\n");
            close(datasock);
            close(msgsock);
            exit(0);
        }
         
      }
   }while(TRUE);
 
   exit(0);
}
void cmd_mkdir(int sock,char *dirName){
    char DirName[256];  
      strcpy(DirName, dirName);
    // char *tempstr="create dir ";
    // strcat(tempstr,dirName);
    printf("create dir %s \n",DirName);
    
    if(mkdir(DirName, 0755)==-1)  
                  {   
                      printf("mkdir   error\n");   
                      return -1;   
                  }  
    cmd_dir(sock);
}
void cmd_rmdir(int sock,char *dirName){
    char DirName[256];  
      strcpy(DirName, dirName);
    // char *tempstr="create dir ";
    // strcat(tempstr,dirName);
    printf("create dir %s \n",DirName);
    
    if(remove(DirName)==-1)  
                  {   
                      printf("mkdir   error\n");   
                      return -1;   
                  }  
    cmd_dir(sock);
}
int CreateDir(const char *sPathName)  
  {  
      char DirName[256];  
      strcpy(DirName, sPathName);  
      int i,len = strlen(DirName);
      for(i=1; i<len; i++)  
      {  
          if(DirName[i]=='/')  
          {  
              DirName[i] = 0; 
              if(access(DirName, NULL)!=0)  
              {  
                  if(mkdir(DirName, 0755)==-1)  
                  {   
                      printf("mkdir   error\n");   
                      return -1;   
                  }  
              }  
              DirName[i] = '/';  

          }  
      }  

      return 0;  
  } 
//pwd command
void cmd_pwd(int sock)
{
    int len;
    memset(currentDirPath,0,sizeof(currentDirPath));
    getcwd(currentDirPath,sizeof(currentDirPath));
    char *savePointer=getDirName(currentDirPath);
    strcpy(currentDirName,savePointer);
    len=strlen(currentDirName)+1;
    write(sock,currentDirName,len);
}

void cmd_login(int sock,char *userName){
    int len;
    struct passwd *pwd;
    if(strlen(userName)<=0){

        len=strlen(currentDirName)+1;
        write(sock,user_pass_empty,len);
    }
    printf("username: %s\n",userName);
    char res[20];
    char* cmdstr="id ";
    strcat(userName,cmdstr);
    executeCMD(cmdstr,res);
    printf("res: %s",res);
    int uid=0;
    int gid=0;
    getUidByusername(res,uid,gid);
    printf("uid %d gid %d \n",uid,gid);

    pwd = getpwuid(uid);
    struct group *grp = getgrgid(getgid());
 
    // char username[20],password[20];
    // username=pwd->pw_name;
    // password=pwd->pw_passwd;
    printf("username %s password %s, group %s\n",pwd->pw_name,pwd->pw_passwd,grp->gr_name);
    userlist[userlistsize++]=pwd->pw_name;
    printf("username: %s\n",userlist[0]);
    printf("size: %d\n",userlistsize);
    char *mesg="login success!";
    len=strlen(mesg)+1;
    write(sock,mesg,len);
}

void getUidByusername(char* res,int *uid,int* gid){
    IString istr;
    if (  Split(res,"=",&istr) )
    {
        int i=0;
        for (i=0;i<istr.num;i++) {
                char des[20];
                IString dstr;
                printf("%s\n",istr.str[i]);
                if (  Split(istr.str[i],"(",&dstr) ){
                    printf("%d %s\n",i,dstr.str[0]);
                    if(i==0){
                        uid=atoi(dstr.str[0]);
                    }
                    if(i==1){
                        gid=atoi(dstr.str[0]);
                    }
                }
            }
        for (i=0;i<istr.num;i++)
            free(istr.str[i]);
        free(istr.str);
    }
}
//dir command
void cmd_dir(int sock)
{
    DIR *pdir; //目录结构
    char fileName[30];
    char fileInfo[50];
    int i, fcounts=0, len;
    struct dirent *pent;//目录的结构体属性
    int fd;
    struct stat fileSta;//文件属性
    char filePath[200];
    
    pdir=opendir(currentDirPath);//打开目录
    pent=readdir(pdir);//读取目录
    
    while(pent!=NULL)
    {
        fcounts++;//计算文件个数
        pent=readdir(pdir);
    }

    write(sock,&fcounts,sizeof(int));//将文件个数写入sock
    closedir(pdir);//关闭目录

    if(fcounts<=0)
    {
        return;
    }
    else
    {
        pdir=opendir(currentDirPath);
        for(i=0;i<fcounts;i++)
        {
            pent=readdir(pdir);
            memset(fileName,0,30);
            memset(fileInfo,0,sizeof(fileInfo));
            strcpy(fileName,pent->d_name);
            
            //check the file is a directory or a file
            memset(filePath,0,sizeof(filePath));
            strcpy(filePath,currentDirPath);
            strcat(filePath,"/");//链接两个字符串
            strcat(filePath,fileName);
            fd=open(filePath,O_RDONLY, S_IREAD);
            
            fstat(fd,&fileSta);//用来读取打开的文件的属性，stat用来读取没有打开的文件的属性
            if(S_ISDIR(fileSta.st_mode))//判断文件是否为目录
            {
                strcat(fileInfo,"dir\t");
                strcat(fileInfo,fileName);
            }
            else
            {    
                strcat(fileInfo,"file\t");
                strcat(fileInfo,fileName);
            }
            write(sock,fileInfo,sizeof(fileInfo));
        }
        closedir(pdir);
    }
    
    
}

//command cd
void cmd_cd(int sock,char *dirName)
{
    DIR *pdir;
    struct dirent *pent;
    char filename[30];
    int i,fcounts=0;
    int flag=0;

    pdir=opendir(currentDirPath);
    pent=readdir(pdir);
    
    while(pent!=NULL)
    {
        fcounts++;
        pent=readdir(pdir);
    }

    closedir(pdir);

    if(fcounts<=0)
    {
        return;
    }
    else
    {
        pdir=opendir(currentDirPath);
        for(i=0;i<fcounts;i++)
        {
            pent=readdir(pdir);
            if(strcmp(pent->d_name,dirName)==0)
            {
                strcat(currentDirPath,"/");
                strcat(currentDirPath,dirName);
                flag=1;
                break;
            }
        }
        
        if(flag==1)
        {
            write(sock,currentDirPath,sizeof(currentDirPath));
        }
        closedir(pdir);
    }
    
    
}

//command cd..
void cmd_cdback(int sock)
{
    int len;
    int i, record;

    len=strlen(currentDirPath);
    
    for(i=len-1;i>=0;i--)
    {
        if(currentDirPath[i]=='/')
        {
            currentDirPath[i]='\0';
            break;
        }
        currentDirPath[i]='\0';
    }
}
//command ?
void cmd_help(int sock)
{
    int len=strlen(help)+1;
    write(sock,help,len);
}

//command get
void cmd_get(int sock,char*fileName)
{
    int fd;
    struct stat fileSta;
    long fileSize;
    char filePath[200], buf[dataLen];
    
    memset(filePath,0,sizeof(filePath));
    strcpy(filePath,currentDirPath);
    strcat(filePath,"/");
    strcat(filePath,fileName);

    fd=open(filePath,O_RDONLY, S_IREAD);
    if(fd!=-1)
    {
        fstat(fd,&fileSta);
        fileSize=(long) fileSta.st_size;//？
        write(sock,&fileSize,sizeof(long));
        memset(buf,0,dataLen);
        while(fileSize>dataLen)
        {
            read(fd,buf,dataLen);
            write(sock,buf,dataLen);
            fileSize=fileSize-dataLen;
        }
        
        read(fd,buf,fileSize);
        write(sock,buf,fileSize);
        close(fd);
        printf("transfer completed\n");
    }
    else
    {
        printf("open file %s failed\n",filePath);
    }    
}

//command put
void cmd_put(int sock,char *fileName)
{    
    int fd;
    long fileSize;
    
    char filePath[200], buf[dataLen];
    strcpy(filePath,currentDirPath);
    strcat(filePath,"/");
    strcat(filePath,fileName);
    

    fd=open(filePath,O_RDWR|O_CREAT, S_IREAD|S_IWRITE);
    if(fd!=-1)
    {    
        memset(buf,0,dataLen);
        read(sock,&fileSize,sizeof(long));

        while(fileSize>dataLen)
        {
            read(sock,buf,dataLen);
            write(fd,buf,dataLen);
            fileSize=fileSize-dataLen;
        }
        
        read(sock,buf,fileSize);
        write(fd,buf,fileSize);
        close(fd);
        printf("transfer completed\n");
    }
    else
    {
        printf("open file %s failed\n",filePath);
    }
    
}

int Split(char *src, char *delim, IString* istr)
{
    int i;
    char *str = NULL, *p = NULL;
 
    (*istr).num = 1;
	str = (char*)calloc(strlen(src)+1,sizeof(char));
	if (str == NULL) return 0;
    (*istr).str = (char**)calloc(1,sizeof(char *));
    if ((*istr).str == NULL) return 0;
    strcpy(str,src);
 
	p = strtok(str, delim);
	(*istr).str[0] = (char*)calloc(strlen(p)+1,sizeof(char));
	if ((*istr).str[0] == NULL) return 0;
 	strcpy((*istr).str[0],p);
	for(i=1; p = strtok(NULL, delim); i++)
    {
        (*istr).num++;
        (*istr).str = (char**)realloc((*istr).str,(i+1)*sizeof(char *));
        if ((*istr).str == NULL) return 0;
        (*istr).str[i] = (char*)calloc(strlen(p)+1,sizeof(char));
        if ((*istr).str[0] == NULL) return 0;
        strcpy((*istr).str[i],p);
    }
    free(str);
    str = p = NULL;
 
    return 1;
}

//get the last string after the last char '/'
char *getDirName(char *dirPathName)
{
    int i, pos, len;
    char *dirName;

    if(dirPathName==NULL)
    {
        printf("directory absoultly path is null!\n");
        return NULL;
    }    
    
    len=strlen(dirPathName);
    for(i=len-1;i>=0;i--)
    {
        if(dirPathName[i]=='/')
        {
            pos=i;
            break;
        }
    }
    
    dirName=(char *)malloc(len-pos+1);
    for(i=pos+1;i<len;i++)
    {
        dirName[i-pos-1]=dirPathName[i];    
    }
    
    return dirName;
}
void executeCMD(const char *cmd, char *result)   
{   
    char buf_ps[1024];   
    char ps[1024]={0};   
    FILE *ptr;   
    strcpy(ps, cmd);   
    if((ptr=popen(ps, "r"))!=NULL)   
    {   
        while(fgets(buf_ps, 1024, ptr)!=NULL)   
        {   
           strcat(result, buf_ps);   
           if(strlen(result)>1024)   
               break;   
        }   
        pclose(ptr);   
        ptr = NULL;   
    }   
    else  
    {   
        printf("popen %s error\n", ps);   
    }   
}  

void cmd_count_current(int sock){
    char onlineUserstr[64];
     printf("在线 %d 人\n",onlineUser);
    sprintf(onlineUserstr,"%d",onlineUser);
    write(sock,onlineUserstr,64);

}
void cmd_count_all(int sock){

    char onlineUserstr[64];
     printf("在线 %d 人\n",userlistsize);
    sprintf(onlineUserstr,"%d",userlistsize);
    write(sock,onlineUserstr,64);
}
void cmd_list(int sock){
        int i=0;
        char listsize[64];
        sprintf(listsize,"%d",userlistsize);
        write(sock,listsize,64);
    for(i=0;i<userlistsize;i++){
        
        write(sock,userlist[i],64);
    }
}
void cmd_killuser(int sock){
        
        int i=0;
        char listsize[64];
        userlistsize--;
        sprintf(listsize,"%d",userlistsize);
        write(sock,listsize,64);
    for(i=0;i<userlistsize;i++){       
        write(sock,userlist[i],64);
    }
}

bool cmd_register(int sock,char* name,char* pass){
    int i = 0, n;
    if(userlistsize >= 20){
        return false; 
    }
    for(i = 0; i < userlistsize; i++){
        n = strcmp(name, user[i].name);
        if(0 == n){
            printf("该用户已注册\n");
            break;
        }
    }
    strcpy(user[userlistsize].name, name);
    strcpy(user[userlistsize].pass, pass);
    userlistsize++;
    char* msg="注册成功!";
    write(sock,msg,strlen(msg));
    return true;
}

bool login(int sock,char* name,char* pass)
{
    if(0 == userlistsize){
        char* msg="没有此用户,请注册!\n";
        printf("%s",msg);
        write(sock,msg,strlen(msg));
        return false;
    }
    int i, n, m;
    for(i = 0; i < userlistsize; i++){
    //strcmp 函数返回值为"0"则表示两字符串相同
    n = strcmp(user[i].name, name);         
    m = strcmp(user[i].pass, pass);
        if(n == 0 && m == 0){
            userlist[userlistsize++]=name;
    //未来可以提供其它接口，实现各种自定义功能
            char* msg="登录成功!";
            printf("%s",msg);
             write(sock,msg,strlen(msg));
            return true;
        }
    }
    //当程序执行到这一步，意味着结构体数组遍历完毕，未找到匹配的用户和 //密码
    char* msg="用户名或密码错误!";
     printf("%s",msg);
    write(sock,msg,strlen(msg));
    return false;
}