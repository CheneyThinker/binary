#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void command_table()
{
  printf("Usage: ./byte [options] [in-file out-file]\n");
  printf("Options:\n");
  printf("  --h, --help                     Print this message and exit.\n");
  printf("  --a, --address-display          Print address offset.\n");
  printf("  --c, --char-display             Print ascii char text.\n");
  printf("  --p, --print                    Print on console\n");
  printf("  --b, --byte-of-line decimal     2^(4 + n) n = 0, 1, 2, ...\n");
  printf("  --s, --start-address dword hex  for example 0x00000000\n");
  printf("  --e, --end-address dword hex    for example 0x00000010\n");
  printf("\n");
  printf("author: CheneyThinker\n");
  printf("e-mail: clxuanye@163.com\n");
}

int command_not_value(int pos, char** argv, unsigned char* addressDisplay, unsigned char* charDisplay, unsigned char* print)
{
  char* cmd = argv[pos];
  if (!strcasecmp(cmd, "--help") || !strcasecmp(cmd, "--h"))
  {
    command_table();
  }
  else if (!strcasecmp(cmd, "--address-display") || !strcasecmp(cmd, "--a"))
  {
    *addressDisplay = 0x01;
  }
  else if (!strcasecmp(cmd, "--char-display") || !strcasecmp(cmd, "--c"))
  {
    *charDisplay = 0x01;
  }
  else if (!strcasecmp(cmd, "--print") || !strcasecmp(cmd, "--p"))
  {
    *print = 0x01;
  }
  else
  {
    return 1;
  }
  return 0;
}

int command_has_value(int pos, char** argv, int* byteOfLine, unsigned int* startAddress, unsigned int* endAddress)
{
  char* ptr = NULL;
  char* cmd = argv[pos];
  char* value = argv[pos + 1];
  if (!strcasecmp(cmd, "--byte-of-line") || !strcasecmp(cmd, "--b"))
  {
    if (!((*byteOfLine = (unsigned int) strtol(value, &ptr, 10)) || *ptr == 0x00))
    {
      return 2;
    }
  }
  else if (!strcasecmp(cmd, "--start-address") || !strcasecmp(cmd, "--s"))
  {
    if (!((*startAddress = (unsigned int) strtol(value, &ptr, 16)) || *ptr == 0x00))
    {
      return 2;
    }
  }
  else if (!strcasecmp(cmd, "--end-address") || !strcasecmp(cmd, "--e"))
  {
    if (!((*endAddress = (unsigned int) strtol(value, &ptr, 16)) || *ptr == 0x00))
    {
      return 2;
    }
  }
  else
  {
    return 1;
  }
  return 0;
}

int not_skip(int* pos, int stop, int* findNotValue, int* findHasValue, char** argv, unsigned char* addressDisplay, unsigned char* charDisplay, unsigned char* print, int* byteOfLine, unsigned int* startAddress, unsigned int* endAddress)
{
  *findNotValue = command_not_value(*pos, argv, addressDisplay, charDisplay, print);
  *findHasValue = command_has_value(*pos, argv, byteOfLine, startAddress, endAddress);
  if (*findNotValue && *findHasValue)
  {
    if (*findHasValue == 2)
    {
      printf("it's error, becuse of %s options value!\n", argv[*pos]);
    }
    else
    {
      printf("%s options not found\n", argv[*pos]);
    }
    command_table();
    return 1;
  }
  if (*pos == stop)
  {
    return 0;
  }
  if (*findHasValue)
  {
    *pos = (*pos) + 1;
  }
  if (*findNotValue)
  {
    *pos = (*pos) + 2;
  }
  if (*findNotValue || *findHasValue)
  {
    if (*pos <= stop)
    {
      return not_skip(pos, stop, findNotValue, findHasValue, argv, addressDisplay, charDisplay, print, byteOfLine, startAddress, endAddress);
    }
  }
  return 0;
}

