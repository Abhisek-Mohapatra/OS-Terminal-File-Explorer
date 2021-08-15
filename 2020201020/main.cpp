

/****************************************************************************************************************************************
 *                                            OS Assignment - 1  ( FILE EXPLORER )                                                       *                                   *
 *                                                                                                                                       *
 *                                                                                                                                       *
 *                                                                                                                                       *
 *                      Name :  Abhisek  Mohapatra                                       Roll Number : 2020201020                        *                                                           *
 *                                                                                                                                       *
 *                                                                                                                                       *
 *                                            Last Update :  Oct 12 8:14 pm                                                              *
 *                                                                                                                                       *                                                                                                                                                                                                                                   *
 * ****************************************************************************************************************************************/

#include <stdio.h>
#include <iomanip>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <bits/stdc++.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <fstream>
#include <filesystem>

using namespace std;

int last_row = 0;       // 1 variable is used to keep track of end row of display matrix
char parentArray[2000]; // Parent Array is the character array defined for storing Current Directory name
//vector<string> dirContentVector;

struct termios cmode;

bool enableNFlag = false;

int insertFlag = 0;

int cmdCursorPtr = 0; // Cursor Pointer for Navigation in command mode

int max_windowSize = 0;

stack<string> fwd_stack;

stack<string> bwd_stack;

vector<string> showDirectoryContentVector; // For cursor and scrolling implementation check
int startIndexPtr = 0;                     // Points to starting record in the present window
int lastIndexPtr = 0;                      // Always point to the last record number
int Cursor = 25;                           // keeps track of Maximum Records a cursor can hold

int cursorPtr = 0;

string dirString = ""; // string to capture directory details

int showStatusFlag = 0;
vector<string> cmdList;

void clrscr() // Function to implement clear screen
{

  cout << "\033[2J\033[1;1H"; // String of special characters to clear the terminal screen
}

void printNewContents()
{

  if ((lastIndexPtr - startIndexPtr) < Cursor)
  {

    for (int i = startIndexPtr; i <= lastIndexPtr; i++)
    {

      cout << std::left << std::setw(40) << showDirectoryContentVector[i];
    }
  }

  else
  {
    for (int i = startIndexPtr; i < startIndexPtr + Cursor; i++)
    {
      cout << showDirectoryContentVector[i];
    }
  }
}

void printContents()
{

  clrscr();

  if ((lastIndexPtr - startIndexPtr) < Cursor)
  {

    for (int i = startIndexPtr; i <= lastIndexPtr; i++)
    {

      cout << std::left << std::setw(40) << showDirectoryContentVector[i];
    }
  }

  else
  {
    for (int i = startIndexPtr; i < startIndexPtr + Cursor; i++)
    {
      cout << showDirectoryContentVector[i];
    }
  }

  if (showStatusFlag == 1) // To show Status Bar
  {
    cout << "\033[" << max_windowSize << ";" << 1 << "H"; // Get the cursor to the last line of the terminal screen
    string tempString = string(parentArray);
    tempString[0] = '~';
    cout << " Current Directory name is :  " << tempString; // Replaced '.' with '~'
    cout << "\033[" << 1 << ";" << 1 << "H";
  }
}

