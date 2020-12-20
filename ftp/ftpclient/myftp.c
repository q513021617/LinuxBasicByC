#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#define TRUE 1
#define dataLen 1024
char user_cmd[10],cmd_arg[20];

char buf[dataLen];
typedef struct {
    char **str;     //the PChar of string array
    size_t num;     //the number of string
}IString;

int Split(char *src, char *delim, IString* istr);
void cmd_pwd(int sock,int sockmsg);
void cmd_dir(int sock,int sockmsg);
void cmd_cd(int sock,int sockmsg,char *dirName);
void cmd_cdback(int sock,int sockmsg);
void cmd_help(int sock,int sockmsg);
char *getDirName(char *dirPathName);
void cmd_get(int sock,int sockmsg,char *fileName);
void cmd_put(int sock,int sockmsg,char *fileName);
void cmd_quit(int sock,int sockmsg);
void cmd_count_current(int sock,int sockmsg);
void cmd_list(int sock,int sockmsg);
void cmd_killuser(int sock,int sockmsg);
void login(int sock,int sockmsg);
void cmd_ldir();
void cmd_lcd();
void cmd_lpwd();
void cmd_lrmdir();
void cmd_lmkdir();
void getUsernameAndpassword(char* argc);
char user_name[20],user_password[20];
char currentDirName[30];
char lcurrentDirPath[300];
char port[20];
char ipaddr[50];
int main(int argc,char *argv[])
{
  int cmd_len,arg_len;
  int sock, sockmsg;
  struct sockaddr_in server, servermsg;
  struct hostent *hp;
 
  sock=socket(AF_INET,SOCK_STREAM,0);
  sockmsg=socket(AF_INET,SOCK_STREAM,0);
     
  if (sock<0||sockmsg<0)
  {
    printf("opening stream socket");
    exit(1);
  }
  getUsernameAndpassword(argv[1]);
  hp = gethostbyname(ipaddr);
  if (hp==0)
  {
    fprintf(stderr,"%s:unknown host\n",ipaddr);
    exit(2);
  }
 
  server.sin_family=AF_INET;
  server.sin_port=htons(atoi(port));
  memcpy((char*)&server.sin_addr, (char*)hp->h_addr, hp->h_length);
 
  servermsg.sin_family=AF_INET;
  servermsg.sin_port=htons(atoi(port)+1);
  memcpy((char*)&servermsg.sin_addr, (char*)hp->h_addr, hp->h_length);
 
  if (connect(sock,(struct sockaddr *)&server,sizeof server)<0||connect(sockmsg,(struct sockaddr *)&servermsg,sizeof servermsg)<0)
  {
     printf("connecting stream socket");
    exit(1);
  }
 
   memset(lcurrentDirPath,0,sizeof(lcurrentDirPath));//将currentDirpath置零
   getcwd(lcurrentDirPath,sizeof(lcurrentDirPath));//得到当前路径，保存在currentDirPath中
   memset(user_cmd,0,10);
   strcpy(user_cmd,"login");
   login(sock,sockmsg);
  //Get command from command line and do just the action
  while(TRUE)
  {
    memset(user_cmd,0,10);
    memset(cmd_arg,0,20);

    //First get the input character by user
    printf("command: ");
    scanf("%s",user_cmd);
    cmd_len=strlen(user_cmd);
    //Then get the command character and the argument
    if(strcmp(user_cmd,"quit")==0)        //command "quit"
    {
        cmd_quit(sock,sockmsg);
        close(sockmsg);
        close(sock);
        printf("connection closed\n\n");
        exit(0);
    }     
    else if(strcmp(user_cmd,"?")==0)      //command "?"
    {
        cmd_help(sock,sockmsg);
    }
    else if(strcmp(user_cmd,"pwd")==0)     //command "pwd"
    {
        cmd_pwd(sock,sockmsg);
    }
    else if (strcmp(user_cmd,"mkdir")==0)
    {
        scanf("%s",cmd_arg);
        cmd_mkdir(sock,sockmsg);
    }
     else if(strcmp(user_cmd,"login")==0)     //command "pwd"
    {
        printf("username: ");
        scanf("%s",user_name);
        printf("password: ");
        scanf("%s",user_password);
        login(sock,sockmsg);
    }
    else if (strcmp(user_cmd,"register")==0)
    {
        printf("username: ");
        scanf("%s",&user_name);
        printf("password: ");
        scanf("%s",&user_password);
        cmd_register(sock,sockmsg);
    }
    
    else if(strcmp(user_cmd,"ls")==0)      //command "dir"
    {
        cmd_dir(sock,sockmsg);
    }
     else if(strcmp(user_cmd,"rmdir")==0)      //command "dir"
    {
        scanf("%s",cmd_arg);
        cmd_rmdir(sock,sockmsg);
    }
     else if(strcmp(user_cmd,"lrmdir")==0)      //command "dir"
    {
        scanf("%s",cmd_arg);
        cmd_lrmdir();
    }
     else if (strcmp(user_cmd,"lmkdir")==0)
    {
        scanf("%s",cmd_arg);
        cmd_lmkdir();
    }
     else if(strcmp(user_cmd,"count")==0)      //command "dir"
    {
        scanf("%s",cmd_arg);
        cmd_count_current(sock,sockmsg);
    }else if(strcmp(user_cmd,"dir")==0)      //command "dir"
    {
        
        cmd_ldir();
    }
    else if(strcmp(user_cmd,"cd")==0)    //commad "cd"
    {
        //input command argument
        scanf("%s",cmd_arg);
        cmd_cd(sock,sockmsg,cmd_arg);
    }
    else if (strcmp(user_cmd,"lcd")==0)
    {
        scanf("%s",cmd_arg);
        cmd_lcd();
    }
     else if (strcmp(user_cmd,"lpwd")==0)
    {
        cmd_lpwd();
    }
    else if (strcmp(user_cmd,"list")==0)
    {
        cmd_list(sock,sockmsg);
    }
    else if (strcmp(user_cmd,"kill")==0)
    {
        scanf("%s",cmd_arg);
        cmd_killuser(sock,sockmsg);
    }
    else if(strcmp(user_cmd,"cd..")==0)
    {
        cmd_cdback(sock,sockmsg);
    }
    else if(strcmp(user_cmd,"get")==0)    //command "get"
    {
        //input command argument
        scanf("%s",cmd_arg);
        cmd_get(sock,sockmsg,cmd_arg);
    }
    else if(strcmp(user_cmd,"put")==0)    //command "put"
    {   
        //input command argument
        scanf("%s",cmd_arg);
        cmd_put(sock,sockmsg,cmd_arg);
    }
    else
    {
        printf("bad command!\n");
    }
  } 
 
}

