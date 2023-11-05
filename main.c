#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

void printGreet()
{
  printf("Notes store\n");
}

void printOptions()
{
  printf("1 - create note   2 - edit note   3 - delete note\n");
}

void mkdirNotes(const char* notesPath)
{
  mkdir(notesPath, 0777);
}

void refreshNotes(const char* notesPath)
{
  uint16_t filesCount;
  DIR* dir;
  struct dirent* entry;

  dir = opendir(notesPath);
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {
      ++filesCount;
    }
  }
  closedir(dir);
  printf("%d\n", filesCount);
}

int checkExistingNote(char* noteName, const char* notesPath)
{
  char fullPath[128];
  strcpy(fullPath, notesPath);
  strcat(fullPath, noteName);
  char* md = ".md";
  strcat(fullPath, md);

  FILE* checkNote = fopen(fullPath, "r");
  if (checkNote != NULL)
  {
    printf("Note \"%s\" already exist!\n", noteName);
    return 0;
  }
  return 1;
}

void createNote(char* noteName, const char* notesPath)
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
}

int main(void)
{
  const char* notesPath = "./notes/";

  printGreet();
  printOptions(); 
  mkdirNotes(notesPath);
  refreshNotes(notesPath);
  /* printNotesList(configPath); */
  
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
      if (checkExistingNote(noteName, notesPath))
      {
      createNote(noteName, notesPath);
      }
      break;
  }
  //printf("\033[2J");
  //printf("%d\n", *action);

  return 0;
}