void listDirectoryContentsCheck(string parentDir) // Display the contents on the screen and update the parent Array
{

  clrscr();

  showDirectoryContentVector.clear(); // Clear the already available contents
  startIndexPtr = 0;                  //Initialize start Index Ptr again

  strcpy(parentArray, parentDir.c_str()); // copying the contents of parentDir string to parentArray character array

  DIR *dr; // Pointer to point to Directory stream

  struct dirent *dirEntry = NULL;
  struct stat *statPtr = (struct stat *)malloc(sizeof(struct stat));

  clrscr(); // Call this Function to implement clear screen

  // dirContentVector.clear();

  dr = opendir(parentArray);

  if (dr == NULL)
  {
    cout << "No entries present";
    return;
  }

  string statPath = "";

  while ((dirEntry = readdir(dr)) != NULL)
  {

    dirString = dirString + string(dirEntry->d_name) + "  ";

    statPath = string(parentArray) + "/" + string(dirEntry->d_name);

    char *statPathArray = new char[statPath.length() + 1];

    strcpy(statPathArray, statPath.c_str());

    stat(statPathArray, statPtr); //  directory/file name is passed to statPtr pointer so that the file properties could be obtained.

    dirString = dirString + to_string(statPtr->st_size) + " "; // Add to Final string

    switch (statPtr->st_mode & S_IFMT)
    {

    case S_IFDIR:
      dirString = dirString + "d";
      break; // Check for Directory and assign value 'd'
    case S_IFREG:
      dirString = dirString + "-";
      break; // Check for Regular file and assign value '-'
    case S_IFBLK:
      dirString = dirString + "b";
      break; // Check for block special file and assign value 'b'
    case S_IFCHR:
      dirString = dirString + "c";
      break; // Check for character special file and assign value 'c'
    }

    /* *******************************************************************************************************************

            Condition To check Permission bits and set the values accordingly for Owner, Groups of owner and other users

      ***********************************************************************************************************************/

    // User Privileges

    if (statPtr->st_mode & S_IRUSR) // Read Privileges of User
    {
      dirString = dirString + "r";
    }

    else
    {
      dirString = dirString + "-";
    }

    if (statPtr->st_mode & S_IWUSR) // Write Privileges of User
    {
      dirString = dirString + "w";
    }

    else
    {
      dirString = dirString + "-";
    }

    if (statPtr->st_mode & S_IXUSR) // Execute Privileges of User
    {
      dirString = dirString + "x";
    }

    else
    {
      dirString = dirString + "-";
    }

    // Group Privileges

    if (statPtr->st_mode & S_IRGRP) // Read Privileges of Group
    {
      dirString = dirString + "r";
    }

    else
    {
      dirString = dirString + "-";
    }

    if (statPtr->st_mode & S_IWGRP) // Write Privileges of Group
    {
      dirString = dirString + "w";
    }

    else
    {
      dirString = dirString + "-";
    }

    if (statPtr->st_mode & S_IXGRP) // Execute Privileges of Group
    {
      dirString = dirString + "x";
    }

    else
    {
      dirString = dirString + "-";
    }

    // Others Privileges

    if (statPtr->st_mode & S_IROTH) // Others can read this file
    {
      dirString = dirString + "r";
    }

    else
    {
      dirString = dirString + "-";
    }

    if (statPtr->st_mode & S_IWOTH) // Others can write this file
    {
      dirString = dirString + "w";
    }

    else
    {
      dirString = dirString + "-";
    }

    if (statPtr->st_mode & S_IXOTH) // Others can execute this file
    {
      dirString = dirString + "x";
    }

    else
    {
      dirString = dirString + "-";
    }

    dirString = dirString + " ";

    dirString = dirString + string(ctime(&statPtr->st_mtime));

    showDirectoryContentVector.push_back(dirString); // Push the string to showDirectoryContrnt vector so that it can be used for display in printContents function

    dirString = ""; // The string is nulled to store new strings in the vector

  } // End of While loop for directory entries

  lastIndexPtr = showDirectoryContentVector.size() - 1;

  closedir(dr); // Close the directory stream

  free(statPtr);

  return;
}

/**********************************************************************************************
 
                                       Function to move the cursor up 

             **********************************************************************************************/

int moveUp()
{

  if (cursorPtr == 0)
    return 0;

  if (cursorPtr <= lastIndexPtr)
  {

    cursorPtr--;       // Decrement the value of present cursor pointer
    cout << "\033[1A"; // Move the cursor up
  }

  return cursorPtr;
}

/**********************************************************************************************
 
                                        Function to move the cursor down

            **********************************************************************************************/

int moveDown()
{
  if (cursorPtr < lastIndexPtr)
  {

    last_row++;

    cout << "\033[1B"; // Move the cursor down

    cursorPtr++; // Increment the value of present cursor pointer
  }

  return last_row;
}

/**********************************************************************************************
 
                                        Function Called when Left Key is pressed

            **********************************************************************************************/

void leftKeyPressEvent()
{

  string current_path = string(parentArray);

  string rootPath = ".";

  string newPath = "";

  if (!bwd_stack.empty())
  {

    string prevPath = bwd_stack.top();

    bwd_stack.pop();

    newPath = prevPath;

    listDirectoryContentsCheck(newPath); // newPath will now store the path obtained after pressing left arrow
    printContents();

    cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position

    startIndexPtr = 0;
    cursorPtr = startIndexPtr;

    insertFlag = 0;
  }

  else
  {
    insertFlag = 1;
  }

  if (insertFlag == 0)
  {
    fwd_stack.push(current_path); // Stored in Forward stack for the event if right arrow key is pressed from the new state obtained after this operation
    insertFlag = 1;
  }
}

/**********************************************************************************************
 
                                        Function Called when Right Key is pressed

            **********************************************************************************************/

void rightKeyPressEvent()
{

  string current_path = string(parentArray);

  string rootPath = ".";

  string newPath = "";

  if (!fwd_stack.empty())
  {

    string prevPath = fwd_stack.top();

    fwd_stack.pop();

    newPath = prevPath;

    listDirectoryContentsCheck(newPath); // newPath will now store the path obtained after pressing right arrow key

    printContents(); // To print the COntents

    cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position

    startIndexPtr = 0;
    cursorPtr = startIndexPtr;

    insertFlag = 0;
  }

  else
  {
    insertFlag = 1;
  }

  if (insertFlag == 0)
  {
    bwd_stack.push(current_path); // Stored in Forward stack for the event if right arrow key is pressed from the new state obtained after this operation
    insertFlag = 1;
  }
}

/**********************************************************************************************
 
                                        Function Called when Backspace Key is pressed

            **********************************************************************************************/

