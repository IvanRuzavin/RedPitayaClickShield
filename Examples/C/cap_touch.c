#include <stdio.h>
#include <stdlib.h>

#include "rp.h"

#define MIKROBUS 1

#if MIKROBUS == 1
    #define INT_PIN RP_DIO2_P
#else
    #define INT_PIN RP_DIO4_P
#endif


int main (int argc, char **argv) {
  rp_pinState_t state;

  if (rp_Init() != RP_OK) {
      fprintf(stderr, "Red Pitaya API init failed!\n");
      return EXIT_FAILURE;
  }

  rp_DpinSetDirection (INT_PIN, RP_IN);

  while(1){
    rp_DpinGetState(INT_PIN, &state);
    if (state == RP_HIGH){
      rp_DpinSetState(RP_LED0, state);
    }
    else{
        rp_DpinSetState(RP_LED0, state);
        printf("TOUCH DETECTED!!!\n");
    }


  }

  rp_Release();

  return EXIT_SUCCESS;
}