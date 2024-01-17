#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "pds.h"

struct PDS_RepoInfo repo_handle;

int pds_create(char *repo_name) // repo_name is the name of the repository
{
    char Filename[50];                // Filename is the name of the file
    strcpy(Filename, repo_name);      //  Copy repo_name to Filename
    strcat(Filename, ".dat");         //  Concatenate .dat to Filename
    FILE *f = fopen(Filename, "wb+"); //  Open Filename in write mode
    if (f == NULL)                    //  If file is not opened
    {
        return PDS_FILE_ERROR;
    }
    else //  If file is opened
    {
        fclose(f);
        return PDS_SUCCESS;
    }
}

int pds_open(char *repo_name, int rec_size) // repo_name is the name of the repository, rec_size is the size of the record
{
    char *Filename = (char *)malloc(sizeof(repo_name)); // Filename is the name of the file
    strcpy(Filename, repo_name);                        //  Copy repo_name to Filename
    strcpy(repo_handle.pds_name, Filename);             //  Copy Filename to repo_handle.pds_name
    strcat(Filename, ".dat");                           //  Concatenate .dat to Filename
    repo_handle.pds_data_fp = fopen(Filename, "ab+");   //  Open Filename in append mode
    if (repo_handle.pds_data_fp == NULL)                //  If file is not opened
    {
        fclose(repo_handle.pds_data_fp);           //  Close the file
        repo_handle.repo_status = PDS_REPO_CLOSED; //  Update repo_status
        return PDS_FILE_ERROR;                     //  Return PDS_FILE_ERROR
    }
    repo_handle.rec_size = rec_size;         //  Update rec_size
    repo_handle.repo_status = PDS_REPO_OPEN; //  Update repo_status
    return PDS_SUCCESS;                      //  Return PDS_SUCCESS
}
int put_rec_by_key(int key, void *rec) // key is the key of the record, rec is the record to be added
{

    if (fseek(repo_handle.pds_data_fp, 0, SEEK_END) != 0) //    If file pointer is not at the end of the file
    {
        repo_handle.repo_status = PDS_REPO_ALREADY_OPEN; //  Update repo_status
        return PDS_ADD_FAILED;
    }
    fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp);         //  Write key to the file
    fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp); //  Write rec to the file
    return PDS_SUCCESS;
}

int get_rec_by_key(int key, void *rec) // key is the key of the record, rec is the record to be searched
{
    int temp;
    if (fseek(repo_handle.pds_data_fp, 0, SEEK_SET) != 0) //  If file pointer is not at the start of the file
    {
        repo_handle.repo_status = PDS_REPO_ALREADY_OPEN; //  Update repo_status
        return PDS_FILE_ERROR;                           //  Return PDS_FILE_ERROR
    }
    while (fread(&temp, sizeof(int), 1, repo_handle.pds_data_fp) > 0) //  While file is not empty
    {
        if (temp == key) //  If key is found
        {
            repo_handle.repo_status = PDS_REPO_ALREADY_OPEN;              //  Update repo_status
            fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp); //  Read rec from the file
            return PDS_SUCCESS;
        }
        fseek(repo_handle.pds_data_fp, repo_handle.rec_size, SEEK_CUR); //  Move file pointer to the next record
    }
    repo_handle.repo_status = PDS_REPO_ALREADY_OPEN; //  Update repo_status
    return PDS_REC_NOT_FOUND;                        //  Return PDS_REC_NOT_FOUND
}

int pds_close() // Close the repo file
{
    char *temp = (char *)malloc(sizeof(repo_handle.pds_name)); //  temp is a temporary variable
    strcpy(temp, repo_handle.pds_name);                        //  Copy repo_handle.pds_name to temp
    strcat(temp, ".dat");                                      //  Concatenate .dat to temp
    repo_handle.pds_data_fp = fopen(temp, "wb+");              //  Open temp in write mode
    repo_handle.repo_status = PDS_REPO_CLOSED;                 //  Update repo_status
    return PDS_SUCCESS;                                        //  Return PDS_SUCCESS
}