/*
任务与任务之间的通信 全局变量
全局变量的局限性 一个任务写 其余任务只能读
因为存在Mutex互斥
*/

/*
ESP32 CPU 32bit 4byte
int long是4byte
但是Arduino的int是2byte
所以使用 uint32_t
*/

/*
Wi-Fi 蓝牙在核心 0
所以 大量使用Wi-Fi的代码用核心1
避免造成核心0负载
*/

void taskA(void *) {
  while (1) {
    Serial.println(xPortGetCoreID());
  }
}


void setup() {
  xTaskCreatePinnedToCore(
    taskA,    //函数
    "taskA",  //取名
    1024,     //盏分配 1024 2048...字节
    NULL,     //指针 函数传参
    1,        //任务等级
    NULL,     //操作任务的指针
    0         //核心0
  );
}

void loop() {
  // put your main code here, to run repeatedly:
}