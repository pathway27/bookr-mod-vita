// TODO:
#if defined(__vita__) && defined(DEBUG)
  #include <stdio.h>
  #include <debugnet.h>
  #define ip_server "192.168.178.20"
  #define port_server 18194

  #define printf(...) debugNetPrintf(DEBUG, __VA_ARGS__)
#endif
