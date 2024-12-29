//定义结构体
typedef struct {
  byte pin;
  int delayTime;
} LEDFlash;
//变量无法直接传入task中
//必须以指针的形式


// & 取地址
// void * 空指针
// byte * 字节指针
// * 打印地址的数据
// -> * 无法打印结构体中变量的数据

void blind(void *param) {
  LEDFlash *pt = (LEDFlash *)param;  //将传入的空指针转成LEDFlash类型的指针
  byte pin = pt->pin;                //对于结构体就不可以直接用 *pt了
  int delayTime = pt->delayTime;

  pinMode(pin, OUTPUT);
  while (1) {
    digitalWrite(pin, !digitalRead(pin));
    vTaskDelay(delayTime);
  }
}

void setup() {

  LEDFlash led1, led2;  //对于结构体声明两个变量
  led1.pin = 23;        //赋值
  led1.delayTime = 1000;
  led2.pin = 21;
  led2.delayTime = 3000;

  //此时 两个线程可以直接用一个函数了
  xTaskCreate(
    blind,          //函数
    "blink 1",      //取名
    1024,           //盏分配 1024 2048...字节
    (void *)&led1,  //指针 函数传参
    1,              //任务等级
    NULL            //操作任务的指针
  );

  xTaskCreate(
    blind,          //函数
    "blink 2",      //取名
    1024,           //盏分配 1024 2048...字节
    (void *)&led2,  //指针 函数传参.  &LED2 取了地址才能强转指针类型
    1,              //任务等级
    NULL            //操作任务的指针
  );
}

void loop() {
  // put your main code here, to run repeatedly:
}