void cmd_login(int sock,int sockmsg){
    char message[30];
    struct passwd *pwd;
    write(sockmsg,user_cmd,sizeof(user_cmd));
    printf("username: ");
    pwd = getpwuid(getuid());
    
    strcpy(user_name,pwd->pw_name);
    write(sockmsg,user_name,sizeof(user_name));

    read(sock,message,30);
    printf("%s\n",message);
}
//command pwd
void cmd_pwd(int sock,int sockmsg)
{
    //int numRead;
    char dirName[30];
    write(sockmsg,user_cmd,sizeof(user_cmd));
    read(sock,dirName,30);
    printf("%s\n",dirName);
}
//command dir
void cmd_dir(int sock, int sockmsg)
{
    
   
    write(sockmsg,user_cmd,sizeof(user_cmd));
    show_dir(sock);
}
void cmd_rmdir(int sock, int sockmsg)
{
    
   
    write(sockmsg,user_cmd,sizeof(user_cmd));
    write(sockmsg,cmd_arg,sizeof(cmd_arg));
    show_dir(sock);
}
void show_dir(int sock){
     
    int i, fileNum=0;
    char fileInfo[50];
    read(sock,&fileNum,sizeof(int));
    printf("--------------------------------------------------------\n");
    printf("file number : %d\n",fileNum);
    if(fileNum>0)
    {
        for(i=0;i<fileNum;i++)
        {
            memset(fileInfo,0,sizeof(fileInfo));
            read(sock,fileInfo,sizeof(fileInfo));
            printf("%s\n",fileInfo);
        }
        printf("--------------------------------------------------------\n");
    }
    else if(fileNum==0)
    {
        printf("directory of server point is empty.\n");
        return;
    }
    else
    {
        printf("error in command 'dir'\n");
        return;
    }
}
//command cd
void cmd_cd(int sock,int sockmsg,char *dirName)
{
    char currentDirPath[200];
    write(sockmsg,user_cmd,sizeof(user_cmd));
    write(sockmsg,cmd_arg,sizeof(cmd_arg));
    read(sock,currentDirPath,sizeof(currentDirPath));
   
    printf("now in directory : %s\n",currentDirPath);
}
//command cd
void cmd_mkdir(int sock,int sockmsg,char *dirName)
{
     int i, fileNum=0;
    char fileInfo[50];
    char currentDirPath[200];
    write(sockmsg,user_cmd,sizeof(user_cmd));
    write(sockmsg,cmd_arg,sizeof(cmd_arg));
     read(sock,&fileNum,sizeof(int));

    printf("--------------------------------------------------------\n");
    printf("file number : %d\n",fileNum);
    if(fileNum>0)
    {
        for(i=0;i<fileNum;i++)
        {
            memset(fileInfo,0,sizeof(fileInfo));
            read(sock,fileInfo,sizeof(fileInfo));
            printf("%s\n",fileInfo);
        }
        printf("--------------------------------------------------------\n");
    }
    else if(fileNum==0)
    {
        printf("directory of server point is empty.\n");
        return;
    }
    else
    {
        printf("error in command 'dir'\n");
        return;
    }
}
int Split(char *src, char *delim, IString* istr)//split buf
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
//command cd..
void cmd_cdback(int sock,int sockmsg)
{
    write(sockmsg,user_cmd,sizeof(user_cmd));   
}
void cmd_register(int sock,int sockmsg)
{
    write(sockmsg,user_cmd,sizeof(user_cmd));
    write(sockmsg,user_name,sizeof(user_name));
    write(sockmsg,user_password,sizeof(user_password));
    char msg[20];
    read(sock,msg,20);
    printf("%s",msg);
}
void getUsernameAndpassword(char* argc){

    IString istr;
    
    printf("%s \n",argc);
    Split(argc,":",&istr);

    strcpy(user_name,istr.str[0]);
    strcpy(port,istr.str[2]);
    printf("username : %s\n",user_name);
    char tempstr[100];
     strcpy(tempstr,istr.str[1]);
    Split(tempstr,"@",&istr);
     strcpy(user_password,istr.str[0]);

    strcpy(tempstr,istr.str[1]);
    printf("userpassword : %s\n",user_password);

 
    strcpy(ipaddr,tempstr);
    printf("ipaddr : %s\n",ipaddr);
   
    printf("port : %s\n",port);

}
void login(int sock,int sockmsg)
{
    write(sockmsg,user_cmd,sizeof(user_cmd));
    write(sockmsg,user_name,sizeof(user_name));
    write(sockmsg,user_password,sizeof(user_password));
    char msg[60];
    read(sock,msg,60);
    printf("%s\n",msg);
}
//command quit
void cmd_quit(int sock,int sockmsg)
{
    write(sockmsg,user_cmd,sizeof(user_cmd));
}

