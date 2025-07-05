# 《Linux 系统中几种常见的进程间通信方式及示例》

在 Linux 系统中，进程间通信（IPC, Inter-Process Communication）是实现不同进程协同工作的关键机制。以下是几种常见的 IPC 方式及其示例代码：


### **1. 管道（Pipe）**

#### **匿名管道（Anonymous Pipe）**



*   **特点**：单向通信，只能在父子进程间使用。


*   **示例**：父进程发送数据，子进程接收。




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<unistd.h>


int main() {


&#x20;   int fd\[2];  // 管道文件描述符：fd\[0]读，fd\[1]写


&#x20;   pid\_t pid;


&#x20;   char buffer\[100];


&#x20;   // 创建管道


&#x20;   if (pipe(fd) == -1) {


&#x20;       perror("pipe");


&#x20;       exit(EXIT\_FAILURE);


&#x20;   }


&#x20;   // 创建子进程


&#x20;   if ((pid = fork()) == -1) {


&#x20;       perror("fork");


&#x20;       exit(EXIT\_FAILURE);


&#x20;   }


&#x20;   if (pid == 0) {  // 子进程（接收数据）


&#x20;       close(fd\[1]);  // 关闭写端


&#x20;       read(fd\[0], buffer, sizeof(buffer));


&#x20;       printf("子进程收到：%s\n", buffer);


&#x20;       close(fd\[0]);


&#x20;   } else {  // 父进程（发送数据）


&#x20;       close(fd\[0]);  // 关闭读端


&#x20;       write(fd\[1], "Hello from parent!", 18);


&#x20;       close(fd\[1]);


&#x20;       wait(NULL);  // 等待子进程结束


&#x20;   }


&#x20;   return 0;


}
```

#### **命名管道（Named Pipe, FIFO）**



*   **特点**：通过文件系统路径名标识，可在无关进程间通信。


*   **示例**：两个程序通过 FIFO 通信。


**写进程（fifo\_writer.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<fcntl.h>


\#include \<unistd.h>


int main() {


&#x20;   int fd;


&#x20;   char \*fifo\_path = "/tmp/myfifo";


&#x20;   char \*message = "Hello from FIFO writer!";


&#x20;   // 创建命名管道（如果不存在）


&#x20;   if (mkfifo(fifo\_path, 0666) == -1) {


&#x20;       if (errno != EEXIST) {


&#x20;           perror("mkfifo");


&#x20;           exit(EXIT\_FAILURE);


&#x20;       }


&#x20;   }


&#x20;   // 打开管道并写入数据


&#x20;   fd = open(fifo\_path, O\_WRONLY);


&#x20;   write(fd, message, sizeof(message));


&#x20;   close(fd);


&#x20;   printf("数据已写入FIFO\n");


&#x20;   return 0;


}
```

**读进程（fifo\_reader.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<fcntl.h>


\#include \<unistd.h>


int main() {


&#x20;   int fd;


&#x20;   char \*fifo\_path = "/tmp/myfifo";


&#x20;   char buffer\[100];


&#x20;   // 打开管道并读取数据


&#x20;   fd = open(fifo\_path, O\_RDONLY);


&#x20;   read(fd, buffer, sizeof(buffer));


&#x20;   printf("从FIFO读取到：%s\n", buffer);


&#x20;   close(fd);


&#x20;   // 删除命名管道


&#x20;   unlink(fifo\_path);


&#x20;   return 0;


}
```

### **2. 消息队列（Message Queue）**



*   **特点**：通过内核消息队列传递格式化数据，支持多进程读写。


*   **示例**：发送和接收消息。


**发送进程（msg\_sender.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<string.h>


\#include \<sys/msg.h>


struct msg\_buffer {


&#x20;   long msg\_type;


&#x20;   char message\[100];


} message;


int main() {


&#x20;   key\_t key;


&#x20;   int msgid;


&#x20;   // 生成唯一键值


&#x20;   key = ftok(".", 'a');


&#x20;   // 创建消息队列


&#x20;   msgid = msgget(key, 0666 | IPC\_CREAT);


&#x20;   // 准备消息


&#x20;   message.msg\_type = 1;


&#x20;   strcpy(message.message, "Hello from message queue!");


&#x20;   // 发送消息


&#x20;   msgsnd(msgid, \&message, sizeof(message), 0);


&#x20;   printf("消息已发送\n");


&#x20;   return 0;


}
```

**接收进程（msg\_receiver.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<sys/msg.h>


struct msg\_buffer {


&#x20;   long msg\_type;


&#x20;   char message\[100];


} message;


