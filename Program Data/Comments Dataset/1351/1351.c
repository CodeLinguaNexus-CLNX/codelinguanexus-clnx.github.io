/*
The provided code snippet appears to be a function named `s390_pci_generate_fid` that is responsible for generating a unique Feature ID (FID) for a PCI device in a s390 architecture environment. It returns the generated FID or an error if no free FID could be found.

Here's the breakdown:

This function is responsible for generating a unique Feature ID (FID) for a PCI device in a s390 architecture environment.

Here's the detail:
- It initializes the FID variable `fid` to 0.
- It enters a loop to iterate through FID values from 0 up to a maximum FID value `ZPCI_MAX_FID`.
- Within the loop, it checks if a device with the current FID exists by calling the function `s390_pci_find_dev_by_fid(fid)`. If no device is found with the current FID, it returns the FID, indicating that it is available for use.
- If the loop reaches the maximum FID value and no available FID is found, it breaks the loop and sets an error message using the `error_setg` function.
- Finally, it returns 0 if no available FID is found, indicating an error condition.

Needs to notice:
- When using this function, ensure that the `Error` pointer `errp` is properly initialized to capture any error messages.
- The function implements a simple linear search algorithm to find an available FID. Depending on the range of possible FID values and the frequency of FID usage, the performance of this algorithm may need to be considered.
- The maximum FID value `ZPCI_MAX_FID` is used as a constraint, and its value should be set appropriately based on the specifications of the s390 architecture and the PCI device management requirements.
- The function provides a mechanism to handle the case where no available FID is found, ensuring that an appropriate error message is set before returning.
This function seems to be an important part of managing PCI devices in a s390 environment, and its proper functioning is critical for device allocation and management.
*/
static uint32_t s390_pci_generate_fid(Error **errp)

{

    uint32_t fid = 0;



    while (fid <= ZPCI_MAX_FID) {

        if (!s390_pci_find_dev_by_fid(fid)) {

            return fid;

        }



        if (fid == ZPCI_MAX_FID) {

            break;

        }



        fid++;

    }



    error_setg(errp, "no free fid could be found");

    return 0;

}
