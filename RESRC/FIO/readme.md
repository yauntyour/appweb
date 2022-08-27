# FIO——提供基于posix thread 原理的异步FIO（AIO）

核心函数包括

1. Asfio Asfio_create(const char *filepath,const char *mode) （创建一个AIO文件指针）
2. int Asfio_close(Asfio *asp, int mode) （等待所有异步操作执行完成后销毁Asfio *）
3. CALLBLACK_TH Asfio_readall(Asfio *asp) （异步读取整个文件的内容）
4. size_t Asfio_callblack(CALLBLACK_TH th,bytes *buf) （回调获取异步读取的内容）
5. int Asfio_write(Asfio *asp, const void *Element) （异步写入）
5. size_t Asfio_readcall(Asfio *asp, bytes *buf)（同步读取）

其他相关依赖

- pthread.h 提供多线程支持
- bytes.h 提供byter的操作

相关信息

- 可以利用以下函数实现UNIX下的FIFO通信（此处不提供支持）
- - FILE *fdopen(int fd, const char *mode); 
  - int fileno(FILE *stream);