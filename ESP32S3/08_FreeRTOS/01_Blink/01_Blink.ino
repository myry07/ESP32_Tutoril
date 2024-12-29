void task1(void *param) {
  //这边就是setup
  pinMode(22, OUTPUT);

  while (1) {  //这边就是loop
    digitalWrite(22, 1);
    vTaskDelay(1000);  //该delay不会导致其他任务延迟 1000 -> 1000ms
  }
}

void task2(void *param) {
  //这边就是setup
  pinMode(23, OUTPUT);

  while (1) {  //这边就是loop
    digitalWrite(23, 1);
    vTaskDelay(2000);  //该delay不会导致其他任务延迟 2000 -> 2000ms
  }
}

void setup() {
  xTaskCreate(
    task1,      //函数
    "blink 1",  //取名
    1024,       //盏分配 1024 2048...字节
    NULL,       //指针 函数传参
    1,          //核心
    NULL        //操作任务的指针
  );

  xTaskCreate(
    task2,      //函数
    "blink 2",  //取名
    1024,       //盏分配 1024 2048...字节
    NULL,       //指针 函数传参
    1,          //核心
    NULL        //操作任务的指针
  );
}

void loop() {
  // put your main code here, to run repeatedly:
}
