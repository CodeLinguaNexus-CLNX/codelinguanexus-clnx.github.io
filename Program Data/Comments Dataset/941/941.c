/*
This function is responsible for retrieving the boot index configuration for an NVMe (Non-Volatile Memory Express) controller.

Here's the detail:
The function retrieves the bootindex configuration from the NvmeCtrl object and uses the Visitor pattern to visit the integer value of the bootindex within the object. It utilizes the visit_type_int32 function to handle the visitation process and retrieve the bootindex value for further processing.

Needs to notice:
1. The function is likely part of the configuration and management interface for NVMe controllers within a virtualized environment.
2. Proper error handling and validation of the retrieved bootindex value should be in place to ensure correct behavior.
3. Changes to the bootindex configuration should be made following the appropriate protocols and considerations for the virtualized NVMe environment.
*/
static void nvme_get_bootindex(Object *obj, Visitor *v, void *opaque,

                                  const char *name, Error **errp)

{

    NvmeCtrl *s = NVME(obj);



    visit_type_int32(v, &s->conf.bootindex, name, errp);

}
