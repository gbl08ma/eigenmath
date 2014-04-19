#include <fxcg/display.h>
#include <fxcg/file.h>
#include <fxcg/keyboard.h>
#include <fxcg/system.h>
#include <fxcg/misc.h>
#include <fxcg/app.h>
#include <fxcg/serial.h>
#include <fxcg/rtc.h>
#include <fxcg/heap.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "graphicsProvider.hpp"
#include "menuGUI.hpp"
#include "fileProvider.hpp"
#include "stringsProvider.hpp"

int compareFileStructs(File* f1, File* f2, int type) {
  if(f1->isfolder < f2->isfolder) return 1;
  else if(f1->isfolder > f2->isfolder) return -1;
  switch(type) {
    case 1:
      return strcmp( f1->filename, f2->filename );
    case 2:
      return -strcmp( f1->filename, f2->filename );
    case 3:
      return f1->size-f2->size;
    case 4:
    default:
      return f2->size-f1->size;
  }
}

void insertSortFileMenuArray(File* data, MenuItem* mdata, int size) {
  int sort = 1;//GetSetting(SETTING_FILE_MANAGER_SORT);
  if(!sort) return;
  int i, j;
  File temp;
  MenuItem mtemp;

  for(i = 1; i < size; i++) {
    temp = data[i];
    mtemp = mdata[i];
    for (j = i - 1; j >= 0 && compareFileStructs(&data[j], &temp, sort) > 0; j--) {
      data[j + 1] = data[j];
      mdata[j + 1] = mdata[j];
    }
    data[j + 1] = temp;
    mdata[j + 1] = mtemp;
  }
  // update menu text pointers (these are still pointing to the old text locations):
  for(i = 0; i < size; i++) mdata[i].text = data[i].visname;
}

int GetFiles(File* files, MenuItem* menuitems, char* basepath, int* count, unsigned char* filter) {
  // searches storage memory for folders and files, puts their count in int* count
  // if File* files is NULL, function will only count files. If it is not null, MenuItem* menuitems will also be updated
  // this function always returns status codes defined on fileProvider.hpp
  // basepath should start with \\fls0\ and should always have a slash (\) at the end
  // filter is the filter for the files to list
  unsigned short path[MAX_FILENAME_SIZE+1], found[MAX_FILENAME_SIZE+1];
  unsigned char buffer[MAX_FILENAME_SIZE+1];

  // make the buffer
  strcpy((char*)buffer, basepath);
  strcat((char*)buffer, "*");
  
  *count = 0;
  file_type_t fileinfo;
  int findhandle;
  Bfile_StrToName_ncpy(path, buffer, MAX_FILENAME_SIZE+1);
  int ret = Bfile_FindFirst_NON_SMEM((const char*)path, &findhandle, (char*)found, &fileinfo);
  Bfile_StrToName_ncpy(path, filter, MAX_FILENAME_SIZE+1);
  while(!ret) {
    Bfile_NameToStr_ncpy(buffer, found, MAX_FILENAME_SIZE+1);
    if(!(strcmp((char*)buffer, "..") == 0 || strcmp((char*)buffer, ".") == 0 || strcmp((char*)buffer, "@MainMem") == 0)
      && (fileinfo.fsize == 0 || Bfile_Name_MatchMask((const short int*)path, (const short int*)found)))
    {
      if(files != NULL) {
        strncpy(files[*count].visname, (char*)buffer, 40);
        strcpy(files[*count].filename, basepath); 
        strcat(files[*count].filename, (char*)buffer);
        files[*count].size = fileinfo.fsize;
        files[*count].isfolder = menuitems[*count].isfolder = !fileinfo.fsize;
        if(fileinfo.fsize == 0) menuitems[*count].icon = FILE_ICON_FOLDER; // it would be a folder icon anyway, because isfolder is true
        else menuitems[*count].icon = fileIconFromName((char*)buffer);
        menuitems[*count].isselected = 0; //clear selection. this means selection is cleared when changing directory (doesn't happen with native file manager)
        // because usually alloca is used to declare space for MenuItem*, the space is not cleared. which means we need to explicitly set each field:
        menuitems[*count].text = files[*count].visname;
        menuitems[*count].color=TEXT_COLOR_BLACK;
        menuitems[*count].type=MENUITEM_NORMAL;
        menuitems[*count].value=MENUITEM_VALUE_NONE;
      }
      *count=*count+1;
    }
    if (*count-1==MAX_ITEMS_IN_DIR) {
      Bfile_FindClose(findhandle);
      if(files != NULL && menuitems != NULL) insertSortFileMenuArray(files, menuitems, *count);
      return GETFILES_MAX_FILES_REACHED; // Don't find more files, the array is full. 
    } else ret = Bfile_FindNext_NON_SMEM(findhandle, (char*)found, (char*)&fileinfo);
  }
  Bfile_FindClose(findhandle);
  if(*count > 1 && files != NULL && menuitems != NULL) insertSortFileMenuArray(files, menuitems, *count);
  return GETFILES_SUCCESS;
}

void nameFromFilename(char* filename, char* name) {
  //this function takes a full filename like \\fls0\Folder\file.123
  //and puts file.123 in name.
  strcpy(name, (char*)"");
  int i=strlen(filename)-1;
  while (i>=0 && filename[i] != '\\')
          i--;
  if (filename[i] == '\\') {
    strcpy(name, filename+i+1);
  }
}

int fileIconFromName(char* name) {
  if(EndsIWith(name, (char*)".g1m") || EndsIWith(name, (char*)".g2m") || EndsIWith(name, (char*)".g3m"))
    return FILE_ICON_G3M;
  else if (EndsIWith(name, (char*)".g1e") || EndsIWith(name, (char*)".g2e") || EndsIWith(name, (char*)".g3e"))
    return FILE_ICON_G3E;
  else if (EndsIWith(name, (char*)".g3a") || EndsIWith(name, (char*)".g3l"))
    return FILE_ICON_G3A;
  else if (EndsIWith(name, (char*)".g3p"))
    return FILE_ICON_G3P;
  else if (EndsIWith(name, (char*)".g3b"))
    return FILE_ICON_G3B;
  else if (EndsIWith(name, (char*)".bmp"))
    return FILE_ICON_BMP;
  else if (EndsIWith(name, (char*)".txt"))
    return FILE_ICON_TXT;
  else if (EndsIWith(name, (char*)".csv"))
    return FILE_ICON_CSV;
  else return FILE_ICON_OTHER;
}