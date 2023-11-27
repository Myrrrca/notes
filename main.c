#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "./lib/tomlc99/toml.h"

void copyNotes(const char *sourceDir, const char *destinationDir) 
{
  DIR *source = opendir(sourceDir);
  DIR *destination = opendir(destinationDir);

  if (!source || !destination) 
  {
    perror("Error opening directories");
    exit(EXIT_FAILURE);
  }

  struct dirent *entry;

  while ((entry = readdir(source)) != NULL)
  {
    if (entry->d_type == DT_REG) 
    {
      char sourcePath[PATH_MAX];
      char destinationPath[PATH_MAX];
      snprintf(sourcePath, sizeof(sourcePath), "%s/%s", sourceDir, entry->d_name);
      snprintf(destinationPath, sizeof(destinationPath), "%s/%s", destinationDir, entry->d_name);

      FILE *sourceFile = fopen(sourcePath, "rb");
      FILE *destinationFile = fopen(destinationPath, "wb");

      if (!sourceFile || !destinationFile) 
      {
        perror("Error opening files");
        exit(EXIT_FAILURE);
      }

      char buffer[4096];
      size_t bytesRead;

      while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) 
      {
        fwrite(buffer, 1, bytesRead, destinationFile);
      }

      fclose(sourceFile);
      fclose(destinationFile);
    }
  }
    closedir(source);
    closedir(destination);
}

void rmAllNotesInDir(const char* dirPath)
{
  DIR *dir = opendir(dirPath);

  if (dir == NULL) 
  {
    exit(EXIT_FAILURE);
  }

  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) 
  {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
    {
      char notePath[256];
      snprintf(notePath, sizeof(notePath), "%s/%s", dirPath, entry->d_name);

      if (remove(notePath) != 0) 
      {
        exit(EXIT_FAILURE);
      }

    }
  }

  closedir(dir);

}

void rmDir(const char* dirPath)
{
  rmdir(dirPath);
}

int checkNotes(const char* configPath, char* _location)
{
  FILE* config = fopen(configPath, "r");
  if (config != NULL)
  {
    fseek(config, 37, SEEK_SET); 
    char location[1024];
    fread(location, 1, 1024, config);
    char* slash = strrchr(location, '/');
    *++slash = '\0';
    strcpy(_location, location);
    return 0;
  }
  return 1;
}

int createNotesAndConfig(const char* configPath, const char* HOME, char* notesPathBuff)
{
  char customPath[1024];
  printf("Enter path for your notes directory:\n");
  printf("(you can leave input empty and press \"Enter\" for creating \"note\" directory at %s.local/share/mynotes/)\n", HOME);
  printf("%s", HOME);
  fgets(customPath, sizeof(customPath), stdin);
  char notesPath[1024];
  strcpy(notesPath, HOME);

  if (customPath[0] == '\n')
  {
    strcat(notesPath, ".local/share/mynotes/");
    mkdir(notesPath, 0777);
  }
  else 
  {
    strcat(notesPath, customPath);
    notesPath[strcspn(notesPath, "\n")] = '\0'; //replacing '\n' with '\0' in notesPath
    strcat(notesPath, "/");

    if (strcmp(notesPathBuff, notesPath)== 0)
    {
      printf("Your notes directory already created here!\nPress Enter...\n");
      scanf("%*c");
      return 0;
    }

    if (mkdir(notesPath, 0777) == -1)
    {
      if (errno != EEXIST)
      {
        perror("Error while creating directory");
        exit(1);
      }
    }
  }

  FILE* checkConfig = fopen(configPath, "r");
  if (checkConfig != NULL && strcmp(notesPathBuff, notesPath))
  {

    printf("y - copy notes from previous directory\nyd - copy notes and delete previous directory\nc - cut notes from previous directory\nENTER - leave notes directory with all the notes as it was\n");
    char action[3];
    if (fgets(action, 3, stdin) == NULL)
    {
      printf("Error while reading\nPress Enter...\n");
    }

    if (action[0] == 'y' && action[1] == '\n')
    {
      copyNotes(notesPathBuff, notesPath);
      printf("copied");
      scanf("%*c");
    }
    else if (action[0] == 'y' && action[1] == 'd')
    {
      copyNotes(notesPathBuff, notesPath);
      rmAllNotesInDir(notesPathBuff);
      rmDir(notesPathBuff);
      printf("copied and deleted");
      scanf("%*c");
    }
    else if (action[0] == 'c' && action[1] == '\n')
    {
      copyNotes(notesPathBuff, notesPath);
      rmAllNotesInDir(notesPathBuff);
      printf("cutted");
      scanf("%*c");
    }
  }

  FILE* createConfig = fopen(configPath, "w");
  /* char configText[] = "# provides config location\nlocation = "; */
  char configText[] = "# provides config location\nlocation = "; // size = 39!!!!! (works only 37)
  /* uint16_t size = sizeof(configText); */
  /* printf("%d", size); */
  fwrite(configText, sizeof(configText[0]), strlen(configText) - 1, createConfig);
  fwrite(notesPath, sizeof(notesPath[0]), strlen(notesPath), createConfig);
  fclose(createConfig);
  strcpy(notesPathBuff, notesPath);
  printf("\033[2J\033[H");
  return;
}

