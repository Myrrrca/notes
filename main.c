#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

void printGreet()
{
  printf("Notes store\n");
}

void printOptions()
{
  printf("1 - create note   2 - edit note   3 - delete note\n");
}

void mkdirConfig(const char* configPath)
{
  char pathDir[256];
  strcpy(pathDir, configPath);
  size_t len = strlen(pathDir);
  char* c = pathDir + len - 1;
  while (*c != '/' && c != pathDir)
  {
    *c = '\0';
    --c;
  }
  /* printf("%s\n", pathDir); */
  mkdir(pathDir, 0777);
}

void mkdirNotes(const char* notesPath)
{
  char pathDir[256];
  strcpy(pathDir, notesPath);
  size_t len = strlen(pathDir);
  char* c = pathDir + len - 1;
  while (*c != '/' && c != pathDir)
  {
    *c = '\0';
    --c;
  }
  mkdir(pathDir, 0777);
}

void printNotesList(const char* configPath)
{
  FILE* getAmount = fopen(configPath, "rb");
  if (getAmount == NULL)
  {
    printf("**Notes storage is empty**\n");
    return;
  }
  uint16_t amount;

  fread(&amount, sizeof(uint16_t), 1, getAmount);
  if (amount == 0)
  {
    printf("**Notes storage is empty**\n");
  }
  else 
  {
    printf("Current amount of notes: %d\n", amount);
  }
  fclose(getAmount);
}

void createNote(char* noteName, const char* configPath, const char* notesPath)
{
  char fullPath[128];
  strcpy(fullPath, notesPath);
  strcat(fullPath, noteName);
  char* md = ".md";
  strcat(fullPath, md);
  /* printf("%s\n", fullPath); */

  FILE* newNote = fopen(fullPath, "w");
  printf("Enter content:\n");
  char content[2048];
  fgets(content, 2048, stdin);
  /* scanf("%*c"); */
  fputs(content, newNote);
  fclose(newNote);

  uint16_t amount = 1;

  FILE* config = fopen(configPath, "r+b");
  if (config == NULL)
  {
    FILE* create = fopen(configPath, "wb");
    if (create == NULL)
    {
      perror("Idk whats happening");
      return;
    }
    fwrite(&amount, sizeof(uint16_t), 1, create);
    fclose(create);
    return;
  }
  fread(&amount, sizeof(uint16_t), 1, config);
  ++amount;
  rewind(config);
  fwrite(&amount, sizeof(uint16_t), 1, config);
  fclose(config);

  /* FILE* */
}

int main(void)
{
  const char* configPath = "./.config/config.dat";
  const char* notesPath = "./notes/";
  /* int c; //for cleaning buffer */

  printGreet();
  printOptions(); 
  mkdirConfig(configPath);
  mkdirNotes(notesPath);
  printNotesList(configPath);
  
  char action[2];
  if (fgets(action, 2, stdin) == NULL)
  {
    printf("Error while reading\n");
  }
  *action -= 48;
  scanf("%*c"); // cleans '\n' from buffer after fgets()
  
  char noteName[64];
  switch (*action)
  {
    case 1:
      printf("Enter name of new note:\n");
      fgets(noteName, 64, stdin);
      /* while ((c = getchar()) != '\n' && c != EOF) { } */
      printf(noteName);
      noteName[strcspn(noteName, "\n")] = '\0'; //replacing '\n' with '\0' in noteName
      createNote(noteName, configPath, notesPath);
      break;
  }
  //printf("\033[2J");
  //printf("%d\n", *action);

  return 0;
}
