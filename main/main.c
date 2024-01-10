#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#if CONFIG_FREERTOS_UNICORE 
    static const BaseType_t app_cpu = 0; 
#else 
    static const BaseType_t app_cpu = 1; 
#endif

#define arrSize 100


void dTask(void *arg){
    int k = 0; 
    while (1){
        int w = 6; 
        int x[arrSize]; 

        char *taskName = pcTaskGetName(NULL); 

        for (size_t i = 0; i < arrSize ; i++){
            x[i] = w + i; 
        }
        ESP_LOGI(taskName, "x at %i: %i\n", k, x[k]); 
        if (99 > k){ 
            k++;
        } else {
            k = 0; 
        }

        //looking at stack and heap memory 
        ESP_LOGW(taskName, "Remaining mem in task stack (words): %d", uxTaskGetStackHighWaterMark(NULL)); 

        //this block is being used to check what happens when memory isnt freed up and you continue to call request more 
        ESP_LOGE(taskName, "Remaining mem in heap pre-malloc call (bytes): %d", xPortGetFreeHeapSize()); 
        int *randPtr = (int*)pvPortMalloc(1024*sizeof(int)); 
        //If not enough heap, do nothing, else ...
        if (NULL == randPtr){
            ESP_LOGE("FAIL", "Not enough memory available!"); 
        } else {
            //do stuff with pointer to make it sure it uses up heap
            for (size_t i = 0; i < 1024; i++){
                randPtr[i] = k*k;  
            }
        }
        ESP_LOGE(taskName, "Remaining mem in heap post-malloc call (bytes): %d", xPortGetFreeHeapSize()); 
        printf("\n"); 

        vPortFree(randPtr);                                                                 //Frees up the allocated memory 

        vTaskDelay(100/portTICK_PERIOD_MS);                                                 //this delay included to avoid watchdog error caused by this task not giving way to IDLE takss to do required background work
                                                                                            //this can also be solved by setting task priority to 0 or tskIDLE_PRIORITY, which means scheduler will preemppt these tasks
         
    }   
}

void setup(){
    vTaskDelay(1000/portTICK_PERIOD_MS); 
    printf("\n"); 
    char *taskName = pcTaskGetName(NULL); 
    ESP_LOGI(taskName, "Task Demo - Memory Management ... \n"); 
    xTaskCreatePinnedToCore(dTask, "Demo Task", 2300, NULL, 1, NULL, app_cpu); 
    vTaskDelete(NULL); 
}

void app_main(void){
    setup(); 
    vTaskDelete(NULL); 
}