void printGreet()
{
  printf("Notes Store\n");
}

void printOptions()
{
  printf("c - create   o - open   e - edit   d - delete   r - refresh   n - change notes directory   v - view location   q - exit\n"); }

void mkdirNotes(const char* notesPath)
{
  mkdir(notesPath, 0777);
}

void showNotes(const char* notesPath)
{
  uint16_t filesCount;
  DIR* dir;
  struct dirent* entry;
  char* noteName;

  dir = opendir(notesPath);
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {
      noteName = entry->d_name; 
      noteName[strcspn(noteName, ".")] = '\0'; // removing .md 
      printf("--- %s\n", noteName);
      ++filesCount;
    }
  }
  if (filesCount == 0)
    printf("**Notes storage is empty**\n");
  closedir(dir);
}

void createNote(char* noteName, const char* notesPath)
{
  char fullPath[128];
  strcpy(fullPath, notesPath);
  strcat(fullPath, noteName);
  char* md = ".md";
  strcat(fullPath, md);

  FILE* checkNote = fopen(fullPath, "r");
  if (checkNote != NULL)
  {
    printf("Note \"%s\" already exist!\nPress Enter...\n", noteName);
    scanf("%*c");
    printf("\033[2J\033[H");
    fclose(checkNote);
    return;
  }

  FILE* newNote = fopen(fullPath, "w");
  printf("Enter content:\n");
  char content[4096];
  fgets(content, 4096, stdin);
  fputs(content, newNote);
  printf("Note \"%s\" sucsessfully created\nPress Enter...\n", noteName);
  scanf("%*c");
  printf("\033[2J\033[H");
  fclose(newNote);
  return;
}

void openNote(char* noteName, const char* notesPath)
{
  char fullPath[128];
  strcpy(fullPath, notesPath);
  strcat(fullPath, noteName);
  char* md = ".md";
  strcat(fullPath, md);
  char content[4096];

  FILE* readNote = fopen(fullPath, "r");
  if (readNote != NULL)
  {
    printf("Content of note \"%s\": \n", noteName);
    fread(content, 1, 4096, readNote);
    printf("%s", content);
    printf("Press Enter...\n");
    scanf("%*c");
    printf("\033[2J\033[H");
    return;
  }
  printf("Note \"%s\" does not exist!\nPress Enter...\n", noteName);
  scanf("%*c");
  printf("\033[2J\033[H");
  return;

}

void deleteNote(char* noteName, const char* notesPath)
{
  char fullPath[128];
  strcpy(fullPath, notesPath);
  strcat(fullPath, noteName);
  char* md = ".md";
  strcat(fullPath, md);

  FILE* checkNote = fopen(fullPath, "r");
  if (checkNote != NULL)
  {
    remove(fullPath);
    printf("Note \"%s\" sucsessfully deleted\nPress Enter...\n", noteName);
    scanf("%*c");
    printf("\033[2J\033[H");
    return;
  }
  printf("Note \"%s\" does not exist!\nPress Enter...\n", noteName);
  scanf("%*c");
  printf("\033[2J\033[H");
  return;
}

