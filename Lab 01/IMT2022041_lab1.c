#include <stdio.h>

struct student // struct for student predefined
{
  int rollnum;
  char name[30];
  int age;
};

int save_num_text(char *filename) // function to save numbers in text file
{
  int num[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // integer array of 20 numbers
  FILE *fp = fopen(filename, "w");                                                       // opening file in write mode
  if (fp == NULL)                                                                        // if file not opened
  {
    printf("Error opening file\n");
    return 1;
  }
  else // else file opened
  {
    for (int i = 0; i < 20; i++)
    {
      fprintf(fp, "%d ", num[i]);
    }
  }
  fclose(fp); // closing file
  return 0;
}

int read_num_text(char *filename) // function to read numbers from text file
{
  FILE *fp = fopen(filename, "r"); // opening file in read mode
  if (fp == NULL)                  // if file not opened
  {
    printf("Error opening file\n");
    return 1;
  }
  else // else file opened
  {
    int num;                              // integer variable to store number
    while (fscanf(fp, "%d", &num) != EOF) // reading file till end of file
    {
      printf("%d ", num);
    }
  }
  fclose(fp); // closing file
  return 0;
}

int save_struct_text(char *filename) // function to save struct in text file
{
  struct student s[5] = // struct array of 5 students
      {
          {1, "A", 20},
          {2, "B", 21},
          {3, "C", 22},
          {4, "D", 23},
          {5, "E", 24}};
  FILE *fp = fopen(filename, "w"); // opening file in write mode
  if (fp == NULL)                  // if file not opened
  {
    printf("Error opening file\n");
    return 1;
  }
  else // else file opened
  {
    for (int i = 0; i < 5; i++) // loop to write struct in file
    {
      fprintf(fp, "%d %s %d\n", s[i].rollnum, s[i].name, s[i].age);
    }
  }
  fclose(fp); // closing file
  return 0;
}

int read_struct_text(char *filename) // function to read struct from text file
{
  FILE *fp = fopen(filename, "r"); // opening file in read mode
  if (fp == NULL)                  // if file not opened
  {
    printf("Error opening file\n");
    return 1;
  }
  else // else file opened
  {
    struct student s;                                                 // struct variable to store student
    while (fscanf(fp, "%d %s %d", &s.rollnum, s.name, &s.age) != EOF) // reading file till end of file
    {
      printf("%d %s %d\n", s.rollnum, s.name, s.age); // printing struct
    }
  }
  fclose(fp); // closing file
  return 0;
}

int save_num_binary(char *filename) // function to save numbers in binary file
{
  int bin[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // integer array of 20 numbers
  FILE *fp = fopen(filename, "wb");                                                      // opening file in write mode
  if (fp == NULL)                                                                        // if file not opened
  {
    printf("Error opening file");
    return 1;
  }
  else // else file opened
  {
    fwrite(bin, sizeof(int), sizeof(bin) / sizeof(int), fp); // writing array in binary format in file
  }
  fclose(fp); // closing file
  return 0;
}

int read_num_binary(char *filename) // function to read numbers from binary file
{
  FILE *fp = fopen(filename, "rb"); // opening file in read mode
  if (fp == NULL)                   // if file not opened
  {
    printf("Error opening file");
    return 1;
  }
  else // else file opened
  {
    int bin[20];                                            // integer array to store numbers
    fread(bin, sizeof(int), sizeof(bin) / sizeof(int), fp); // reading file in binary format
    for (int i = 0; i < 20; i++)                            // loop to print numbers
    {
      printf("%d ", bin[i]);
    }
  }
  fclose(fp); // closing file
  return 0;
}

int save_struct_binary(char *filename) // function to save struct in binary file
{
  struct student s[5] = // struct array of 5 students
      {
          {1, "A", 20},
          {2, "B", 21},
          {3, "C", 22},
          {4, "D", 23},
          {5, "E", 24}};
  FILE *fp = fopen(filename, "wb"); // opening file in write mode
  if (fp == NULL)                   // if file not opened
  {
    printf("Error opening file");
    return 1;
  }
  else // else file opened
  {
    fwrite(s, sizeof(struct student), sizeof(s) / sizeof(struct student), fp); // writing struct in binary format in file
  }
  fclose(fp); // closing file
  return 0;
}

int read_struct_binary(char *filename) // function to read struct from binary file
{
  FILE *fp = fopen(filename, "rb"); // opening file in read mode
  if (fp == NULL)                   // if file not opened
  {
    printf("Error opening file");
    return 1;
  }
  else // else file opened
  {
    struct student s;                                // struct variable to store student
    while (fread(&s, sizeof(struct student), 1, fp)) // reading file in binary format
    {
      printf("%d %s %d\n", s.rollnum, s.name, s.age); // printing struct
    }
  }
  fclose(fp); // closing file
  return 0;
}
