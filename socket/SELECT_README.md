# 理解select模型

理解select模型的关键在于理解fd_set,为说明方便，取fd_set长度为1字节，fd_set中的每一bit可以对应一个文件描述符fd。则1字节长的fd_set最大可以对应8个fd。

（1）执行fd_set set; FD_ZERO(&set);则set用位表示是0000,0000。

（2）若fd＝5,执行FD_SET(fd,&set);后set变为0001,0000(第5位置为1)

（3）若再加入fd＝2，fd=1,则set变为0001,0011

（4）执行select(6,&set,0,0,0)阻塞等待

（5）若fd=1,fd=2上都发生可读事件，则select返回，此时set变为0000,0011。注意：没有事件发生的fd=5被清空。

　基于上面的讨论，可以轻松得出select模型的特点：

　　（1)可监控的文件描述符个数取决与sizeof(fd_set)的值。我这边服务 器上sizeof(fd_set)＝512，每bit表示一个文件描述符，则我服务器上支持的最大文件描述符是512*8=4096。据说可调，另有说虽 然可调，但调整上限受于编译内核时的变量值。本人对调整fd_set的大小不太感兴趣，参考http://www.cppblog.com /CppExplore/archive/2008/03/21/45061.html中的模型2（1）可以有效突破select可监控的文件描述符上 限。

　　（2）将fd加入select监控集的同时，还要再使用一个数据结构array保存放到select监控集中的fd，一是用于再select 返回后，array作为源数据和fd_set进行FD_ISSET判断。二是select返回后会把以前加入的但并无事件发生的fd清空，则每次开始 select前都要重新从array取得fd逐一加入（FD_ZERO最先），扫描array的同时取得fd最大值maxfd，用于select的第一个 参数。

　　（3）可见select模型必须在select前循环array（加fd，取maxfd），select返回后循环array（FD_ISSET判断是否有时间发生）。

参考链接
http://www.cnblogs.com/Anker/p/3265058.html
https://blog.csdn.net/turkeyzhou/article/details/8609360
https://blog.csdn.net/qq546770908/article/details/53082870
http://www.cnblogs.com/Anker/archive/2013/08/14/3258674.html

源码分析 select
http://www.cnblogs.com/apprentice89/archive/2013/05/09/3064975.html