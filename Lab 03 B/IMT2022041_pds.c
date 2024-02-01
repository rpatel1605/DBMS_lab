#include <stdio.h>


#include <stdlib.h>

#include <string.h>

#include "pds.h"



void bst_print_custom(struct BST_Node *root) // Custom function to print the BST

{

	if (root == 0) // If the root is NULL, return

		return;	   // Return

	else

	{																	   // Else

		fwrite(root->data, sizeof(root->data), 1, Repo_handle.pds_ndx_fp); // Write the data to the file

		bst_print(root->left_child);									   // Call the function recursively for the left child

		bst_print(root->right_child);									   // Call the function recursively for the right child

	}

}



struct PDS_RepoInfo Repo_handle; // Global variable to store the repo info

int pds_create(char *repo_name)	 // Function to create the repo

{

	char *T_data = (char *)malloc(sizeof(repo_name));	   // Allocate memory for the data file name

	char *T_ndx = (char *)malloc(sizeof(repo_name));	   // Allocate memory for the index file name

	strcpy(T_data, repo_name);							   // Copy the repo name to the data file name

	strcpy(T_ndx, repo_name);							   // Copy the repo name to the index file name

	strcat(T_data, ".dat");								   // Append the extension to the data file name

	strcat(T_ndx, ".ndx");								   // Append the extension to the index file name

	Repo_handle.pds_data_fp = fopen(T_data, "wb");		   // Open the data file in write mode

	Repo_handle.pds_ndx_fp = fopen(T_ndx, "wb");		   // Open the index file in write mode

	if (Repo_handle.pds_data_fp && Repo_handle.pds_ndx_fp) // If both the files are opened successfully

	{

		int count_zero = 0;											 // Variable to store the number of records

		fwrite(&count_zero, sizeof(int), 1, Repo_handle.pds_ndx_fp); // Write the number of records to the index file

		fclose(Repo_handle.pds_data_fp);							 // Close the data file

		fclose(Repo_handle.pds_ndx_fp);								 // Close the index file

		Repo_handle.repo_status = PDS_REPO_CLOSED;					 // Set the repo status to closed

		free(T_data);												 // Free the memory allocated for the data file name

		free(T_ndx);												 // Free the memory allocated for the index file name

		return PDS_SUCCESS;											 // Return success


	}


	free(T_data);		   // Free the memory allocated for the data file name

	free(T_ndx);		   // Free the memory allocated for the index file name

	return PDS_FILE_ERROR; // Return file error

}



int pds_open(char *repo_name, int rec_size) // Function to open the repo

{


	strcpy(Repo_handle.pds_name, repo_name);		  // Copy the repo name to the repo handle

	char *T_data = (char *)malloc(sizeof(repo_name)); // Allocate memory for the data file name

	strcpy(T_data, repo_name);						  // Copy the repo name to the data file name

	strcat(T_data, ".dat");							  // Append the extension to the data file name

	char *T_ndx = (char *)malloc(sizeof(repo_name));  // Allocate memory for the index file name

	strcpy(T_ndx, repo_name);						  // Copy the repo name to the index file name

	strcat(T_ndx, ".ndx");							  // Append the extension to the index file name

	if (Repo_handle.repo_status == PDS_REPO_CLOSED)	  // If the repo is closed

	{

		Repo_handle.pds_data_fp = fopen(T_data, "rb+"); // Open the data file in read mode

		Repo_handle.pds_ndx_fp = fopen(T_ndx, "rb+");	// Open the index file in read mode

		Repo_handle.rec_size = rec_size;				// Set the record size

		if (pds_load_ndx() == PDS_SUCCESS)				// If the index file is loaded successfully

		{

			fclose(Repo_handle.pds_ndx_fp);			 // Close the index file

			Repo_handle.repo_status = PDS_REPO_OPEN; // Set the repo status to open

			free(T_data);							 // Free the memory allocated for the data file name

			free(T_ndx);							 // Free the memory allocated for the index file name

			return PDS_SUCCESS;						 // Return success

		}

		else // Else

		{

			Repo_handle.repo_status = PDS_REPO_OPEN; // Set the repo status to open

			free(T_data);							 // Free the memory allocated for the data file name

			free(T_ndx);							 // Free the memory allocated for the index file name

			return PDS_NDX_SAVE_FAILED;				 // Return index save failed

		}

	}

	if (Repo_handle.repo_status == PDS_REPO_OPEN) // If the repo is open

	{

		free(T_data);				  // Free the memory allocated for the data file name

		free(T_ndx);				  // Free the memory allocated for the index file name

		return PDS_REPO_ALREADY_OPEN; // Return repo already open

	}

	free(T_data);		   // Free the memory allocated for the data file name

	free(T_ndx);		   // Free the memory allocated for the index file name

	return PDS_FILE_ERROR; // Return file error

}


