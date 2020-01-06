#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int command(char** argv, unsigned char* address_display, unsigned char* char_display, int index) {
  if (!strcasecmp(argv[index], "--address-display")) {
    *address_display = 0x01;
  } else if (!strcasecmp(argv[index], "--char-display")) {
    *char_display = 0x01;
  } else {
    return -1;
  }
  return 0;
}

void command_table() {
  printf("--------------command table----------------------------------\n");
  printf("|./jbyte [optional] filePath                                |\n");
  printf("|optional:                                                  |\n");
  printf("|         --address-display                                 |\n");
  printf("|         --char-display                                    |\n");
  printf("|         --word-number numberOfWord(can be adjust)         |\n");
  printf("-------------------------------------------------------------\n");
}

int main(int argc, char** argv)
{
  char* filePath = NULL;
  unsigned char address_display = 0x00;
  unsigned char char_display = 0x00;
  int byteOfLine = 0x00000010;
  if (argc == 2) {
    if (!strcasecmp(argv[0x00000001], "--help") || !strcasecmp(argv[0x00000001], "--h")) {
      command_table();
      return 0;
    }
    filePath = argv[1];
  } else if (argc == 3) {
    if (command(argv, &address_display, &char_display, 0x00000001)) {
      command_table();
      return 0;
    }
    filePath = argv[2];
  } else if (argc == 4) {
    if (command(argv, &address_display, &char_display, 0x00000001)) {
      if (!strcasecmp(argv[0x00000001], "--word-number") && (byteOfLine = atoi(argv[0x00000002]))) {
      } else {
        command_table();
        return 0;
      }
    }
    if (strcasecmp(argv[0x00000001], "--word-number") && command(argv, &address_display, &char_display, 0x00000002)) {
      command_table();
      return 0;
    }
    filePath = argv[3];
  } else if (argc == 5) {
    if (command(argv, &address_display, &char_display, 0x00000001)) {
      if (!strcasecmp(argv[0x00000001], "--word-number") && (byteOfLine = atoi(argv[0x00000002]))) {
      } else {
        command_table();
        return 0;
      }
    }
    if (!strcasecmp(argv[0x00000001], "--word-number")) {
      if (command(argv, &address_display, &char_display, 0x00000003)) {
        command_table();
        return 0;
      }
    } else {
      if (command(argv, &address_display, &char_display, 0x00000002)) {
        if (!strcasecmp(argv[0x00000002], "--word-number") && (byteOfLine = atoi(argv[0x00000003]))) {
        } else {
          command_table();
          return 0;
        }
      }
      if (strcasecmp(argv[0x00000002], "--word-number") && command(argv, &address_display, &char_display, 0x00000003)) {
        command_table();
        return 0;
      }
    }
    filePath = argv[4];
  } else if (argc == 6) {
    if (command(argv, &address_display, &char_display, 0x00000001)) {
      if (!strcasecmp(argv[0x00000001], "--word-number") && (byteOfLine = atoi(argv[0x00000002]))) {
      } else {
        command_table();
        return 0;
      }
    }
    if (!strcasecmp(argv[0x00000001], "--word-number")) {
      if (command(argv, &address_display, &char_display, 0x00000003)) {
        command_table();
        return 0;
      }
      if (command(argv, &address_display, &char_display, 0x00000004)) {
        command_table();
        return 0;
      }
    } else {
      if (command(argv, &address_display, &char_display, 0x00000002)) {
        if (!strcasecmp(argv[0x00000002], "--word-number") && (byteOfLine = atoi(argv[0x00000003]))) {
        } else {
          command_table();
          return 0;
        }
      }
      if (!strcasecmp(argv[0x00000002], "--word-number")) {
        if (command(argv, &address_display, &char_display, 0x00000004)) {
          command_table();
          return 0;
        }
      } else {
        if (command(argv, &address_display, &char_display, 0x00000003)) {
          if (!strcasecmp(argv[0x00000003], "--word-number") && (byteOfLine = atoi(argv[0x00000004]))) {
          } else {
            command_table();
            return 0;
          }
        }
      }
    }
    filePath = argv[5];
  } else {
    command_table();
    return 0;
  }
  if (byteOfLine % 0x00000010) {
    byteOfLine = ((int) (byteOfLine / 0x00000010) + 0x00000001) * 0x00000010;
  }
  FILE* pReadFile = fopen(filePath, "r");
  if (pReadFile != NULL) {
    int counter = 0x00000000;
    int address = 0x00000000;
    unsigned char byte = 0x00;
    unsigned char* content = NULL;
    if (char_display) {
      content = (unsigned char*) malloc((byteOfLine + 0x00000001) * sizeof(unsigned char));
    }
    while (fread(&byte, 0x00000001, 0x00000001, pReadFile)) {
      if (counter == byteOfLine) {
        counter = 0x00000000;
        if (address_display) {
          address = address + byteOfLine;
        }
        if (char_display) {
          content[byteOfLine] = 0x00;
          printf(" ");
          for (int index = 0x00000000; index < byteOfLine; index++) {
            printf("%c", content[index]);
            content[index] = 0x00;
          }
        }
        printf("\n");
      }
      if (counter == 0x00000000 && address_display) {
        printf("%08x: ", address);
      }
      if (char_display) {
        if (byte >= 0x21 && byte <= 0x7d) {
          content[counter] = byte;
        } else {
          content[counter] = '.';
        }
      }
      counter = counter + 0x00000001;
      printf("%02x", byte);
      if (counter > 0x00000000 && ((counter % 0x00000002) == 0x00000000)) {
        printf(" ");
      }
    }
    if (char_display) {
      for (int index = (byteOfLine << 1) + (byteOfLine >> 1) - 0x00000007; counter <= index; counter++) {
        printf(" ");
      }
      for (int index = 0x00000000; index < byteOfLine; index++) {
        printf("%c", content[index]);
      }
      free(content);
    }
    fclose(pReadFile);
    printf("\n");
  } else {
    command_table();
  }
  return 0;
}
