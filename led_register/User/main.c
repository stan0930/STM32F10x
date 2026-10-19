
#include "stdint.h"

int main(){
	
	*(uint32_t *)(0x40021000 + 0x18) = 4;
	
	*(uint32_t *)(0x40010800 + 0x00) = 0x33;
	*(uint32_t *)(0x40010800 + 0x04) = 0x3;
	
	
	
	*(uint32_t *)((0x40010800 + 0x0c)) = 0xfefc;
	
	while(1){
	
	}
}