void backspaceKeyPressEvent() // Move to the parent directory of the current directory and empty the fwd stack
{

  if (!bwd_stack.empty() && (string)parentArray != ".") // If Parent Array is not empty
  {

    string current_path = string(parentArray);
    string new_path = "";
    int posIndex = 0;

    int len = current_path.length();

    for (int i = len - 1; i >= 0; i--)
    {
      if (current_path[i] == '/') // Check for last found '/'  character
      {
        new_path = current_path.substr(0, i);
        break;
      }
    }

    bwd_stack.push(current_path); // Stored in Forward stack for the event if right arrow key is pressed from the new state obtained after this operation

    while (!fwd_stack.empty()) // fwd stack is emptied
    {

      fwd_stack.pop();
    }

    listDirectoryContentsCheck(new_path); // current_path will now store the path obtained after pressing backspace arrow key

    printContents(); // To print Contents

    cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position

    startIndexPtr = 0;
    cursorPtr = startIndexPtr;
  }
}

void listDefaultContents() // Show the contents of Default Home directory
{

  string current_path = string(parentArray);

  string rootPath = string(".");

  while (fwd_stack.size() != 0) // fwd stack is emptied
  {

    fwd_stack.pop();
  }

  while (bwd_stack.size() != 0) // bwd stack is emptied
  {

    bwd_stack.pop();
  }

  bwd_stack.push(current_path);

  listDirectoryContentsCheck(rootPath); // current_path will now store the path obtained after pressing 'h' key

  printContents();

  cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position

  startIndexPtr = 0;
  cursorPtr = startIndexPtr;
}

/**********************************************************************************************
 
                           Function to check for the events when Enter key is pressed

             **********************************************************************************************/

void enterKeyPressed()
{

  char tempArray[2000];

  string curr_dir_Fullcontent = "";

  string curr_dir_content = "";
  curr_dir_Fullcontent = showDirectoryContentVector[cursorPtr];

  int i = 0;

  while (curr_dir_Fullcontent[i] != ' ')
  {
    i++;
  }

  curr_dir_content = curr_dir_Fullcontent.substr(0, i); // Stores only the File name

  i = 0;

  string currDir = "";

  currDir = parentArray; // Get the current directory from parentArray

  bwd_stack.push(currDir); // Update the Back stack

  strcpy(tempArray, parentArray);

  currDir = currDir + "/" + curr_dir_content;

  listDirectoryContentsCheck(currDir); // After this function is called , parent Array gets updated as well as lastIndexPtr
  printContents();                     // To get the contents after Enter key is pressed

  cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position

  startIndexPtr = 0; // Added Oct 7 : Added in order to always point to the first record in the new page
  cursorPtr = startIndexPtr;
}

/**********************************************************************************
                *                  Function to Reset Non Canonical Mode                          *
                *                                                                                *
                * ********************************************************************************/

void resetNonCanonicalMode()
{
  tcsetattr(STDIN_FILENO, TCSANOW, &cmode);
}

void enableNonCanonicalMode()
{

  // cout<<cmode.c_lflag<<endl;

  if (!enableNFlag)
  {
    enableNFlag = true;

    tcgetattr(STDIN_FILENO, &cmode);

    atexit(resetNonCanonicalMode); //[Just for checking]

    struct termios new_cmode = cmode;
    new_cmode.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &new_cmode);
  }

  return;
}

/************************************************************************************************************************
        *                     
        * 
        * 
        * 
        * 
        * 
        *                           Command Mode Generic Functions Start From Here....
        * 
        * 
        * 
        *
        * 
        * **********************************************************************************************************************/

string getFileName(string inputPath)
{

  int n = inputPath.length();

  string fileName = "";
  int index;

  for (int i = n - 1; i >= 0; i--)
  {
    if (inputPath[i] == '/')
    {
      index = i;
      break;
    }
  }

  fileName = inputPath.substr(index + 1, n - index + 1);

  return fileName;
}

string getAbsolutePath(string inputPath) // Get absolute Path wrt to application
{

  string returnPath = "";

  if (inputPath[0] != '.' && inputPath[0] != '~' && inputPath[0] != '/') // Means only file name is passed as input
  {
    returnPath = (string)parentArray + "/" + inputPath;
    return returnPath;
  }

  if (inputPath[0] == '~') // Replace '~' with '.'
  {
    inputPath[0] = '.';
    return inputPath;
  }

  if (inputPath == ".")
  {
    returnPath = (string)parentArray;
    return returnPath;
  }

  else if (inputPath[0] == '.') // Should point to path of current directory
  {
    returnPath = (string)parentArray + inputPath.substr(1, inputPath.length());
    return returnPath;
  }

  if (inputPath[0] == '/')
  {
    returnPath = "." + inputPath;
    return returnPath;
  }

  return inputPath; // If we get relative path as input of the form ./Test/abc.txt
}

bool isDirectory(string inputPath) // Check for if the file is directory or not
{

  DIR *dr; // Pointer to point to Directory stream
  struct dirent *dirEntry = NULL;

  char *pathArray = new char[inputPath.length() + 1];

  strcpy(pathArray, inputPath.c_str());
  dr = opendir(pathArray);

  if (dr != NULL)
  {
    closedir(dr);
    return true;
  }

  return false;
}

