#include "tim6.h"
#include "led.h"

void Dri_Tim6_Callback(void)
{
    Int_LED_Toggle(LED1);
}



int main(void){
    Int_Led_Init();
    Dri_Tim6_Init();

    Dri_Tim6_Start();


    while(1){
    }

}
