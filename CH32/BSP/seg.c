#include "seg.h"


uint8_t password[10]={0};

void password_set(uint8_t* word,uint8_t lenth)
{
    for (int i = 0; i < lenth; ++i) {
        password[i]=word[i];
    }
}
void password_get(uint8_t* word)
{
    for (int i = 0; i < 10; ++i) {
        word [i]=password[i];
       }
}
uint8_t password_mate(uint8_t* word,uint8_t lenth)
{
    for (int i = 0; i < lenth; ++i) {
           if(password[i]!=word[i])return 0;
          }
    return 1;

}


