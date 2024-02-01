#include <stdio.h>


#include <stdlib.h>

#include <string.h>

#include "pds.h"


struct PDS_RepoInfo repo_handle; // repo_handle is a structure of type PDS_RepoInfo

int pds_create(char *repo_name) // repo_name is the name of the repository

{

	char *data = (char *)malloc(sizeof(repo_name));		   // data is the name of the data file

	char *ndx = (char *)malloc(sizeof(repo_name));		   // ndx is the name of the index file

	strcpy(data, repo_name);							   // copy repo_name to data

	strcpy(ndx, repo_name);								   // copy repo_name to ndx

	strcat(data, ".dat");								   // append .dat to data

	strcat(ndx, ".ndx");								   // append .ndx to ndx

	repo_handle.pds_data_fp = fopen(data, "wb");		   // open data file in write mode

	repo_handle.pds_ndx_fp = fopen(ndx, "wb");			   // open index file in write mode

	if (repo_handle.pds_data_fp && repo_handle.pds_ndx_fp) // if both files are opened successfully

	{

		int ini = 0;										  // ini is the initial value of the number of records in the index file

		fwrite(&ini, sizeof(int), 1, repo_handle.pds_ndx_fp); // write ini to index file

		fclose(repo_handle.pds_data_fp);					  // close data file

		fclose(repo_handle.pds_ndx_fp);						  // close index file

		repo_handle.repo_status = PDS_REPO_CLOSED;			  // set repo_status to closed

		return PDS_SUCCESS;									  // return success

	}

	return PDS_FILE_ERROR; // return file error

}

int pds_open(char *repo_name, int rec_size) // repo_name is the name of the repository, rec_size is the size of the record

{

	strcpy(repo_handle.pds_name, repo_name);		// copy repo_name to pds_name

	char *data = (char *)malloc(sizeof(repo_name)); //	data is the name of the data file

	strcpy(data, repo_name);						// copy repo_name to data

	strcat(data, ".dat");							// append .dat to data

	char *ndx = (char *)malloc(sizeof(repo_name));	// ndx is the name of the index file

	strcpy(ndx, repo_name);							// copy repo_name to ndx

	strcat(ndx, ".ndx");							//	append .ndx to ndx

	if (repo_handle.repo_status == PDS_REPO_CLOSED) // if repo_status is closed

	{

		repo_handle.pds_data_fp = fopen(data, "rb+"); // open data file in read mode

		repo_handle.pds_ndx_fp = fopen(ndx, "rb+");	  // open index file in read mode

		repo_handle.rec_size = rec_size;			  // set rec_size to rec_size

		if (pds_load_ndx() == PDS_SUCCESS)			  // if pds_load_ndx returns success

		{

			fclose(repo_handle.pds_ndx_fp);			 // close index file

			repo_handle.repo_status = PDS_REPO_OPEN; // set repo_status to open

			return PDS_SUCCESS;						 // return success

		}

		else // if pds_load_ndx returns failure

		{

			repo_handle.repo_status = PDS_REPO_OPEN; // set repo_status to open

			return PDS_NDX_SAVE_FAILED;				 // return ndx save failed

		}

	}

	if (repo_handle.repo_status == PDS_REPO_OPEN) // if repo_status is open

	{

		printf("Repo was already open.\n"); //	print repo was already open

		return PDS_REPO_ALREADY_OPEN;		// return repo already open

	}

	return PDS_FILE_ERROR; // return file error

}



int pds_load_ndx() // no parameters

{

	int re_NDX = fread(&repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp); // read number of records from index file

	if (re_NDX == 0)																	  // if re_NDX is 0

	{

		return PDS_NDX_SAVE_FAILED; // return ndx save failed

	}

	fread(repo_handle.ndx_array, sizeof(struct PDS_NdxInfo), repo_handle.rec_count, repo_handle.pds_ndx_fp); // read index entries from index file

	return PDS_SUCCESS;																						 // return success

}



int put_rec_by_key(int key, void *rec) // key is the key of the record, rec is the record

{

	if (repo_handle.repo_status == PDS_REPO_OPEN) // if repo_status is open

	{

		if (fseek(repo_handle.pds_data_fp, 0, SEEK_END) == 0) // if fseek returns 0

		{

			int offset = ftell(repo_handle.pds_data_fp);												// set offset to the current location in the data file

			struct PDS_NdxInfo *temp_Struct = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo)); // temp_Struct is a temporary structure of type PDS_NdxInfo

			temp_Struct->key = key;																		// set key of temp_Struct to key

			temp_Struct->offset = offset;																// set offset of temp_Struct to offset

			repo_handle.ndx_array[repo_handle.rec_count] = *(temp_Struct);								// set the index entry at the current location in ndx_array to temp_Struct

			repo_handle.rec_count++;																	// increment record count

			fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);								// write rec to the data file

			return PDS_SUCCESS;																			// return success

		}

		return PDS_ADD_FAILED; // return add failed

	}

	else // if repo_status is closed

	{

		return PDS_REPO_NOT_OPEN; // return repo not open

	}

}






int get_rec_by_key(int key, void *rec) // key is the key of the record, rec is the record

{

	if (repo_handle.repo_status == PDS_REPO_OPEN) // if repo_status is open

	{

		for (int i = 0; i < repo_handle.rec_count; i++) // for i from 0 to record count

		{

			if (repo_handle.ndx_array[i].key == key) // if key of index entry at i is equal to key

			{

				fseek(repo_handle.pds_data_fp, repo_handle.ndx_array[i].offset, SEEK_SET); // seek to the offset of index entry at i

				fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);			   // read the record at the current location

				return PDS_SUCCESS;														   // return success

			}

		}

		return PDS_REC_NOT_FOUND; // return record not found

	}

	else // if repo_status is closed

	{

		return PDS_REPO_NOT_OPEN; // return repo not open

	}

}

int pds_close() // no parameters

{

	char *temp = (char *)malloc(sizeof(repo_handle.pds_name)); // temp is the name of the index file

	strcpy(temp, repo_handle.pds_name);						   // copy pds_name to temp

	strcat(temp, ".ndx");									   // append .ndx to temp

	repo_handle.pds_ndx_fp = fopen(temp, "wb");				   // open index file in write mode

	if (repo_handle.pds_ndx_fp != NULL)						   // if index file is opened successfully

	{

		fwrite(&repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp);									  // write record count to index file

		fwrite(repo_handle.ndx_array, sizeof(struct PDS_NdxInfo), repo_handle.rec_count, repo_handle.pds_ndx_fp); // write index entries to index file

		fclose(repo_handle.pds_ndx_fp);																			  // close index file

		fclose(repo_handle.pds_data_fp);																		  // close data file

		repo_handle.repo_status = PDS_REPO_CLOSED;																  // set repo_status to closed

		return PDS_SUCCESS;																						  // return success

	}

	return PDS_FILE_ERROR; // return file error

}

