#include<stdio.h>


#include<stdlib.h>

#include<string.h>



#include "pds.h"

#include "contact.h"



int store_contacts( char *contact_data_file )   // Store the contacts from the file in the PDS

{

    FILE *cfptr;    // File pointer

    char C_line[500], token;    // Line buffer and token

    struct Contact c, dum;  // Contact and dummy contact




    cfptr = (FILE *) fopen(contact_data_file, "r"); // Open the file in read mode

    while(fgets(C_line, sizeof(C_line)-1, cfptr)){  // Read the file line by line



        sscanf(C_line, "%d%s%s", &(c.contact_id),c.contact_name,c.phone);   // Read the line and store the data in the contact structure


        print_contact( &c );    // Print the contact

        add_contact( &c );  // Add the contact to the PDS

    }   

}



void print_contact( struct Contact *c ) // Print the contact

{

	printf("%d,%s,%s\n", c->contact_id,c->contact_name,c->phone);   // Print the contact details

}



int search_contact( int contact_id, struct Contact *c ) // Search the contact by contact id

{

    return get_rec_by_ndx_key( contact_id, c );    // Get the record by the contact id

}         



int add_contact( struct Contact *c )    // Add the contact to the PDS

{

	int stat;   // Status



	stat = put_rec_by_key( c->contact_id, c );  // Put the record by the contact id



	if( stat != PDS_SUCCESS ){  // If the status is not success

		fprintf(stderr, "Unable to add contact with key %d. Error %d", c->contact_id, stat );   // Print the error message

		return CONTACT_FAILURE;

	}

	return stat;

}



int search_contact_by_phone( char *phone, struct Contact *c, int *io_count )    // Search the contact by phone

{

	if (get_rec_by_non_ndx_key(phone, c, &match_contact_phone, io_count) == 1)  // If the record is not found

    {

        return CONTACT_FAILURE; // Return failure

    }

    else

    {

        return CONTACT_SUCCESS; // Return success

    }



}



int match_contact_phone( void *rec, void *key ) // Match the contact by phone

{

	struct Contact * temp_Contact_Pointer = (struct Contact *)(rec);    // Temporary contact pointer

	char * T_char = (char *)(key);  // Temporary character pointer

	if (strcmp(T_char, temp_Contact_Pointer->phone) == 0)    // If the phone number matches

    {

        return 0;   // Return 0

    }

	else if (strcmp(T_char, temp_Contact_Pointer->phone) != 0)      // If the phone number does not match

	{

		return 1;

	}

	else    // If the phone number does not match


    {

        return 5;

    }



}