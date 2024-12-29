byte LED1 = 22;
byte LED2 = 23;
//变量无法直接传入task中
//必须以指针的形式


// & 取地址
// void * 空指针
// byte * 字节指针
// * 打印地址的数据

void task1(void *param) {
  //数据还原 逆向操作
  byte *ptb_LED1 = (byte *)param;
  byte LED = *ptb_LED1;

  while (1) {  //这边就是loop
    digitalWrite(LED, 1);
    vTaskDelay(1000);  //该delay不会导致其他任务延迟 1000 -> 1000ms
  }
}

void task2(void *param) {
  byte LED = *(byte *)param; //先强转指针类型 再取变量的数据

  while (1) {  //这边就是loop
    digitalWrite(LED, 1);
    vTaskDelay(2000);  //该delay不会导致其他任务延迟 2000 -> 2000ms
  }
}

void setup() {

  byte *ptb_LED1;    //定义字节指针
  ptb_LED1 = &LED1;  //赋值

  void *pt_LED1;               //定义空指针 因为只能传入空指针参数
  pt_LED1 = (void *)ptb_LED1;  //指针格式转换




  xTaskCreate(
    task1,      //函数
    "blink 1",  //取名
    1024,       //盏分配 1024 2048...字节
    pt_LED1,    //指针 函数传参
    1,          //任务等级
    NULL        //操作任务的指针
  );

  xTaskCreate(
    task2,          //函数
    "blink 2",      //取名
    1024,           //盏分配 1024 2048...字节
    (void *)&LED2,  //指针 函数传参.  &LED2 取了地址才能强转指针类型
    1,              //任务等级
    NULL            //操作任务的指针
  );
}

void loop() {
  // put your main code here, to run repeatedly:
}