int main() {


&#x20;   key\_t key;


&#x20;   int msgid;


&#x20;   // 生成相同键值


&#x20;   key = ftok(".", 'a');


&#x20;   // 获取消息队列


&#x20;   msgid = msgget(key, 0666 | IPC\_CREAT);


&#x20;   // 接收消息


&#x20;   msgrcv(msgid, \&message, sizeof(message), 1, 0);


&#x20;   printf("收到消息：%s\n", message.message);


&#x20;   // 删除消息队列


&#x20;   msgctl(msgid, IPC\_RMID, NULL);


&#x20;   return 0;


}
```

### **3. 共享内存（Shared Memory）**



*   **特点**：多个进程映射同一块物理内存，读写效率最高。


*   **示例**：一个进程写入数据，另一个进程读取。


**写进程（shm\_writer.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<string.h>


\#include \<sys/ipc.h>


\#include \<sys/shm.h>


int main() {


&#x20;   key\_t key;


&#x20;   int shmid;


&#x20;   char \*shm\_addr;


&#x20;   char \*message = "Hello from shared memory!";


&#x20;   // 生成键值


&#x20;   key = ftok(".", 'a');


&#x20;   // 创建共享内存段（大小为1024字节）


&#x20;   shmid = shmget(key, 1024, 0666 | IPC\_CREAT);


&#x20;   // 连接共享内存


&#x20;   shm\_addr = shmat(shmid, NULL, 0);


&#x20;   // 写入数据


&#x20;   strcpy(shm\_addr, message);


&#x20;   printf("数据已写入共享内存\n");


&#x20;   // 断开连接


&#x20;   shmdt(shm\_addr);


&#x20;   return 0;


}
```

**读进程（shm\_reader.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<sys/ipc.h>


\#include \<sys/shm.h>


int main() {


&#x20;   key\_t key;


&#x20;   int shmid;


&#x20;   char \*shm\_addr;


&#x20;   // 生成相同键值


&#x20;   key = ftok(".", 'a');


&#x20;   // 获取共享内存段


&#x20;   shmid = shmget(key, 1024, 0666);


&#x20;   // 连接共享内存


&#x20;   shm\_addr = shmat(shmid, NULL, 0);


&#x20;   // 读取数据


&#x20;   printf("从共享内存读取到：%s\n", shm\_addr);


&#x20;   // 断开连接并删除共享内存


&#x20;   shmdt(shm\_addr);


&#x20;   shmctl(shmid, IPC\_RMID, NULL);


&#x20;   return 0;


}
```

### **4. 信号量（Semaphore）**



*   **特点**：用于进程间同步，防止竞态条件（Race Condition）。


*   **示例**：两个进程通过信号量控制共享资源访问。


**信号量初始化（sem\_init.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<sys/sem.h>


union semun {


&#x20;   int val;


&#x20;   struct semid\_ds \*buf;


&#x20;   unsigned short \*array;


};


int main() {


&#x20;   key\_t key;


&#x20;   int semid;


&#x20;   union semun arg;


&#x20;   // 生成键值


&#x20;   key = ftok(".", 'a');


&#x20;   // 创建信号量集（包含1个信号量）


&#x20;   semid = semget(key, 1, 0666 | IPC\_CREAT);


&#x20;   // 初始化信号量值为1（表示资源可用）


&#x20;   arg.val = 1;


&#x20;   semctl(semid, 0, SETVAL, arg);


&#x20;   printf("信号量已初始化，ID: %d\n", semid);


&#x20;   return 0;


}
```

**资源访问进程（sem\_access.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<sys/sem.h>


// P操作（获取资源）


void P(int semid) {


&#x20;   struct sembuf op = {0, -1, SEM\_UNDO};


&#x20;   semop(semid, \&op, 1);


}


// V操作（释放资源）


void V(int semid) {


&#x20;   struct sembuf op = {0, 1, SEM\_UNDO};


&#x20;   semop(semid, \&op, 1);


}


