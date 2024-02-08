#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pds.h"

void bst_print_custom(struct BST_Node *root) // Print the BST


{

    if (root == NULL) // If the root is NULL

        return;

    else // Else

    {

        fwrite(root->data, sizeof(root->data), 1, repo_handle.pds_ndx_fp); // Write the data to the file

        bst_print_custom(root->left_child);                                // Print the left child

        bst_print_custom(root->right_child);                               // Print the right child

    }
}



struct PDS_RepoInfo repo_handle; // Repository handle



int pds_create(char *repo_name) // Create the repository

{

    char *T_dat = (char *)malloc(sizeof(repo_name));       // Allocate memory for the data file

    char *T_ndx = (char *)malloc(sizeof(repo_name));       // Allocate memory for the index file

    strcpy(T_dat, repo_name);                              // Copy the repository name to the data file

    strcpy(T_ndx, repo_name);                              // Copy the repository name to the index file

    strcat(T_dat, ".dat");                                 // Append .dat to the data file

    strcat(T_ndx, ".ndx");                                 // Append .ndx to the index file

    repo_handle.pds_data_fp = fopen(T_dat, "wb");          // Open the data file in write mode

    repo_handle.pds_ndx_fp = fopen(T_ndx, "wb");           // Open the index file in write mode

    if (repo_handle.pds_data_fp && repo_handle.pds_ndx_fp) // If the data file and index file are open

    {

        int zero = 0;                                          // Initialize zero

        fwrite(&zero, sizeof(int), 1, repo_handle.pds_ndx_fp); // Write zero to the index file

        fclose(repo_handle.pds_data_fp);                       // Close the data file

        fclose(repo_handle.pds_ndx_fp);                        // Close the index file

        repo_handle.repo_status = PDS_REPO_CLOSED;             // Set the repository status to closed

        free(T_dat);                                           // Free the memory allocated for the data file

        free(T_ndx);                                           // Free the memory allocated for the index file

        return PDS_SUCCESS;                                    // Return success

    }
    free(T_dat);           // Free the memory allocated for the data file

    free(T_ndx);           // Free the memory allocated for the index file

    return PDS_FILE_ERROR; // Return file error

}



int pds_open(char *repo_name, int rec_size) // Open the repository

{

    strcpy(repo_handle.pds_name, repo_name);         // Copy the repository name to the repository handle

    char *T_dat = (char *)malloc(sizeof(repo_name)); // Allocate memory for the data file
    strcpy(T_dat, repo_name);                        // Copy the repository name to the data file

    strcat(T_dat, ".dat");                           // Append .dat to the data file

    char *T_ndx = (char *)malloc(sizeof(repo_name)); // Allocate memory for the index file

    strcpy(T_ndx, repo_name);                        // Copy the repository name to the index file

    strcat(T_ndx, ".ndx");                           // Append .ndx to the index file

    if (repo_handle.repo_status == PDS_REPO_CLOSED)  // If the repository status is closed

    {

        repo_handle.pds_data_fp = fopen(T_dat, "rb+"); // Open the data file in read mode

        repo_handle.pds_ndx_fp = fopen(T_ndx, "rb+");  // Open the index file in read mode

        repo_handle.rec_size = rec_size;               // Set the record size

        if (pds_load_ndx() == PDS_SUCCESS)             // If the index is loaded successfully

        {

            fclose(repo_handle.pds_ndx_fp);          // Close the index file

            repo_handle.repo_status = PDS_REPO_OPEN; // Set the repository status to open

            free(T_dat);                             // Free the memory allocated for the data file

            free(T_ndx);                             // Free the memory allocated for the index file

            return PDS_SUCCESS;                      // Return success

        }

        else // Else

        {

            repo_handle.repo_status = PDS_REPO_OPEN; // Set the repository status to open

            free(T_dat);                             // Free the memory allocated for the data file

            free(T_ndx);                             // Free the memory allocated for the index file

            return PDS_NDX_SAVE_FAILED;              // Return index save failed
        }

    }

    if (repo_handle.repo_status == PDS_REPO_OPEN) // If the repository status is open

    {

        free(T_dat);                  // Free the memory allocated for the data file

        free(T_ndx);                  // Free the memory allocated for the index file

        return PDS_REPO_ALREADY_OPEN; // Return

    }

    free(T_dat);           // Free the memory allocated for the data file

    free(T_ndx);           // Free the memory allocated for the index file

    return PDS_FILE_ERROR; // Return file error

}

int pds_load_ndx() // Load the index

{
    int R_ndx = fread(&repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp);                // Read the index

    struct PDS_NdxInfo *T_Ndx_Arr = (struct PDS_NdxInfo *)malloc(R_ndx * sizeof(struct PDS_NdxInfo)); // Allocate memory for the index array

    fread(T_Ndx_Arr, sizeof(struct PDS_NdxInfo), R_ndx, repo_handle.pds_ndx_fp);                      // Read the index array

    if (R_ndx == 0)                                                                                   // If the index is empty

    {

        free(T_Ndx_Arr);            // Free the memory allocated for the index array

        return PDS_NDX_SAVE_FAILED; // Return index save failed
    }

    for (int i = 0; i < R_ndx; i++) // For each index

    {

        bst_add_node(&repo_handle.pds_bst, T_Ndx_Arr[i].key, &(T_Ndx_Arr[i])); // Add the index to the BST

    }

    free(T_Ndx_Arr);    // Free the memory allocated for the index array

    return PDS_SUCCESS; // Return success

}



