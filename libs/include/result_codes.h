#ifndef __result_codes__
#define __result_codes__

typedef enum{
	False,
	True
}Boolean;

typedef enum{
	Success,
	Unknown_Failure,
	Memory_Failure,
	Empty_list,
	List_out_of_bounds,
	Rewind_list,
	Request_data_not_found,
	Request_data_found,
	File_exists,
	File_does_not_exist,
	Max_Error_Num
}Result;

typedef Result OK_SUCCESS;
#endif
