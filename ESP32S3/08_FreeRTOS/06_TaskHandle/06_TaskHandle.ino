/*
最后一个参数是 任务句柄
句柄可以用来管理任务，例如删除任务、获取任务状态等
如果不需要那么就是NULL

删除线程和删除任务 其实是一回事
*/

/*
    BaseType_t xTaskCreate(,,,,,); //任务创建
    void vTaskDelete( TaskHandle_t xTask ); //任务删除 
    void vTaskSuspend( TaskHandle_t xTaskToSuspend ); //任务暂停
    void vTaskResume( TaskHandle_t xTaskToResume ); //任务恢复
*/


#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

TaskHandle_t biliHandle = NULL; //创建任务句柄

void control(void *ptParam) {  //按钮控制
  
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);

  while (1) {
    // 创建任务
    if (digitalRead(32) == LOW) {
      //判断是否之前已经创建了Bilibili channel task， 如果没有创建，则创建该Task
      if (biliHandle == NULL) {
        if (xTaskCreate(radioBilibili, "Bilibili Channel", 1024 * 8, NULL, 1, &biliHandle) == pdPASS) { //== pdPASS 检查是否创建成功
          Serial.print(xTaskGetTickCount()); //滴答计数 1ms
          Serial.println(" - LOG: Task is Created.");
        } else {//如果任务没有创建成功
          Serial.print(xTaskGetTickCount());
          Serial.println(" - WARNING: Unable to Create Task.");
        }
      } else {//如果事件不为NULL
        Serial.print(xTaskGetTickCount());
        Serial.println(" - WARNING: Task **WAS** Created.");
      }
      vTaskDelay(120); //粗暴的Button Debounce
    }

    //任务删除
    if (digitalRead(33) == LOW) {
      //注意在删除任务前，一定要确保任务是存在的
      //删除不存在的任务，比如连续删除两次，自动重启
      if (biliHandle != NULL) {
        vTaskDelete( biliHandle );
        lcdClear(); //清空LCD
        biliHandle = NULL; //手动将handler设置为空
      }

      if (biliHandle != NULL) {
        Serial.print(xTaskGetTickCount());
        Serial.println(" - WARNING: Unable to Delete Task.");
      } else {
        Serial.print(xTaskGetTickCount());
        Serial.println(" - LOG: Task is Deleted.");
      }
      vTaskDelay(120);  //粗暴的Button Debounce
    }

    // 任务暂停
    if (digitalRead(25) == LOW) {
      if (biliHandle != NULL) {
        vTaskSuspend(biliHandle);
        Serial.print(xTaskGetTickCount());
        Serial.println(" - LOG: Task is suspended.");
      } else {
        Serial.print(xTaskGetTickCount());
        Serial.println(" - WARNING: Unable to Suspend Task.");
      }
      vTaskDelay(120);  //粗暴的Button Debounce
    }

    // 任务恢复
    if (digitalRead(26) == LOW) { 
      if (biliHandle != NULL) {
        vTaskResume(biliHandle);
        Serial.print(xTaskGetTickCount());
        Serial.println(" - LOG: Task is resumed.");
      } else {
        Serial.print(xTaskGetTickCount());
        Serial.println(" - WARNING: Unable to Resume Task.");
      }
      vTaskDelay(120);  //粗暴的Button Debounce
    }
  }
}

void radioBilibili(void *ptParam) {  //任务主体

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Bilibili   Channel ");
  lcd.setCursor(0, 1);
  lcd.print("-FreeRTOS on EPS32- ");
  lcd.setCursor(0, 2);
  lcd.print("Study Hard  &  Smart");

  while (1) {
    lcd.setCursor(9, 3);
    lcd.print(xTaskGetTickCount() / 100);
    vTaskDelay(100);
  }
}

void lcdClear() { //清空LCD
  lcd.clear();
  lcd.noBacklight();
  lcd.setCursor(0, 0);
  lcd.print("                                                                                                    ");
}

void setup()
{
  Serial.begin(115200);
  xTaskCreate(control, "control panel", 1024 * 8, NULL, 1, NULL);
}


void loop() {}