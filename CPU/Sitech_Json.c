// #include "Sitech_Json.h"
// static void getit(struct json_object *new_obj, const char *field)
// {
	// struct json_object *o = NULL;
	// if (!json_object_object_get_ex(new_obj, field, &o))
		// printf("Field %s does not exist\n", field);

	// enum json_type o_type = json_object_get_type(o);
	// printf("new_obj.%s json_object_get_type()=%s\n", field, json_type_to_name(o_type));
	// printf("new_obj.%s json_object_get_int()=%d\n", field, json_object_get_int(o));
	// printf("new_obj.%s json_object_get_int64()=%" PRId64 "\n", field, json_object_get_int64(o));
	// printf("new_obj.%s json_object_get_uint64()=%" PRIu64 "\n", field,
	       // json_object_get_uint64(o));
	// printf("new_obj.%s json_object_get_boolean()=%d\n", field, json_object_get_boolean(o));
	// printf("new_obj.%s json_object_get_double()=%f\n", field, json_object_get_double(o));
// }

// static void checktype_header()
// {
	// printf("json_object_is_type: %s,%s,%s,%s,%s,%s,%s\n", json_type_to_name(json_type_null),
	       // json_type_to_name(json_type_boolean), json_type_to_name(json_type_double),
	       // json_type_to_name(json_type_int), json_type_to_name(json_type_object),
	       // json_type_to_name(json_type_array), json_type_to_name(json_type_string));
// }
// static void checktype(struct json_object *new_obj, const char *field)
// {
	// struct json_object *o = new_obj;
	// if (field && !json_object_object_get_ex(new_obj, field, &o))
		// printf("Field %s does not exist\n", field);

	// printf("new_obj%s%-18s: %d,%d,%d,%d,%d,%d,%d\n", field ? "." : " ", field ? field : "",
	       // json_object_is_type(o, json_type_null), json_object_is_type(o, json_type_boolean),
	       // json_object_is_type(o, json_type_double), json_object_is_type(o, json_type_int),
	       // json_object_is_type(o, json_type_object), json_object_is_type(o, json_type_array),
	       // json_object_is_type(o, json_type_string));
// }