/****************************************************************************************************************************
    * 
    * 
    * 
    *                                     Command Line Functions  Below  .......
    *                                    
    * 
    * 
    * 
    * 
    * ********************************************************************************************************************************/

/**************************************
            Clear Command Line Screen Function

         ************************************/

void clearCommandScreen()
{
  cmdList.clear();

  cout << "\033[" << 27 << ";" << 2 << "K"; // First, Move the cursor to starting of command window
  cout << "\033[" << 27 << ";" << 2 << "H";

  cout << "\033[J";

  cout << "\033[" << 27 << ";" << 2 << "K";
  cout << "\033[" << 27 << ";" << 2 << "H";
  cout << ":";

  cmdCursorPtr = 27; // Initialize the cmd Cursor ptr to the start of the new Command Window Line

  cout << "\033[" << max_windowSize << ";" << 1 << "H"; // Get the cursor to the last line of the terminal screen

  string tempString = string(parentArray);
  tempString[0] = '~';
  cout << " Current Directory name is :  " << tempString; // Replaced '.' with '~'
  cout << "\033[" << 27 << ";" << 5 << "H";
}

void printNewCommandModeContents() // To print command mode contents after commands are entered like copy, rename etc
{
  cout << "\033[" << 27 << ";" << 2 << "K";
  cout << "\033[" << 27 << ";" << 2 << "H";
  cout << ":";
  cout << "\033[" << 27 << ";" << 5 << "H";

  cmdCursorPtr = 27;

  for (int i = 0; i < cmdList.size(); i++)
  {
    cout << cmdList[i];
    cmdCursorPtr++;
    cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";
  }

  cout << "\033[" << max_windowSize << ";" << 1 << "H"; // Get the cursor to the last line of the terminal screen
  string tempString = string(parentArray);
  tempString[0] = '~';
  cout << " Current Directory name is :  " << tempString; // Replaced '.' with '~'
  cout << "\033[" << 27 << ";" << 5 << "H";
}

/************************************
            Copy File Function Call

         ************************************/