void operator(char* infile, char* outfile, int byteOfLine, unsigned char addressDisplay, unsigned char charDisplay, unsigned char print, unsigned int startAddress, unsigned int endAddress)
{
  FILE* pfread = fopen(infile, "rb");
  FILE* pfwrite = fopen(outfile, "w+");
  if (pfread != NULL && pfwrite != NULL)
  {
    if (byteOfLine % 0x00000010)
    {
      byteOfLine = ((int) (byteOfLine / 0x00000010) + 0x00000001) * 0x00000010;
    }
    unsigned char byte = 0x00;
    unsigned char ascii[byteOfLine];
    unsigned int address = 0x00000000;
    fseek(pfread, startAddress, SEEK_SET);
    while (fread(&byte, 1, sizeof(unsigned char), pfread))
    {
      if (address % byteOfLine || (charDisplay && address))
      {
        fprintf(pfwrite, " ");
        if (print)
        {
          printf(" ");
        }
      }
      if (!(address % byteOfLine))
      {
        if (address)
        {
          if (charDisplay)
          {
            for (unsigned int index = 0x00000000; index < byteOfLine; index = index + 0x00000001)
            {
              if (ascii[index] < 0x21 || ascii[index] > 0x7e)
              {
                ascii[index] = '.';
              }
              if (print)
              {
                printf("%c", ascii[index]);
              }
              fprintf(pfwrite, "%c", ascii[index]);
            }
          }
          if (print)
          {
            printf("\n");
          }
          fprintf(pfwrite, "\n");
        }
        if (addressDisplay && !(endAddress && (startAddress == endAddress)))
        {
          fprintf(pfwrite, "%08x: ", startAddress);
          if (print)
          {
            printf("%08x: ", startAddress);
          }
        }
      }
      if (endAddress && (startAddress == endAddress))
      {
        break;
      }
      ascii[address % byteOfLine] = byte;
      fprintf(pfwrite, "%02x", byte);
      if (print)
      {
        printf("%02x", byte);
      }
      address = address + 0x0000001;
      startAddress = startAddress + 0x0000001;
      byte = 0x00;
    }
    if (charDisplay)
    {
      unsigned int offset = address % byteOfLine;
      if (offset)
      {
        unsigned int remain = endAddress && (startAddress == endAddress) ? (byteOfLine - offset) * 3 : (byteOfLine - offset) * 3 + 1;
        while (remain--)
        {
          fprintf(pfwrite, " ");
          if (print)
          {
            printf(" ");
          }
        }
        for (unsigned int index = 0x00000000; index < offset; index = index + 0x00000001)
        {
          if (ascii[index] < 0x21 || ascii[index] > 0x7e)
          {
            ascii[index] = '.';
          }
          fprintf(pfwrite, "%c", ascii[index]);
          if (print)
          {
            printf("%c", ascii[index]);
          }
        }
      }
    }
    if (print && (address % byteOfLine))
    { 
      printf("\n");
    }
  }
  fclose(pfread);
  fclose(pfwrite);
}

int main(int argc, char** argv)
{
  if (argc == 1)
  {
    command_table();
    return 0;
  }
  if (argc == 2)
  {
    if (!strcasecmp(argv[1], "--help") || !strcasecmp(argv[1], "--h"))
    {
      command_table();
    }
    else
    {
      printf("%s options not found\n", argv[1]);
      command_table();
    }
    return 0;
  }
  int byteOfLine = 0x00000010;
  unsigned char addressDisplay = 0x00;
  unsigned char charDisplay = 0x00;
  unsigned char print = 0x00;
  unsigned int startAddress = 0x00000000;
  unsigned int endAddress = 0x00000000;
  if (argc == 3)
  {
    operator(argv[1], argv[2], byteOfLine, addressDisplay, charDisplay, print, startAddress, endAddress);
    return 0;
  }
  int findNotValue = 0x00000000;
  int findHasValue = 0x00000000;
  int pos = 1;
  if (!not_skip(&pos, argc - 3, &findNotValue, &findHasValue, argv, &addressDisplay, &charDisplay, &print, &byteOfLine, &startAddress, &endAddress))
  {
    operator(argv[argc - 2], argv[argc - 1], byteOfLine, addressDisplay, charDisplay, print, startAddress, endAddress);
  }
  return 0;
}