int pds_load_ndx() // Function to load the index file

{


	int R_ndx = fread(&Repo_handle.rec_count, sizeof(int), 1, Repo_handle.pds_ndx_fp); // Read the number of records from the index file

	if (R_ndx == 0)																	   // If the number of records is 0

	{

		return PDS_NDX_SAVE_FAILED; // Return index save failed

	}

	struct PDS_NdxInfo temp_Ndx_Struct; // Temporary variable to store the index info

	for (int i = 0; i < R_ndx; i++)		// For each record

	{


		fread(&temp_Ndx_Struct, sizeof(struct PDS_NdxInfo), 1, Repo_handle.pds_ndx_fp); // Read the index info

		bst_add_node(&Repo_handle.ndx_root, temp_Ndx_Struct.key, &temp_Ndx_Struct);		// Add the index info to the BST

	}

	return PDS_SUCCESS; // Return success

}

int put_rec_by_key(int key, void *rec) // Function to add a record to the repo

{

	if (Repo_handle.repo_status == PDS_REPO_OPEN) // If the repo is open

	{

		if (fseek(Repo_handle.pds_data_fp, 0, SEEK_END) == 0) // If the file pointer is set to the end of the file

		{

			struct BST_Node *temp_BST_Struct;									   // Temporary variable to store the BST node

			if ((temp_BST_Struct = bst_search(Repo_handle.ndx_root, key)) == NULL) // If the key is not found in the BST

			{

				int offset = ftell(Repo_handle.pds_data_fp);													// Get the offset of the record

				struct PDS_NdxInfo *temp_Struct = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo)); // Allocate memory for the index info

				temp_Struct->key = key;																		// Set the key

				temp_Struct->offset = offset;																		// Set the offset

				Repo_handle.rec_count++;																	// Increment the record count

				fwrite(rec, Repo_handle.rec_size, 1, Repo_handle.pds_data_fp);								// Write the record to the data file

				int stat = bst_add_node(&Repo_handle.ndx_root, key, temp_Struct);							// Add the index info to the BST

				if (stat == BST_SUCCESS)																	// If the index info is added successfully

				{

					return PDS_SUCCESS; // Return success

				}

				else // Else
				{

					return PDS_ADD_FAILED; // Return add failed


				}

				{

					return PDS_ADD_FAILED; // Return add failed
				}

			}

			return PDS_ADD_FAILED; // Return add failed

		}

		return PDS_FILE_ERROR; // Return file error

	}

	else // Else

	{

		return PDS_REPO_NOT_OPEN; // Return repo not open

	}

	return PDS_ADD_FAILED; // Return add failed

}

int get_rec_by_key(int key, void *rec) // Function to get a record from the repo

{

	if (Repo_handle.repo_status == PDS_REPO_OPEN) // If the repo is open


	{

		struct BST_Node *temp_BST_Struct;									   // Temporary variable to store the BST node

		if ((temp_BST_Struct = bst_search(Repo_handle.ndx_root, key)) != NULL) // If the key is found in the BST

		{

			struct PDS_NdxInfo *temp_Struct = (struct PDS_NdxInfo *)(temp_BST_Struct->data); // Get the index info

			if (fseek(Repo_handle.pds_data_fp, temp_Struct->offset, SEEK_SET) == 0)			 // If the file pointer is set to the offset of the record

			{

				fread(rec, Repo_handle.rec_size, 1, Repo_handle.pds_data_fp); // Read the record from the data file

				return PDS_SUCCESS;											  // Return success

			}
			return PDS_FILE_ERROR; // Return file error

		}

		return PDS_REC_NOT_FOUND; // Return record not found

	}

	return PDS_REPO_NOT_OPEN; // Return repo not open

}

int pds_close() // Function to close the repo

{

	char *T = (char *)malloc(sizeof(Repo_handle.pds_name)); // Allocate memory for the file name

	strcpy(T, Repo_handle.pds_name);						// Copy the repo name to the file name

	strcat(T, ".ndx");										// Append the extension to the file name

	Repo_handle.pds_ndx_fp = fopen(T, "wb");				// Open the file in write mode

	if (Repo_handle.pds_ndx_fp != NULL)						// If the file is opened successfully

	{

		fwrite(&Repo_handle.rec_count, sizeof(int), 1, Repo_handle.pds_ndx_fp); // Write the number of records to the file

		bst_print_custom(Repo_handle.ndx_root);									// Print the BST



		fclose(Repo_handle.pds_data_fp);		   // Close the data file

		fclose(Repo_handle.pds_ndx_fp);			   // Close the index file

		Repo_handle.repo_status = PDS_REPO_CLOSED; // Set the repo status to closed

		free(T);								   // Free the memory allocated for the file name

		return PDS_SUCCESS;						   // Return success

	}



	return PDS_FILE_ERROR; // Return file error

}