void copyFileCommand(string srcFilePath, string destDir)
{

  int rd;

  char block[1024];

  char *src = new char[srcFilePath.size() + 1];

  strcpy(src, srcFilePath.c_str()); // Store the source path

  string fname = getFileName(string(src));
  destDir = destDir + "/" + fname; // Store the file name in the destination Directory

  char *final = new char[destDir.size() + 1];
  strcpy(final, destDir.c_str()); // Store the destination path

  int fd1 = open(src, O_RDONLY);
  int fd2 = open(final, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  while ((rd = read(fd1, block, sizeof(block))) > 0)
  {
    write(fd2, block, rd);
  }

  struct stat srcstat;
  struct stat finalstat;

  stat(src, &srcstat);
  stat(final, &finalstat);

  int permFlag = chmod(final, srcstat.st_mode); // Setting Permission using chmod

  int ownerFlag = chown(final, srcstat.st_uid, srcstat.st_gid); // Setting ownership using  chown
}

/************************************
            Copy Directory Function Call

         ************************************/

void copyDirCommand(string srcDirPath, string destDir)
{

  DIR *dr; // Pointer to point to Directory stream
  struct dirent *dirEntry = NULL;

  string fileNameAppend = getFileName(srcDirPath);
  destDir = destDir + "/" + fileNameAppend;

  char *srcDirPathArray = new char[srcDirPath.length() + 1];

  strcpy(srcDirPathArray, srcDirPath.c_str()); // Copying of Source Directory Path completed

  char *destDirArray = new char[destDir.length() + 1];
  strcpy(destDirArray, destDir.c_str()); // Copying of Destination Directory Path completed

  int newDirStatus = mkdir(destDirArray, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); // New Directory is created in Destination Path

  if (newDirStatus != 0)
    cout << "Error in making new Directory....";

  dr = opendir(srcDirPathArray); // Read the contents of the File to be copied

  if (dr == NULL)
  {
    cout << "No entries present";
    return;
  }

  while ((dirEntry = readdir(dr)) != NULL)
  {

    if (!((string(dirEntry->d_name) == ".") || (string(dirEntry->d_name) == "..")))

    {

      struct stat sb;

      string subFilePath = srcDirPath + "/" + string(dirEntry->d_name);
      char *newSubPath = new char[subFilePath.length() + 1];
      strcpy(newSubPath, subFilePath.c_str());

      string subDestPath = destDir;

      int validateStatus = stat(newSubPath, &sb);

      if ((S_ISDIR(sb.st_mode)))
      {
        copyDirCommand(subFilePath, subDestPath);
      }
      else
      {

        copyFileCommand(subFilePath, subDestPath);
      }
    }
  }
}

/************************************
              Create File Function Call

         ************************************/

void createFileCommand(string srcFileName, string destDir)
{
  destDir = getAbsolutePath(destDir); //  Get absolute File names with respect to current directories  //Added

  destDir = destDir + "/" + srcFileName; // Store the file name in the destination Directory

  char *destDirArray = new char[destDir.length() + 1];

  ofstream of(destDir);

  of.close(); // Close File pointer
}

/************************************
              Create Directory Function Call

         ************************************/

void createDirCommand(string srcDirName, string destDir)
{

  destDir = getAbsolutePath(destDir); //  Get absolute File names with respect to current directories  //Added

  destDir = destDir + "/" + srcDirName; // Store the file name in the destination Directory

  char *destDirArray = new char[destDir.length() + 1];

  strcpy(destDirArray, destDir.c_str());

  int checkStatus = mkdir(destDirArray, 0777); // mkdir returns 0 on successful execution

  if (checkStatus != 0)
  {
    cout << "Directory cannot be created";
  }
}

/************************************
             Delete File Function Call

         ************************************/

void deleteFileCommand(string destDir)
{
  char *destDirArray = new char[destDir.length() + 1];

  strcpy(destDirArray, destDir.c_str());

  int checkStatus = remove(destDirArray); // remove returns 0 on successful execution

  if (checkStatus != 0)
  {
    cout << "File cannot be deleted";
  }
}

/**************************************
              Delete Directory Function Call

         ************************************/

void deleteDirCommand(string destDir) // To be modified // Done
{

  DIR *dr; // Pointer to point to Directory stream
  struct dirent *dirEntry = NULL;

  char *delPathArray = new char[destDir.length() + 1]; // Character Array to be used in opendir

  strcpy(delPathArray, destDir.c_str()); // Copying of Source  Path into character array completed
  dr = opendir(delPathArray);            // Read the contents of the File to be copied

  while ((dirEntry = readdir(dr)) != NULL)
  {

    if (!((string(dirEntry->d_name) == ".") || (string(dirEntry->d_name) == ".."))) // Ignore '.' and '..' entries

    {

      struct stat sb;

      string newPath = destDir + "/" + string(dirEntry->d_name);
      char *newPathArray = new char[newPath.length() + 1];

      strcpy(newPathArray, newPath.c_str());

      if (isDirectory(newPath)) // If newPath is a directory, search the contents of the directory recursively
      {

        deleteDirCommand(newPath); // Call recursively
      }

      else // Check if the entry is a file
      {
        deleteFileCommand(newPath); //  Call deleteFileCommand function to delete the file
      }
    }
  }

  //  closedir(dr);
  int a = rmdir(delPathArray);

  return;
}

/***************************************
                 Rename Function Call

             ************************************/

void renameFile(string o_name, string n_name) // To be modified
{

  o_name = getAbsolutePath(o_name); //  To get the absolute paths
  n_name = getAbsolutePath(n_name);

  char *tempOld = new char[o_name.length() + 1];
  char *tempNew = new char[n_name.length() + 1];

  strcpy(tempOld, o_name.c_str());
  strcpy(tempNew, n_name.c_str());

  int checkStatus = rename(tempOld, tempNew); // rename returns 0 on successful execution

  if (checkStatus != 0)
  {
    cout << "Files cannot be renamed";
  }
}

/***************************************
              Seach File/Directory Function Call

           **************************************/

int searchFileCommand(string getPath, string searchName) // Navigates through the current directory and sub directories and returns the result for the file to be searched.
{

  int searchStatus = 0; // Flag to check if required matching entry is found or not

  DIR *dr; // Pointer to point to Directory stream
  struct dirent *dirEntry = NULL;

  char *searchPathArray = new char[getPath.length() + 1]; // Character Array to be used in opendir

  strcpy(searchPathArray, getPath.c_str()); // Copying of Source  Path into character array completed

  dr = opendir(searchPathArray); // Read the contents of the File to be copied

  while ((dirEntry = readdir(dr)) != NULL)
  {

    if (!((string(dirEntry->d_name) == ".") || (string(dirEntry->d_name) == ".."))) // Ignore '.' and '..' entries
    {
      struct stat sb;

      string newPath = getPath + "/" + string(dirEntry->d_name);

      if (isDirectory(newPath)) // If newPath is a directory, search the contents of the directory recursively
      {
        if (string(dirEntry->d_name) == searchName)
        {
          searchStatus = 1;
          return searchStatus;
        }

        searchStatus = searchFileCommand(newPath, searchName);

        if (searchStatus == 1)
          return searchStatus;
      }

      else // Check if the file name matches with the directory entry
      {
        if (string(dirEntry->d_name) == searchName)
        {
          searchStatus = 1;
          return searchStatus;
        }
      }
    }
  }

  return searchStatus;
}

/********************************************************************************************
               *     
               *                            Initate Command Mode Function  
               * 
               *            (  Driver Function to initiate Different operations in Command Mode  )
               * 
               * 
               * ********************************************************************************************/

int initiateCommandMode()
{
  char inputChar;

  vector<string> tokens;
  string tokenStr = "";

  string cwd = "."; // Stores Current Directory as root directory

  do
  {

    tokenStr = "";

    while ((inputChar = cin.get()) != 10 && inputChar != 27) // While Input Key is not equal to Enter Key(10) and Input char is not equal to ESC key
    {
      cout << inputChar;

      if (inputChar == 127) // when backspace is pressed
      {
        cout << "\b \b";
        if (tokenStr != "")
          tokenStr.pop_back();

        continue;
      }

      if (inputChar != ' ')
      {
        tokenStr = tokenStr + inputChar;
      }

      else
      {
        tokens.push_back(tokenStr);
        tokenStr = "";
      }
    }

    tokens.push_back(tokenStr); // Push the last token into the vector after Enter key is pressed

    string tempString = "";

    for (int i = 0; i < tokens.size(); i++)
    {
      tempString = tempString + tokens[i];
      tempString += " ";
    }

    cmdList.push_back(tempString);

    tempString = "";

    int commandLen = tokens.size();

    if (inputChar == 10) // Print new line when Enter Key is pressed
    {

      if (cmdCursorPtr == max_windowSize - 2)
      {

        clearCommandScreen();
        continue;
      }

      cmdCursorPtr++;

      cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";
    }

    /*****************************************************************************************************
                      // Check for values Present in 1st token and choose the function accordingly
             ***************************************************************************************************/

    /*************************************************
                    Case 1 : Copy File and Directories

         **************************************************/

    if (tokens[0] == "copy")
    {

      string destPath = tokens[tokens.size() - 1]; // Get the last token from tokens vector => It represents final destination

      destPath = getAbsolutePath(destPath); //  Get absolute File names with respect to current directories

      for (int i = 1; i < tokens.size() - 1; i++)
      {
        string filePath = tokens[i];
        filePath = getAbsolutePath(filePath); //  Get absolute File names with respect to current directories

        if (isDirectory(filePath))
        {

          copyDirCommand(filePath, destPath);
        }

        else
        {
          //  cout<<"Hi";
          copyFileCommand(filePath, destPath);
        }
      }

      listDirectoryContentsCheck(string(parentArray)); // parentArray gets updated and stores latest vector contents // 0ct12
      cout << "\033[" << 1 << ";" << 1 << "H";         // To move the cursor to topmost position
      printNewContents();

      startIndexPtr = 0;
      cursorPtr = startIndexPtr;

      printNewCommandModeContents(); // To print command mode contents

      cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

      tokens.clear(); // Clear the tokens vector after operation is successful
    }

    /************************************
                     Case 2 : Create File 

              ************************************/

    else if (tokens[0] == "create_file")
    {
      if (tokens.size() != 3)
        cout << "Invalid Number of Arguments" << endl;

      string destPath = tokens[tokens.size() - 1]; // Get the last token from tokens vector => It represents final destination

      for (int i = 1; i < tokens.size() - 1; i++)
      {
        string filename = tokens[i];
        createFileCommand(filename, destPath); // Call the createFileCommand function
      }

      listDirectoryContentsCheck(string(parentArray)); // parentArray gets updated and stores latest vector contents // 0ct12
      cout << "\033[" << 1 << ";" << 1 << "H";         // To move the cursor to topmost position
      printNewContents();

      startIndexPtr = 0;
      cursorPtr = startIndexPtr;

      printNewCommandModeContents(); // To print command mode contents

      cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

      tokens.clear(); // Clear the tokens vector after operation is successful
    }

    /************************************
                    Case 3 : Create Directory 

         ************************************/

    else if (tokens[0] == "create_dir")
    {
      if (tokens.size() != 3)
        cout << "Invalid Number of Arguments" << endl;

      string destPath = tokens[tokens.size() - 1]; // Get the last token from tokens vector => It represents final destination

      for (int i = 1; i < tokens.size() - 1; i++)
      {
        string dirname = tokens[i];
        createDirCommand(dirname, destPath); // Call the createDirCommand function
      }

      listDirectoryContentsCheck(string(parentArray)); // parentArray gets updated and stores latest vector contents // 0ct12
      cout << "\033[" << 1 << ";" << 1 << "H";         // To move the cursor to topmost position
      printNewContents();

      startIndexPtr = 0;
      cursorPtr = startIndexPtr;

      printNewCommandModeContents(); // To print command mode contents

      cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

      tokens.clear(); // Clear the tokens vector after operation is successful
    }

    /************************************
                    Case 4 : Delete File 

         ************************************/

    else if (tokens[0] == "delete_file")
    {

      if (tokens.size() != 2)
        cout << "Invalid Number of Arguments" << endl;

      string destPath = tokens[tokens.size() - 1]; // Get the last token from tokens vector => It represents final destination

      destPath = getAbsolutePath(destPath); //  Get absolute File names with respect to current directories  //Added // Oct 12

      deleteFileCommand(destPath); // Call the deleteFileCommand function

      listDirectoryContentsCheck(string(parentArray)); // parentArray gets updated and stores latest vector contents // 0ct12
      cout << "\033[" << 1 << ";" << 1 << "H";         // To move the cursor to topmost position
      printNewContents();

      startIndexPtr = 0;
      cursorPtr = startIndexPtr;

      printNewCommandModeContents(); // To print command mode contents

      cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

      tokens.clear(); // Clear the tokens vector after operation is successful
    }

    /************************************
                  Case 5 : Delete Directory 

         ************************************/

    else if (tokens[0] == "delete_dir")
    {

      if (tokens.size() != 2)
        cout << "Invalid Number of Arguments" << endl;

      string destPath = tokens[tokens.size() - 1]; // Get the last token from tokens vector => It represents final destination

      destPath = getAbsolutePath(destPath); //  Get absolute File names with respect to current directories  // Oct 12

      deleteDirCommand(destPath); // Call the deleteFileCommand function

      listDirectoryContentsCheck(string(parentArray)); // parentArray gets updated and stores latest vector contents // 0ct12
      cout << "\033[" << 1 << ";" << 1 << "H";         // To move the cursor to topmost position
      printNewContents();

      startIndexPtr = 0;
      cursorPtr = startIndexPtr;

      printNewCommandModeContents(); // To print command mode contents

      cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

      tokens.clear(); // Clear the tokens vector after operation is successful
    }

    /************************************
           Case 6 : Rename Fiels / Directories

         ************************************/

    else if (tokens[0] == "rename")
    {

      if (tokens.size() != 3)
        cout << "Invalid Number of Arguments" << endl;

      string newname = tokens[tokens.size() - 1]; // Stores the new name

      string oldname = tokens[1]; // Stores the old name

      renameFile(oldname, newname);

      listDirectoryContentsCheck(string(parentArray)); // parentArray gets updated and stores latest vector contents // 0ct12
      cout << "\033[" << 1 << ";" << 1 << "H";         // To move the cursor to topmost position
      printNewContents();

      startIndexPtr = 0;
      cursorPtr = startIndexPtr;

      printNewCommandModeContents(); // To print command mode contents

      cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

      tokens.clear(); // Clear the tokens vector after operation is successful
    }

    /************************************
           Case 7 : Move Files/ Directories

         ************************************/

    else if (tokens[0] == "move")
    {

      if (tokens.size() < 3)
        cout << "Invalid Number of Arguments" << endl;

      string destPath = tokens[tokens.size() - 1]; // Get the last token from tokens vector => It represents final destination

      destPath = getAbsolutePath(destPath); // get relative path wrt application  //Oct 12

      for (int i = 1; i < tokens.size() - 1; i++)
      {
        string filePath = tokens[i];

        filePath = getAbsolutePath(filePath); // get  path wrt application  // Oct 12

        if (isDirectory(filePath))
        {

          copyDirCommand(filePath, destPath); // Copy the directory to Destination
          deleteDirCommand(filePath);         // Delete the file after copying is done
        }

        else
        {
          copyFileCommand(filePath, destPath);
          deleteFileCommand(filePath);
        }
      }

      listDirectoryContentsCheck(string(parentArray)); // parentArray gets updated and stores latest vector contents // 0ct12
      cout << "\033[" << 1 << ";" << 1 << "H";         // To move the cursor to topmost position
      printNewContents();

      startIndexPtr = 0;
      cursorPtr = startIndexPtr;

      printNewCommandModeContents(); // To print command mode contents

      cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

      tokens.clear(); // Clear the tokens vector after operation is successful
    }

    /************************************
               Case 8 : Goto Command

         ************************************/

    else if (tokens[0] == "goto")
    {

      if (tokens.size() != 2)
        cout << "Invalid Number of Arguments" << endl;

      string destPath = tokens[tokens.size() - 1]; // Get the last token from tokens vector => It represents final destination

      destPath = getAbsolutePath(destPath); // Added Oct 11

      cmdList.clear(); // Oct 12

      startIndexPtr = 0; // To point to the topmost record in the directory

      string presentPath = string(parentArray);

      bwd_stack.push(presentPath); // Added Oct 9

      listDirectoryContentsCheck(destPath);

      tokens.clear(); // Clear the tokens vector after operation is successful

      return -2;
    }

    /************************************
           Case 9 : Search Command

         ************************************/

    else if (tokens[0] == "search") // Only file name / directory name to be provided as argument
    {

      if (tokens.size() != 2)
        cout << "Invalid Number of Arguments" << endl;

      string searchFileName = tokens[tokens.size() - 1]; // Get the last token from tokens vector => It represents final destination
      string presentDirectory = string(parentArray);     // parentArray is the character array that stores current directory information

      int searchStatusFlag = 0; // A flag to check if File is finally Found or not. If found, then it returns 1

      searchStatusFlag = searchFileCommand(presentDirectory, searchFileName);

      if (searchStatusFlag == 1) // If searchStatusFlag==1
      {
        if (cmdCursorPtr >= max_windowSize - 3)
        {

          clearCommandScreen();
          //continue;
          cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

          cout << "True";
          cmdCursorPtr++;
          cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";
        }

        else
        {
          cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

          cout << "True";
          cmdCursorPtr++;
          cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";
        }
      }

      else // Start of Else condition  (If searchStatusFlag != 1)
      {
        //cmdCursorPtr++;
        if (cmdCursorPtr >= max_windowSize - 3)
        {

          clearCommandScreen();
          //continue;
          cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";

          cout << "True";
          cmdCursorPtr++;
          cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";
        }

        else
        {
          cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";
          cout << "False";
          cmdCursorPtr++;
          cout << "\033[" << cmdCursorPtr << ";" << 5 << "H";
        }

      } // End of else condition

      tokens.clear(); // Clear the tokens vector after operation is successful
    }

  } while (inputChar != 27); // While ESC is not pressed, keep on taking the inputs

  if (inputChar == 27) // When ESC key is pressed
  {
    cmdList.clear();
    int status = -1;
    return status;
  }

  return 0;
}

/*************************  Main Function Here.... ***************************/

int main()
{

  string parentDir = "."; // string to store parent directory
  string currentDir = ""; // string to store current directory

  string rootString = "~";

  listDirectoryContentsCheck(parentDir);

  printContents(); // To print contents from start to cursor and Status Bar implementation is done here....

  enableNonCanonicalMode();

  cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position

  struct winsize ws; // To keep track of window size
  int fdWin;

  //	 Open the controlling terminal.
  fdWin = open("/dev/tty", O_RDWR);

  //   	 Get window size of terminal.

  ioctl(fdWin, TIOCGWINSZ, &ws);

  max_windowSize = ws.ws_row; // Get the maximum window size

  close(fdWin);

  // Status Bar Implementation for Home Directory

  cout << "\033[" << max_windowSize << ";" << 1 << "H";   // Get the cursor to the last line of the terminal screen
  cout << " Current Directory name is :  " << rootString; // Represents '.' as '~' stored in rootString string
  cout << "\033[" << 1 << ";" << 1 << "H";

  showStatusFlag = 1; // Set the status Bar Flag on

  while (1)
  {
    char ch;

    ch = cin.get();

    if (ch == 'A') // Event  when Up key is pressed
    {
      last_row = moveUp();
    }

    else if (ch == 'B') // Event  when Down key is pressed
    {
      last_row = moveDown();
    }

    else if (ch == 10) // Event  when Enter key is pressed
    {
      string cpath = string(parentArray) + "/";
      string temp = showDirectoryContentVector[cursorPtr];

      int subIndex;

      for (int i = 0; i < temp.length(); i++)
      {
        if (temp[i] == ' ')
        {
          subIndex = i;
          break;
        }
      }

      temp = temp.substr(0, subIndex);

      cpath = cpath + temp;

      if (isDirectory(cpath)) // If cpath is directory, retrieve the contents using enterKeyPressed() function
        enterKeyPressed();

      else // For opening the files in vi editor
      {
        int pid = fork();

        if (pid == 0)
        {
          execl("/usr/bin/vi", "vi", cpath.c_str(), NULL);
        }

        else
        {
          wait(NULL);
        }
      }
    }

    else if (ch == 'C') // Right Arrow is pressed
    {
      rightKeyPressEvent();
    }

    else if (ch == 'D') // If left Arrow is pressed
    {

      leftKeyPressEvent();
    }

    else if (ch == 127) // If Backspace is pressed, the parent directory contents of current directory (One Level Up) are shown
    {
      backspaceKeyPressEvent();
    }

    else if (ch == 'h') //  Default Home folder contents are shown
    {
      listDefaultContents();
    }

    else if (ch == 'k') // To perform Scroll Down operation
    {
      if (lastIndexPtr < startIndexPtr + Cursor) // If the displayed records are in the final page, we need not scroll down
      {
        continue;
      }

      startIndexPtr = startIndexPtr + Cursor;
      printContents();
      cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position
      cursorPtr = startIndexPtr;
    }

    else if (ch == 'l') // To perform Scroll Up operation  [Done] // Oct 5
    {
      if (startIndexPtr == 0) // we need not scroll if the cursor is already pointing to topmost record.
      {
        continue;
      }
      startIndexPtr = startIndexPtr - Cursor;
      printContents();
      cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position
      cursorPtr = startIndexPtr;
    }

    else if (ch == 58) // When : is pressed
    {

      cout << "\033[" << 27 << ";" << 2 << "K";
      cout << "\033[" << 27 << ";" << 2 << "H";
      cout << ":";
      cout << "\033[" << 27 << ";" << 5 << "H";

      cmdCursorPtr = 27;

      int status = initiateCommandMode(); // To start the command mode

      if (status == -1) // When ESC key is pressed
      {
        printContents();

        cout << "\033[" << cursorPtr + 1 << ";" << 1 << "H";
      }

      else if (status == -2) // Initiate Normal mode for Goto command
      {

        printContents();
        cout << "\033[" << 1 << ";" << 1 << "H"; // To move the cursor to topmost position

        startIndexPtr = 0;
        cursorPtr = startIndexPtr;
      }
    }
  }

  return 0;
}