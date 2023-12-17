/*
This function is responsible for resolving a path component within an object hierarchy and returning the corresponding object.

Here's the detail:
The function takes a parent object and a path component as input. It then attempts to find a property corresponding to the given path component within the parent object using the object_property_find function. If the property is found, the function checks the type of the property. If it's a link, it returns the object referenced by the link. If it's a child property, it returns the corresponding opaque object. Otherwise, it returns NULL.

Needs to notice:
1. The function assumes that the object hierarchy and properties are correctly set up, and it relies on the object_property_find function to locate the appropriate property.
2. Error handling may be needed if the property lookup fails or if unexpected property types are encountered.
3. It's important to ensure that the parent object and the path component are valid to avoid unexpected behavior.
*/
Object *object_resolve_path_component(Object *parent, const gchar *part)

{

    ObjectProperty *prop = object_property_find(parent, part, NULL);

    if (prop == NULL) {

        return NULL;

    }



    if (object_property_is_link(prop)) {

        return *(Object **)prop->opaque;

    } else if (object_property_is_child(prop)) {

        return prop->opaque;

    } else {

        return NULL;

    }

}