int main() {


&#x20;   key\_t key;


&#x20;   int semid;


&#x20;   // 生成相同键值


&#x20;   key = ftok(".", 'a');


&#x20;   // 获取信号量集


&#x20;   semid = semget(key, 1, 0666);


&#x20;   // 模拟访问共享资源


&#x20;   printf("尝试获取资源...\n");


&#x20;   P(semid);


&#x20;   printf("已获取资源，正在操作...\n");


&#x20;   sleep(2);  // 模拟操作耗时


&#x20;   printf("操作完成，释放资源\n");


&#x20;   V(semid);


&#x20;   return 0;


}
```

### **5. 套接字（Socket）**



*   **特点**：跨主机通信，也可用于本地进程间通信。


*   **示例**：本地 TCP 通信（服务端 / 客户端）。


**服务端（socket\_server.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<string.h>


\#include \<unistd.h>


\#include \<arpa/inet.h>


int main() {


&#x20;   int server\_fd, client\_fd;


&#x20;   struct sockaddr\_in server\_addr, client\_addr;


&#x20;   char buffer\[1024] = {0};


&#x20;   socklen\_t client\_addr\_len = sizeof(client\_addr);


&#x20;   // 创建套接字


&#x20;   server\_fd = socket(AF\_INET, SOCK\_STREAM, 0);


&#x20;   // 配置地址


&#x20;   server\_addr.sin\_family = AF\_INET;


&#x20;   server\_addr.sin\_addr.s\_addr = INADDR\_ANY;


&#x20;   server\_addr.sin\_port = htons(8080);


&#x20;   // 绑定套接字


&#x20;   bind(server\_fd, (struct sockaddr \*)\&server\_addr, sizeof(server\_addr));


&#x20;   // 监听连接


&#x20;   listen(server\_fd, 3);


&#x20;   printf("服务器监听中...\n");


&#x20;   // 接受客户端连接


&#x20;   client\_fd = accept(server\_fd, (struct sockaddr \*)\&client\_addr, \&client\_addr\_len);


&#x20;   printf("客户端已连接\n");


&#x20;   // 接收数据


&#x20;   read(client\_fd, buffer, 1024);


&#x20;   printf("客户端消息：%s\n", buffer);


&#x20;   // 发送响应


&#x20;   char \*response = "Hello from server!";


&#x20;   write(client\_fd, response, strlen(response));


&#x20;   printf("响应已发送\n");


&#x20;   // 关闭连接


&#x20;   close(client\_fd);


&#x20;   close(server\_fd);


&#x20;   return 0;


}
```

**客户端（socket\_client.c）**：




```
\#include \<stdio.h>


\#include \<stdlib.h>


\#include \<string.h>


\#include \<unistd.h>


\#include \<arpa/inet.h>


int main() {


&#x20;   int sock = 0;


&#x20;   struct sockaddr\_in serv\_addr;


&#x20;   char \*message = "Hello from client!";


&#x20;   char buffer\[1024] = {0};


&#x20;   // 创建套接字


&#x20;   sock = socket(AF\_INET, SOCK\_STREAM, 0);


&#x20;   // 配置服务器地址


&#x20;   serv\_addr.sin\_family = AF\_INET;


&#x20;   serv\_addr.sin\_port = htons(8080);


&#x20;   // 将IPv4地址从点分十进制转换为二进制


&#x20;   inet\_pton(AF\_INET, "127.0.0.1", \&serv\_addr.sin\_addr);


&#x20;   // 连接服务器


&#x20;   connect(sock, (struct sockaddr \*)\&serv\_addr, sizeof(serv\_addr));


&#x20;   printf("已连接到服务器\n");


&#x20;   // 发送数据


&#x20;   write(sock, message, strlen(message));


&#x20;   printf("消息已发送\n");


&#x20;   // 接收响应


&#x20;   read(sock, buffer, 1024);


&#x20;   printf("服务器响应：%s\n", buffer);


&#x20;   // 关闭套接字


&#x20;   close(sock);


&#x20;   return 0;


}
```

### **总结**



| IPC 方式&#xA;      | 适用场景&#xA;         | 优点&#xA;           | 缺点&#xA;              |
| ---------------- | ----------------- | ----------------- | -------------------- |
| 匿名管道&#xA;        | 父子进程单向通信&#xA;     | 简单易用&#xA;         | 单向、只能亲缘进程&#xA;       |
| 命名管道 (FIFO)&#xA; | 无关进程间通信&#xA;      | 基于文件系统，灵活&#xA;    | 效率低，需处理同步&#xA;       |
| 消息队列&#xA;        | 格式化数据传输&#xA;      | 异步通信，支持多进程读写&#xA; | 容量有限，需手动管理&#xA;      |
| 共享内存&#xA;        | 高性能数据共享&#xA;      | 读写效率最高&#xA;       | 需手动处理同步，易产生竞态条件&#xA; |
| 信号量&#xA;         | 进程间同步与互斥&#xA;     | 防止竞态条件&#xA;       | 编程复杂度高&#xA;          |
| 套接字&#xA;         | 跨主机 / 本地进程通信&#xA; | 跨平台、灵活&#xA;       | 网络通信开销大&#xA;         |

如果需要更深入的示例或特定场景的优化方案（如多线程同步、大数据传输），请告诉我具体需求，我会进一步调整代码。


> （注：文档部分内容可能由 AI 生成）
>