//command help
void cmd_help(int sock, int sockmsg)
{
    char help[1000];
    write(sockmsg,user_cmd,sizeof(user_cmd));
    read(sock,help,1000);
   
    printf("%s\n",help);   
}

//command get
void cmd_get(int sock,int sockmsg,char *fileName)
{
    int fd;
    long fileSize;
    char localFilePath[200];

    write(sockmsg,user_cmd,sizeof(user_cmd));
    write(sockmsg,cmd_arg,sizeof(cmd_arg));
    printf("%s\n%s\n",user_cmd,cmd_arg);
   
    memset(localFilePath,0,sizeof(localFilePath));
    getcwd(localFilePath,sizeof(localFilePath));
    strcat(localFilePath,"/");
    strcat(localFilePath,fileName);

    fd=open(localFilePath,O_RDWR|O_CREAT, S_IREAD|S_IWRITE);
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
        printf("download completed\n");
    }
    else
    {
        printf("open file %s failed\n",localFilePath);
    }
}

//command put
void cmd_put(int sock,int sockmsg,char* fileName)
{   
    write(sockmsg,user_cmd,sizeof(user_cmd));
    write(sockmsg,cmd_arg,sizeof(cmd_arg));
   
    int fd;
    long fileSize;
    int numRead;
    char filePath[200];
    struct stat fileSta;

    memset(filePath,0,sizeof(filePath));
    getcwd(filePath,sizeof(filePath));
   
    strcat(filePath,"/");
    strcat(filePath,fileName);
   
    fd=open(filePath,O_RDONLY, S_IREAD);

    if(fd!=-1)
    {
        fstat(fd,&fileSta);
        fileSize=(long) fileSta.st_size;

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
        printf("upload completed\n");
    }
    else
    {
        printf("open file %s failed\n",filePath);
    }   
}

void cmd_count_current(int sock,int sockmsg){
    int fileNum=0;
    char fileNumstr[64];
    write(sockmsg,user_cmd,sizeof(user_cmd));
    write(sockmsg,cmd_arg,sizeof(cmd_arg));
    read(sock,fileNumstr,64);
    if(strcmp(cmd_arg,"current")==0)
        printf("在线人数 %s 人\n",fileNumstr);
      if(strcmp(cmd_arg,"all")==0)
        printf("所有用户人数 %s 人\n",fileNumstr);
}