int put_rec_by_key(int key, void *rec) // Put the record by the key
{

    if (repo_handle.repo_status == PDS_REPO_OPEN) // If the repository status is open

    {
        if (fseek(repo_handle.pds_data_fp, 0, SEEK_END) == 0) // If the file pointer is at the end of the file

        {

            struct BST_Node *T_BST_Node;                                     // BST node

            if ((T_BST_Node = bst_search(repo_handle.pds_bst, key)) == NULL) // If the BST node is not found

            {

                int offset = ftell(repo_handle.pds_data_fp);                                                 // Get the offset

                struct PDS_NdxInfo *T_Ndx_Struct = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo)); // Allocate memory for the index structure

                T_Ndx_Struct->key = key;                                                                     // Set the key

                T_Ndx_Struct->offset = offset;                                                               // Set the offset

                repo_handle.rec_count++;                                                                     // Increment the record count
                fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);                               // Write the record to the file

                int T_stat = bst_add_node(&repo_handle.pds_bst, key, T_Ndx_Struct);                          // Add the node to the BST

                if (T_stat == BST_SUCCESS)                                                                   // If the status is success

                {
                    return PDS_SUCCESS; // Return success

                }

                else // Else

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

        return PDS_REPO_NOT_OPEN; // Return repository not open
    }

    return PDS_ADD_FAILED; // Return add failed

}



int get_rec_by_ndx_key(int key, void *rec) // Get the record by the key
{

    if (repo_handle.repo_status == PDS_REPO_OPEN) // If the repository status is open

    {
        struct BST_Node *T_BST_Struct;                                     // BST node

        if ((T_BST_Struct = bst_search(repo_handle.pds_bst, key)) != NULL) // If the BST node is found


        {

            struct PDS_NdxInfo *T_Struct = (struct PDS_NdxInfo *)(T_BST_Struct->data); // Get the index structure
            if (fseek(repo_handle.pds_data_fp, T_Struct->offset, SEEK_SET) == 0)       // If the file pointer is at the offset

            {

                fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp); // Read the record

                return PDS_SUCCESS;                                           // Return success

            }
            return PDS_FILE_ERROR; // Return file error

        }

        return PDS_REC_NOT_FOUND; // Return record not found

    }
    return PDS_REPO_NOT_OPEN; // Return repository not open

}



int get_rec_by_non_ndx_key(void *non_ndx_key, void *rec, int (*matcher)(void *rec, void *non_ndx_key), int *io_count) // Get the record by the non index key
{

    if (repo_handle.repo_status == PDS_REPO_OPEN) // If the repository status is open

    {

        if (fseek(repo_handle.pds_data_fp, 0, SEEK_SET) == 0) // If the file pointer is at the start of the file

        {
            void *T_pointer = malloc(repo_handle.rec_size); // Allocate memory for the pointer

            for (int i = 0; i < repo_handle.rec_count; i++) // For each record

            {

                (*io_count)++;                                                      // Increment the count
                fread(T_pointer, repo_handle.rec_size, 1, repo_handle.pds_data_fp); // Read the record

                if (matcher(T_pointer, non_ndx_key) == 0)                           // If the record is found

                {
                    fseek(repo_handle.pds_data_fp, -repo_handle.rec_size, SEEK_CUR); // Set the file pointer to the start of the record

                    fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);    // Read the record

                    return 1;                                                        // Return 1

                }

            }
            return PDS_REC_NOT_FOUND; // Return record not found

        }

        return PDS_FILE_ERROR; // Return file error

    }
    return PDS_REPO_NOT_OPEN; // Return repository not open

}



int pds_close() // Close the repository

{
    char *T = (char *)malloc(sizeof(repo_handle.pds_name)); // Allocate memory for the repository name

    strcpy(T, repo_handle.pds_name);                        // Copy the repository name to the repository handle

    strcat(T, ".ndx");                                      // Append .ndx to the repository name

    repo_handle.pds_ndx_fp = fopen(T, "wb");                // Open the index file in write mode

    if (repo_handle.pds_ndx_fp != NULL)                     // If the index file is open
    {

        fwrite(&repo_handle.rec_count, sizeof(int), 1, repo_handle.pds_ndx_fp); // Write the record count to the index file

        bst_print_custom(repo_handle.pds_bst);                                  // Print the BST

        fclose(repo_handle.pds_data_fp);                                        // Close the data file

        fclose(repo_handle.pds_ndx_fp);                                         // Close the index file
        repo_handle.repo_status = PDS_REPO_CLOSED;                              // Set the repository status to closed

        free(T);                                                                // Free the memory allocated for the repository name

        return PDS_SUCCESS;                                                     // Return success

    }


    return PDS_FILE_ERROR; // Return file error

}