void editNote(char* noteName, const char* notesPath)
{
  char fullPath[128];
  strcpy(fullPath, notesPath);
  strcat(fullPath, noteName);
  char* md = ".md";
  strcat(fullPath, md);
  char content[4096];

  FILE* checkNote = fopen(fullPath, "r");
  if (checkNote != NULL)
  {
    FILE* newNote = fopen(fullPath, "w");
    printf("Enter new content for note \"%s\" (all previous content will be deleted): \n", noteName);
    fgets(content, 4096, stdin);
    fputs(content, newNote);
    printf("Content in note \"%s\" sucsessfully edited\nPress Enter...\n", noteName);
    scanf("%*c");
    printf("\033[2J\033[H");
    fclose(newNote);
    fclose(checkNote);
    return;
  }
  printf("Note \"%s\" does not exist!\nPress Enter...\n", noteName);
  scanf("%*c");
  printf("\033[2J\033[H");
  return;
}

int main(void)
{
  printf("\033[2J\033[H");
  char isExit = 1;

  char* HOME = getenv("HOME");
  strcat(HOME, "/");
  char configPath[256];
  strcpy(configPath, HOME);
  strcat(configPath, ".config/notes-supa-config/");
  if (mkdir(configPath, 0777) == -1)
  {
    perror("error");
  }
  char* configName = "config.toml";
  strcat(configPath, configName);
  char notesPath[1024];
  if (checkNotes(configPath, notesPath))
  {
    createNotesAndConfig(configPath, HOME, notesPath);
  }

  while (isExit)
  {
    printGreet();
    showNotes(notesPath);
    printOptions(); 

    char action[2];
    if (fgets(action, 2, stdin) == NULL)
    {
      printf("Error while reading\nPress Enter...\n");
      break;
    }

    char noteName[64];
    if (action[0] == 'c')
    {
      printf("Enter name of new note:\n");
      int c;
      while ((c = getchar()) != '\n' && c != EOF) { }
      fgets(noteName, 64, stdin);
      noteName[strcspn(noteName, "\n")] = '\0'; //replacing '\n' with '\0' in noteName
      createNote(noteName, notesPath);
    }
    else if (action[0] == 'o')
    {
      printf("Enter name of note that you want to open:\n");
      int c;
      while ((c = getchar()) != '\n' && c != EOF) { }
      fgets(noteName, 64, stdin);
      noteName[strcspn(noteName, "\n")] = '\0'; //replacing '\n' with '\0' in noteName
      openNote(noteName, notesPath);
    }
    else if (action[0] == 'e')    
    {
      printf("Enter name of note that you want to edit:\n");
      int c;
      while ((c = getchar()) != '\n' && c != EOF) { }
      fgets(noteName, 64, stdin);
      noteName[strcspn(noteName, "\n")] = '\0'; //replacing '\n' with '\0' in noteName
      editNote(noteName, notesPath);
    }
    else if (action[0] == 'd')
    {
      printf("Enter name of note that you want to delete:\n");
      int c;
      while ((c = getchar()) != '\n' && c != EOF) { }
      fgets(noteName, 64, stdin);
      noteName[strcspn(noteName, "\n")] = '\0'; //replacing '\n' with '\0' in noteName
      deleteNote(noteName, notesPath);      
    }
    else if (action[0] == 'r')
    {
      printf("\033[2J\033[H");
      printGreet();
      showNotes(notesPath);
      printOptions(); 
    }

    else if (action[0] == 'n')
    {
      int c;
      while ((c = getchar()) != '\n' && c != EOF) { }
      if (createNotesAndConfig(configPath, HOME, notesPath))
      {
        printf("Path for your notes sucsessfully changed to \"%s\"\nPress Enter...\n", notesPath);
        scanf("%*c");
      }
      printf("\033[2J\033[H");
    }
    else if (action[0] == 'v')
    {
      printf("\033[2J\033[H");
      int c;
      while ((c = getchar()) != '\n' && c != EOF) { }   
      printf("LOCATION OF YOUR NOTES DIRECTORY: %s\nPress Enter...\n", notesPath);  
      scanf("%*c");
      printf("\033[2J\033[H");
    }
    else if (action[0] == 'q')
    {
      printf("\033[2J\033[H");
      isExit = 0;
      break;
    }
    else 
    {
      printf("\033[2J\033[H");  
    }
  }

  return 0;
}