void cmd_list(int sock,int sockmsg){
    int userNum=0;
    char userNumstr[64];
    write(sockmsg,user_cmd,sizeof(user_cmd));
    read(sock,userNumstr,64);
    printf("在线人数 %s 人\n",userNumstr);
    userNum=atoi(userNumstr);
    printf("用户列表: \n",userNumstr);
    while (userNum>0)
    {
        char userstr[64];
        read(sock,userstr,64);
        printf("%s\n",userstr);
        userNum--;
    }
    
}

void cmd_killuser(int sock,int sockmsg){
     int userNum=0;
    char userNumstr[64];
    write(sockmsg,user_cmd,sizeof(user_cmd));
     write(sockmsg,cmd_arg,sizeof(cmd_arg));
    read(sock,userNumstr,64);
    printf("在线人数 %s 人\n",userNumstr);
    userNum=atoi(userNumstr);
    printf("用户列表: \n",userNumstr);
    while (userNum>0)
    {
        char userstr[64];
        read(sock,userstr,64);
        printf("%s\n",userstr);
        userNum--;
    }
}
void cmd_lpwd()
{
    int len;
    memset(lcurrentDirPath,0,sizeof(lcurrentDirPath));
    getcwd(lcurrentDirPath,sizeof(lcurrentDirPath));
    char *savePointer=getDirName(lcurrentDirPath);
    strcpy(currentDirName,savePointer);
    printf("%s \n",currentDirName);
}

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
void cmd_lmkdir(){
    char DirName[256];  
      strcpy(DirName, cmd_arg);
    // char *tempstr="create dir ";
    // strcat(tempstr,dirName);
    printf("create dir %s \n",DirName);
    
    if(mkdir(DirName, 0755)==-1)  
                  {   
                      printf("mkdir   error\n");   
                      return -1;   
                  }  
    cmd_ldir();
}
void cmd_lrmdir(){
    char DirName[256];  
      strcpy(DirName, cmd_arg);
    // char *tempstr="create dir ";
    // strcat(tempstr,dirName);
    printf("create dir %s \n",DirName);
    
    if(remove(DirName)==-1)  
                  {   
                      printf("mkdir   error\n");   
                      return -1;   
                  }  
    cmd_ldir();
}
void cmd_ldir()
{
    DIR *pdir; //目录结构
    char fileName[30];
    char fileInfo[50];
    int i, fcounts=0, len;
    struct dirent *pent;//目录的结构体属性
    int fd;
    struct stat fileSta;//文件属性
    char filePath[200];
    
    pdir=opendir(lcurrentDirPath);//打开目录
    pent=readdir(pdir);//读取目录
    
    while(pent!=NULL)
    {
        fcounts++;//计算文件个数
        pent=readdir(pdir);
    }

    closedir(pdir);//关闭目录

    if(fcounts<=0)
    {
        return;
    }
    else
    {
        pdir=opendir(lcurrentDirPath);
        for(i=0;i<fcounts;i++)
        {
            pent=readdir(pdir);
            memset(fileName,0,30);
            memset(fileInfo,0,sizeof(fileInfo));
            strcpy(fileName,pent->d_name);
            
            //check the file is a directory or a file
            memset(filePath,0,sizeof(filePath));
            strcpy(filePath,lcurrentDirPath);
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
            printf("%s\n",fileInfo);
        }
        closedir(pdir);
    }
    
    
}

void cmd_lcd()
{
    DIR *pdir;
    struct dirent *pent;
    char filename[30];
    int i,fcounts=0;
    int flag=0;
    char* dirName;
    dirName=cmd_arg;
    pdir=opendir(lcurrentDirPath);
    pent=readdir(pdir);
    
    while(pent!=NULL)
    {
        fcounts++;
        pent=readdir(pdir);
        printf("");
    }

    closedir(pdir);

    if(fcounts<=0)
    {
        return;
    }
    else
    {
        pdir=opendir(lcurrentDirPath);
        for(i=0;i<fcounts;i++)
        {
            pent=readdir(pdir);
            if(strcmp(pent->d_name,dirName)==0)
            {
                strcat(lcurrentDirPath,"/");
                strcat(lcurrentDirPath,dirName);
                flag=1;
                break;
            }
        }
        
        if(flag==1)
        {
            printf("%s \n",lcurrentDirPath);
        }
        closedir(pdir);
    }
    
    
}