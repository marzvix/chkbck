#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  FILE *stream;
  char buffer[1024];
  char *dlg = "dialog";
  char *param = "--inputbox teste 0 0 dff";
  int y, x;

  sprintf(buffer, "%s %s", dlg, param);

  stream = popen(buffer, "w");
  
  return pclose(stream);
}
