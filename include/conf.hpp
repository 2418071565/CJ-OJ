#pragma once 

#define CJOJ_BEGIN namespace CJ {
#define CJOJ_END }

#define DEFAULT_PORT 8888  // 默认端口
#define MAX_READY_TASKS 64 // 一次循环最大就绪任务处理数量
#define TIME_OUT 1000      // 超时时间